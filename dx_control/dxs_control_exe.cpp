// dxs_control_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "dxs_control_exe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*****************************************************************************
 *
 *  dxs_control
 *
 ****************************************************************************/
dxs_control::dxs_control()
{
	m_pWnd = NULL;
	m_pD3d = NULL;

	m_bActive = FALSE;
}
dxs_control::~dxs_control()
{
}

BOOL dxs_control::Init(CWnd * pWnd)
{
	m_pWnd = pWnd;
	if(!m_pWnd->GetSafeHwnd()) return FALSE;

	if(m_pD3d==NULL)
	{
		m_pD3d = new Dxs3D9;
		WCHAR * errorString = m_pD3d->Init(pWnd);
		if(errorString)
		{
			delete m_pD3d;
			m_pD3d = NULL;
		}
	}

	m_bActive = TRUE;

	return TRUE;
}
void dxs_control::Exit()
{
	m_bActive = FALSE;

	if(m_pD3d != NULL)
	{
		delete m_pD3d;
		m_pD3d = NULL;
	}
}
BOOL dxs_control::DxCheck()
{
	if(m_bActive)
	{
		if(m_pD3d){
			return m_pD3d->DxCheck();
		}
	}
	return TRUE;
}
void* dxs_control::GetD3DD()
{
	if(m_bActive)
	{
		if(m_pD3d){
			return m_pD3d->GetD3DD();
		}
	}
	return NULL;
}

DX_SCREEN* dxs_control::GetDxScrn()
{
	DX_SCREEN * pScrn = NULL;
	if(m_pD3d){
		pScrn = m_pD3d->GetDxScrn();
	}
	return pScrn;
}
CRect dxs_control::GetScreen()
{
	CRect reRet;
	m_pWnd->GetClientRect(&reRet);
	return reRet;
}

int dxs_control::GetStatus()
{
	DX_SCREEN * pScrn = GetDxScrn();
	if(pScrn) return pScrn->Status;
	return -1;
}

void dxs_control::ScreenSplit()
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->ScreenSplit();
		}
	}
}
void dxs_control::ScreenCover()
{
	if(m_bActive)
	{
		m_bActive = FALSE;
		if(m_pD3d){
			m_pD3d->ClearSurface();
		}
	}
}
void dxs_control::ChangeSplit()
{
	if(m_bActive)
	{
		Send_Refresh();
	}
}

void dxs_control::SetSelCh(BOOL bSelCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(!pScrn) return;

		if(bSelCh)
			 pScrn->Is_Sel = 1;
		else pScrn->Is_Sel = 0;
		Send_Refresh();
	}
}
BOOL dxs_control::GetSelCh()
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(pScrn) if(pScrn->Is_Sel) return TRUE;
	}
	return FALSE;
}
void dxs_control::SetAudCh(BOOL bAudCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(!pScrn) return;

		if(bAudCh)
			 pScrn->Is_Aud = 1;
		else pScrn->Is_Aud = 0;
		Send_Refresh();
	}
}
BOOL dxs_control::GetAudCh()
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(pScrn) if(pScrn->Is_Aud) return TRUE;
	}
	return FALSE;
}
void dxs_control::SetMicCh(BOOL bMicCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(!pScrn) return;

		if(bMicCh)
			 pScrn->Is_Mic = 1;
		else pScrn->Is_Mic = 0;
		Send_Refresh();
	}
}
BOOL dxs_control::GetMicCh()
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(pScrn) if(pScrn->Is_Mic) return TRUE;
	}
	return FALSE;
}
void dxs_control::SetText(WCHAR* szText)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(!pScrn) return;

		wcscpy_s(pScrn->szText, szText);
		Send_Refresh();
	}
}
WCHAR* dxs_control::GetText()
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(pScrn) return pScrn->szText;
	}
	return NULL;
}

