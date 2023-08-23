// Dx3D9.cpp: implementation of the Dx3D9 class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "Dx3D9.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Dx3D9::Dx3D9()
{
	m_hWnd = NULL;

	m_rgb_cx = 0; m_rgb_cy = 0;

	m_lpD3D = NULL;
	m_lpD3DD = NULL;
	m_lpRGBback = NULL;
	m_lpRGBsnap = NULL;
	for(int i=0; i<MAX_IMG; i++)
	{
		m_lpImg[i] = NULL;
	}

	m_lpD3DS = NULL;

	m_pen = NULL;
	m_font12 = NULL;
	m_font18 = NULL;
}
Dx3D9::~Dx3D9()
{
}

BOOL Dx3D9::Init3D9()
{
	return TRUE;
}
void Dx3D9::Exit3D9()
{
	m_ico_Aud.Clear();
	m_ico_Mic.Clear();
	SAFE_RELEASE(m_pen);
	SAFE_RELEASE(m_font12);
	SAFE_RELEASE(m_font18);
	SAFE_RELEASE(m_lpD3DS);

	for(int i=0; i<MAX_IMG; i++)
	{
		SAFE_RELEASE(m_lpImg[i]);
	}
	SAFE_RELEASE(m_lpRGBsnap);
	SAFE_RELEASE(m_lpRGBback);
	SAFE_RELEASE(m_lpD3DD);
	SAFE_RELEASE(m_lpD3D);
}

BOOL Dx3D9::InitD3DDevice()
{
	SAFE_RELEASE(m_lpD3DD);

	D3DCAPS9 deviceCaps;
	HRESULT hr = m_lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps);
	if(FAILED(hr)) return FALSE;

	DWORD behaviorFlag = D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;
	behaviorFlag |= (deviceCaps.VertexProcessingCaps != 0) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	memset(&m_d3dpp, 0, sizeof(m_d3dpp));
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.hDeviceWindow = m_hWnd;
	m_d3dpp.BackBufferCount  = 1;
	m_d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	m_d3dpp.BackBufferWidth  = m_rgb_cx + 16;
	m_d3dpp.BackBufferHeight = m_rgb_cy + 16;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.Flags                      = D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;//(if use back->GetDC)
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	hr = m_lpD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_hWnd,
		behaviorFlag,
		&m_d3dpp,
		&m_lpD3DD);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}

BOOL Dx3D9::InitRGBBackSurface()
{
	SAFE_RELEASE(m_lpRGBback);

	m_d3dpp.BackBufferWidth  = m_rgb_cx + 16;
	m_d3dpp.BackBufferHeight = m_rgb_cy + 16;

	HRESULT hr = m_lpD3DD->Reset(&m_d3dpp);
	if(FAILED(hr)) return FALSE;

	hr = m_lpD3DD->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if(FAILED(hr)) return FALSE;

	hr = m_lpD3DD->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_lpRGBback);
	if(FAILED(hr)) return FALSE;

	return TRUE;
}

BOOL Dx3D9::Init_StsImgSurface()
{
	for(int i=0; i<MAX_IMG; i++)
	{
		SAFE_RELEASE(m_lpImg[i]);
		memset(&m_infImg[i], 0, sizeof(D3DXIMAGE_INFO));

		WCHAR szImgPath[MAX_PATH];
		swprintf_s(szImgPath, L"%s\\%s", _cmn_ImgPath(), szImgName[i]);

		HRESULT hr = D3DXGetImageInfoFromFile(szImgPath, &m_infImg[i]);
		if(FAILED(hr)) return FALSE;

		hr = m_lpD3DD->CreateOffscreenPlainSurface(
			m_infImg[i].Width, m_infImg[i].Height,
			D3DFMT_X8R8G8B8,
			D3DPOOL_DEFAULT,
			&(m_lpImg[i]),
			0);
		if(FAILED(hr)) return FALSE;

		hr = D3DXLoadSurfaceFromFile(m_lpImg[i], NULL, NULL, szImgPath, NULL, D3DX_DEFAULT, 0, NULL);
		if(FAILED(hr)) return FALSE;
	}
	return TRUE;
}

