// Dxm3D9.cpp: implementation of the Dxm3D9 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Dxm3D9.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Dxm3D9::Dxm3D9()
{
}
Dxm3D9::~Dxm3D9()
{
	Shutdown();
}

WCHAR * Dxm3D9::Init(CWnd * pWnd, WCHAR* szIniPath)
{
	m_hWnd = pWnd->GetSafeHwnd();

	m_rgb_cx = GetSystemMetrics(SM_CXSCREEN);
	m_rgb_cy = GetSystemMetrics(SM_CYSCREEN);

	static WCHAR errorMessage[200];

	SAFE_RELEASE(m_lpD3D);

	m_lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_lpD3D)
	{
		wcscpy_s (errorMessage, L"Error in Direct3DCreate9()");
		return (errorMessage);
	}

	if(!InitD3DDevice())
	{
		wcscpy_s (errorMessage, L"Error in InitD3DDevice()");
		return (errorMessage);
	}
	if(!InitRGBBackSurface())
	{
		wcscpy_s (errorMessage, L"Error in InitRGBBackSurface()");
		return (errorMessage);
	}
	if(!Init_StsImgSurface())
	{
		wcscpy_s (errorMessage, L"Error in Init_StsImgSurface()");
		return (errorMessage);
	}
	if(!Init_TextureSprite())
	{
		wcscpy_s (errorMessage, L"Error in Init_TextureSprite()");
		return (errorMessage);
	}

	Init3D9();

	return NULL;
}
void Dxm3D9::Shutdown()
{
	for(int ch=0; ch<MAX_SCH; ch++)
		m_Offscrn[ch].Clear();

	Exit3D9();
}
BOOL Dxm3D9::DxCheck()
{
	if(m_lpD3DD)
	{
		HRESULT hr = m_lpD3DD->TestCooperativeLevel();
		if(hr == D3DERR_DEVICELOST)
		{
		}
		else if(hr == D3DERR_DEVICENOTRESET)
		{
			return FALSE;
		}
	}
	return TRUE;
}
void* Dxm3D9::GetD3DD()
{
	return (void*)m_lpD3DD;
}

BOOL Dxm3D9::DrawSCHSurface(int nCh, LPRECT lpDst, LPRECT lpSrc)
{
	DX_SCREEN * pScrn = GetDxScrn(nCh);
	if(!pScrn->DxSurf) return FALSE;
	if(!m_lpRGBback) return FALSE;

	CRect reDst(lpDst);

	HRESULT hr = m_lpD3DD->StretchRect(
		(IDirect3DSurface9*)pScrn->DxSurf, lpSrc,
		m_lpRGBback, &reDst,
		D3DTEXF_NONE
	);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}

BOOL Dxm3D9::DrawINFSurface(int nCh, LPRECT lpDst)
{
	DX_SCREEN * pScrn = GetDxScrn(nCh);
	if(!m_lpD3DS) return FALSE;

	m_lpD3DD->BeginScene();
	m_lpD3DS->Begin(D3DXSPRITE_ALPHABLEND);

	if(pScrn->Status == SCR_CAM_CONNECT)
	{
		Draw_3DText(lpDst, pScrn);
		Draw_DxIcon(lpDst, pScrn);
		Draw_Pt_Rect(lpDst, pScrn);
	}
	Draw_3DRect(lpDst, pScrn);

	m_lpD3DS->End();
	m_lpD3DD->EndScene();

	return TRUE;
}

void Dxm3D9::RenderSurface(LPRECT lpDst, LPRECT lpSrc)
{
	HRESULT hr = m_lpD3DD->Present(lpSrc, lpDst, NULL, NULL);
}

void Dxm3D9::SendImageSrc(int nCh, LPRECT lpDst, VID_IMGSRC* pVid_img)
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	if(!m_Offscrn[nCh].Draw(m_lpD3DD, pVid_img)) return;

	DX_SCREEN * pScrn = GetDxScrn(nCh);

	pScrn->DxSurf = m_Offscrn[nCh].lpOffSurf;
	pScrn->H_size = m_Offscrn[nCh].hsize;
	pScrn->V_size = m_Offscrn[nCh].vsize;
	pScrn->Status = SCR_CAM_CONNECT;

	if(IsRectEmpty(lpDst))
		return;

	CRect reDst(lpDst);

	CRect reSrc;
	reSrc = pScrn->GetRect();

	DrawSCHSurface(nCh, &reDst, &reSrc);

	ZCsLock csl(&m_csDraw);

	DrawINFSurface(nCh, &reDst);
	RenderSurface(&reDst, &reDst);
}
void Dxm3D9::SendCleanMsg(int nCh, LPRECT lpDst, int STATUS)
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	DX_SCREEN * pScrn = GetDxScrn(nCh);
	if((pScrn->Status == STATUS) && pScrn->DxSurf) return;

	m_Offscrn[nCh].Clear();

	pScrn->Default();
	pScrn->DxSurf = m_lpImg[STATUS];
	pScrn->H_size = m_infImg[STATUS].Width;
	pScrn->V_size = m_infImg[STATUS].Height;
	pScrn->Status = STATUS;

	if(IsRectEmpty(lpDst))
		return;

	CRect reDst(lpDst);

	CRect reSrc;
	reSrc = pScrn->GetRect();

	DrawSCHSurface(nCh, &reDst, &reSrc);

	ZCsLock csl(&m_csDraw);

	DrawINFSurface(nCh, &reDst);
	RenderSurface(&reDst, &reDst);
}
void Dxm3D9::Send_Refresh(int nCh, LPRECT lpDst)
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	DX_SCREEN * pScrn = GetDxScrn(nCh);

	if(IsRectEmpty(lpDst))
		return;

	CRect reDst(lpDst);

	CRect reSrc;
	reSrc = pScrn->GetRect();

	DrawSCHSurface(nCh, &reDst, &reSrc);

	ZCsLock csl(&m_csDraw);

	DrawINFSurface(nCh, &reDst);
	RenderSurface(&reDst, &reDst);
}

DX_SCREEN* Dxm3D9::GetDxScrn(int nCh)
{
	return &(m_dxScrn[nCh]);
}
void Dxm3D9::SetPt_Rect(int nCh, DX_PT_RECT* pDxRect)
{
	ZCsLock csl(&m_csDraw);
	m_dxScrn[nCh].SetPt_Rect(pDxRect);
}

void Dxm3D9::ScreenSplit()
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	ZCsLock csl(&m_csDraw);

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);

	RenderSurface(&reDst, &reDst);
}
void Dxm3D9::ClearSurface()
{
	HRESULT hr = m_lpD3DD->TestCooperativeLevel();
	if(FAILED(hr)) return;

	ZCsLock csl(&m_csDraw);

	CRect reDst;
	GetClientRect(m_hWnd, &reDst);

	m_lpD3DD->ColorFill(m_lpRGBback, &reDst, D3DCOLOR_XRGB(0, 0, 0));
}