DX_PT_RECT* dxs_control::GetPt_Rect()
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(pScrn) return &(pScrn->dx_Rect);
	}
	return NULL;
}
BOOL dxs_control::GetPt_Rect(DX_PT_RECT* pDxRect)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn();
		if(pScrn)
		{
			*pDxRect = pScrn->dx_Rect;
			return TRUE;
		}
	}
	return FALSE;
}
void dxs_control::SetPt_Rect(DX_PT_RECT* pDxRect)
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->SetPt_Rect(pDxRect);
		}
	}
}

void dxs_control::SendImageSrc(VID_IMGSRC* pVid_img)
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->SendImageSrc(pVid_img);
		}
	}
}
void dxs_control::SendCleanMsg(int STATUS)
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->SendCleanMsg(STATUS);
		}
	}
}
void dxs_control::Send_Refresh()
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->Send_Refresh();
		}
	}
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

Cdx_control* _dxs_Init(CWnd * pWnd)
{
	dxs_control* pdx_S = new dxs_control;
	if(pdx_S)
	{
		if(pdx_S->Init(pWnd))
		{
			return (Cdx_control*)pdx_S;
		}
		else
		{
			pdx_S->Exit();
			delete pdx_S;
			pdx_S = NULL;
		}
	}
	return NULL;
}
void _dxs_Exit(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->Exit();
		delete pdx_S;
		pdx_S = NULL;
	}
}
CWnd* _dxs_GetWnd(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetWnd();
	}
	return NULL;
}
BOOL _dxs_DxCheck(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->DxCheck();
	}
	return TRUE;
}
void* _dxs_GetD3DD(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetD3DD();
	}
	return NULL;
}

void* _dxs_GetDxScrn(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return (void*)(pdx_S->GetDxScrn());
	}
	return NULL;
}
CRect _dxs_GetScreen(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetScreen();
	}
	return CRect(0,0,0,0);
}

int _dxs_GetStatus(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetStatus();
	}
	return -1;
}

void _dxs_ScreenSplit(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->ScreenSplit();
	}
}
void _dxs_ScreenCover(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->ScreenCover();
	}
}
void _dxs_ChangeSplit(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->ChangeSplit();
	}
}

void _dxs_SetSelCh(void* Ptr, BOOL bSelCh)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SetSelCh(bSelCh);
	}
}
BOOL _dxs_GetSelCh(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetSelCh();
	}
	return FALSE;
}
void _dxs_SetAudCh(void* Ptr, BOOL bAudCh)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SetAudCh(bAudCh);
	}
}
BOOL _dxs_GetAudCh(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetAudCh();
	}
	return FALSE;
}
void _dxs_SetMicCh(void* Ptr, BOOL bMicCh)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SetMicCh(bMicCh);
	}
}
BOOL _dxs_GetMicCh(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetMicCh();
	}
	return FALSE;
}
void _dxs_SetText(void* Ptr, WCHAR* szText)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SetText(szText);
	}
}
WCHAR* _dxs_GetText(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetText();
	}
	return NULL;
}

DX_PT_RECT* _dxs_GetPt_Rect(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetPt_Rect();
	}
	return NULL;
}
BOOL _dxs_GetPt_Rect(void* Ptr, DX_PT_RECT* pDxRect)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		return pdx_S->GetPt_Rect(pDxRect);
	}
	return FALSE;
}
void _dxs_SetPt_Rect(void* Ptr, DX_PT_RECT* pDxRect)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SetPt_Rect(pDxRect);
	}
}

void _dxs_SendImageSrc(void* Ptr, VID_IMGSRC* pVid_img)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SendImageSrc(pVid_img);
	}
}
void _dxs_SendCleanMsg(void* Ptr, int STATUS)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->SendCleanMsg(STATUS);
	}
}
void _dxs_Send_Refresh(void* Ptr)
{
	if(Ptr)
	{
		dxs_control* pdx_S = (dxs_control*)Ptr;
		pdx_S->Send_Refresh();
	}
}