BOOL Dx3D9::Init_TextureSprite()
{
	m_ico_Aud.Clear();
	m_ico_Mic.Clear();
	SAFE_RELEASE(m_pen);
	SAFE_RELEASE(m_font12);
	SAFE_RELEASE(m_font18);
	SAFE_RELEASE(m_lpD3DS);

	HRESULT hr = D3DXCreateSprite(m_lpD3DD, &m_lpD3DS);
	if(FAILED(hr)) return FALSE;

	hr = D3DXCreateLine(m_lpD3DD, &m_pen);
	if(FAILED(hr)) return FALSE;

	m_pen->SetWidth(1.0f);
	m_pen->SetAntialias(false);

	D3DXFONT_DESC logFont;
	memset(&logFont, 0, sizeof(logFont));
	wcscpy_s(logFont.FaceName, L"Tahoma Bold");
	logFont.Weight = 400;
	logFont.CharSet = DEFAULT_CHARSET;

	logFont.Height = 12;
	hr = D3DXCreateFontIndirect(m_lpD3DD, &logFont, &m_font12);
	if(FAILED(hr)) return FALSE;

	logFont.Height = 18;
	hr = D3DXCreateFontIndirect(m_lpD3DD, &logFont, &m_font18);
	if(FAILED(hr)) return FALSE;

	WCHAR szPath[MAX_PATH];
	swprintf_s(szPath, L"%s//OSD//aud16.png", _cmn_IcoPath()); m_ico_Aud.Load(m_lpD3DD, 0, szPath);
	swprintf_s(szPath, L"%s//OSD//aud32.png", _cmn_IcoPath()); m_ico_Aud.Load(m_lpD3DD, 1, szPath);
	swprintf_s(szPath, L"%s//OSD//mic16.png", _cmn_IcoPath()); m_ico_Mic.Load(m_lpD3DD, 0, szPath);
	swprintf_s(szPath, L"%s//OSD//mic32.png", _cmn_IcoPath()); m_ico_Mic.Load(m_lpD3DD, 1, szPath);

	return TRUE;
}

void Dx3D9::Draw_3DRect(LPRECT lpDst, DX_SCREEN* pScrn)
{
	CRect reDst(lpDst);
	reDst.right -= 1; reDst.bottom -= 1;

	for(int i=0; i<2; i++)
	{
		vector<D3DXVECTOR2> vLst1, vLst2;
		vLst1.push_back(D3DXVECTOR2((float)reDst.left, (float)reDst.bottom));
		vLst1.push_back(D3DXVECTOR2((float)reDst.left, (float)reDst.top));
		vLst1.push_back(D3DXVECTOR2((float)reDst.right, (float)reDst.top));
		vLst2.push_back(D3DXVECTOR2((float)reDst.right, (float)reDst.top));
		vLst2.push_back(D3DXVECTOR2((float)reDst.right, (float)reDst.bottom));
		vLst2.push_back(D3DXVECTOR2((float)reDst.left, (float)reDst.bottom));

		D3DCOLOR col1, col2;
		if(pScrn->Is_Sel)
		{
			col1 = D3DCOLOR_XRGB(176,226,17);
			col2 = col1;
		}
		else
		{
			col1 = D3DCOLOR_XRGB(226,226,226);
			col2 = D3DCOLOR_XRGB(17,17,17);
		}

		m_pen->Draw(vLst1.data(), (DWORD)vLst1.size(), col1);
		m_pen->Draw(vLst2.data(), (DWORD)vLst2.size(), col2);

		reDst.DeflateRect(1,1);
	}
}
void Dx3D9::Draw_3DText(LPRECT lpDst, DX_SCREEN* pScrn)
{
	CRect reDst(lpDst);
	reDst.DeflateRect(2,2);
	reDst.right -= 1; reDst.bottom -= 1;

	D3DXCOLOR rgb = D3DCOLOR_XRGB(255,255,255);
	if(pScrn->Is_Sel) rgb = D3DCOLOR_XRGB(176,226,17);

	UINT nFormat = DT_TOP|DT_RIGHT|DT_NOCLIP;
	if(*pScrn->szText != L'\0')
	{
		CRect tr(&reDst);
		tr.DeflateRect(1, 1);
			
		OffsetRect(&tr,2,2);
		m_font18->DrawText(m_lpD3DS, pScrn->szText, -1, &tr, nFormat, D3DCOLOR_XRGB(0,0,0));

		OffsetRect(&tr,-1,-1);
		m_font18->DrawText(m_lpD3DS, pScrn->szText, -1, &tr, nFormat, rgb);
	}
}

void Dx3D9::Draw_DxIcon(LPRECT lpDst, DX_SCREEN* pScrn)
{
	CRect reDst(lpDst);
	reDst.DeflateRect(3, 3);

	int idx = 1;
	if(reDst.Width()*reDst.Height() < 400*300)
		idx = 0;

	int XPOS = 0;
	if(pScrn->Is_Aud) Draw_Icon(&reDst, &m_ico_Aud, XPOS);
	if(pScrn->Is_Mic) Draw_Icon(&reDst, &m_ico_Mic, XPOS);
}
void Dx3D9::Draw_Icon(LPRECT lpDst, PngObj* pIco, int& XPOS)
{
	int idx = 1;
	if((lpDst->right-lpDst->left)*(lpDst->bottom-lpDst->top) < 400*300)
		idx = 0;
	if(pIco->TX(idx))
	{
		D3DXVECTOR3 pt = D3DXVECTOR3((float)(lpDst->left + XPOS), (float)(lpDst->bottom - pIco->Cy(idx)), 0.f);
		m_lpD3DS->Draw(pIco->TX(idx), pIco->Re(idx), NULL, &pt, D3DXCOLOR(1.0, 1.0, 1.0, 1.0));
		XPOS += pIco->Cx(idx) + 1;
	}
}

void Dx3D9::Draw_Pt_Rect(LPRECT lpDst, DX_SCREEN* pScrn)
{
	DX_PT_RECT* pDxRect = &(pScrn->dx_Rect);
	if(pDxRect->IsRtObj())
	{
		CRect reDst(lpDst);
		reDst.right -= 1; reDst.bottom -= 1;

		CRect reSrc = pDxRect->Ref;
		for(int i=0; i<pDxRect->Cnt(); i++)
		{
			RtObj* pRt = pDxRect->Obj(i);
			if(pRt) DrawRtObj(&reSrc, &reDst, pRt);
		}
	}
}
void Dx3D9::DrawRtObj(LPRECT lpSrc, LPRECT lpDst, RtObj* pRt)
{
	CRect reSrc(lpSrc), reDst(lpDst);

	RtObj obj(pRt);
	if(!obj.SetScale(&reSrc, &reDst)) return;

	if(obj.b_mosaic)
	{
		if(!obj.re.IsRectEmpty()) DrawMosaic(&obj.re);
	}
	else if(obj.b_fill)
	{
		D3DCOLOR color = D3DCOLOR_COLRF(obj.cr, obj.alp_fill);

		vector<Vertex> vtx_array;
		int iCnt = RECT2Vertex(&obj.re, color, vtx_array);

		m_lpD3DD->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
		m_lpD3DD->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (UINT)iCnt, vtx_array.data(), sizeof(Vertex));
	}

	vector<D3DXVECTOR2> vLst;
	RECT2D3DXVECTOR(&obj.re, vLst);
	m_pen->Draw(vLst.data(), (DWORD)vLst.size(), D3DCOLOR_COLRF(obj.cr, 191));

	if(*obj.sz != L'\0')
	{
		D3DCOLOR col_text = D3DCOLOR_COLRF(obj.cr);
		D3DCOLOR col_back = obj.cr == RGB(0,0,0) ? D3DCOLOR_XRGB(255,255,255) : D3DCOLOR_XRGB(0,0,0);

		UINT nFormat = DT_LEFT|DT_TOP|DT_SINGLELINE|DT_NOCLIP;
		CRect tr(obj.re.left, obj.re.top, reDst.right, reDst.bottom);
		tr.DeflateRect(1,1);
		OffsetRect(&tr,2,2);
		m_font12->DrawText(m_lpD3DS, obj.sz, -1, &tr, nFormat, col_back);
		OffsetRect(&tr,-1,-1);
		m_font12->DrawText(m_lpD3DS, obj.sz, -1, &tr, nFormat, col_text);
	}
}
void Dx3D9::DrawMosaic(LPRECT lpDst)
{
	CRect reDst(lpDst);

	int width = reDst.right - reDst.left;
	int height = reDst.bottom - reDst.top;

	int h_num = 10;
	int v_num = 10;
	int h_cel = width/h_num;
	int v_cel = height/v_num;
	if(width < 200)
	{
		h_num = 1;
		h_cel = width;
		while(h_cel > 20){h_cel /= 2;h_num *= 2;}
	}
	if(height < 200)
	{
		v_num = 1;
		v_cel = height;
		while(v_cel > 20){v_cel /= 2;v_num *= 2;}
	}
	int h_off = reDst.left + (width - h_cel*h_num)/2;
	int v_off = reDst.top + (height - v_cel*v_num)/2;

	D3DCOLOR color[2];
	color[0] = D3DCOLOR_ARGB(127,0,0,0);
	color[1] = D3DCOLOR_ARGB(127,255,255,255);

	D3DCOLOR * col_lock = NULL;

	D3DLOCKED_RECT locked;
	HRESULT hr = m_lpRGBback->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
	if(SUCCEEDED(hr))
	{
		col_lock = new D3DCOLOR [h_num * v_num];

		DWORD* pixels = (DWORD*)locked.pBits;
		int Pitch = locked.Pitch/4;

		for(int col=0; col<v_num; col++)
		{
			for(int row=0; row<h_num; row++)
			{
				CRect reCel(
					(row == 0) ? reDst.left : h_off + h_cel*row,
					(col == 0) ? reDst.top : v_off + v_cel*col,
					(row == h_num-1) ? reDst.right : h_off + h_cel*row + h_cel,
					(col == v_num-1) ? reDst.bottom : v_off + v_cel*col + v_cel
					);
				CPoint pt = reCel.CenterPoint();
				col_lock[h_num*col + row] = (pixels[pt.y*Pitch + pt.x] | 0xff000000);
			}
		}
		m_lpRGBback->UnlockRect();
	}

	for(int col=0; col<v_num; col++)
	{
		for(int row=0; row<h_num; row++)
		{
			int idx = row % 2; if(col%2) idx = 1 - idx;

			CRect reCel(
				(row == 0) ? reDst.left : h_off + h_cel*row,
				(col == 0) ? reDst.top : v_off + v_cel*col,
				(row == h_num-1) ? reDst.right : h_off + h_cel*row + h_cel,
				(col == v_num-1) ? reDst.bottom : v_off + v_cel*col + v_cel
				);
			if(col_lock) color[idx] = col_lock[h_num*col + row];

			vector<Vertex> vtx_array;
			int iCnt = RECT2Vertex(&reCel, color[idx], vtx_array);

			m_lpD3DD->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			m_lpD3DD->DrawPrimitiveUP(D3DPT_TRIANGLELIST, (UINT)iCnt, vtx_array.data(), sizeof(Vertex));
		}
	}

	SAFE_DELETEA(col_lock);
}
