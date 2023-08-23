// RenderSCtrl.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "RenderSCtrl.h"

IMPLEMENT_DYNAMIC(CRenderSCtrl, CWnd)

CRenderSCtrl::CRenderSCtrl()
{
	m_bInitial = FALSE;
	m_tracking = false;

	m_pdx = NULL;

	m_fLButtonDn = 0;

	m_hHandUp = NULL;
	m_hHandDn = NULL;
}

CRenderSCtrl::~CRenderSCtrl()
{
}

BEGIN_MESSAGE_MAP(CRenderSCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

// CRenderSCtrl 메시지 처리기입니다.

BOOL CRenderSCtrl::Create(DWORD dwStyle, CWnd* pParent)
{
	LPCTSTR lpszClassName =
		AfxRegisterWndClass(CS_DBLCLKS);
	return CWnd::CreateEx(0,lpszClassName,L"CRenderSCtrl",dwStyle,0,0,0,0,pParent->GetSafeHwnd(),NULL);
}

BOOL CRenderSCtrl::DxCheck()
{
	if(m_pdx)
	{
		if(!_dxs_DxCheck(m_pdx))
			return FALSE;
	}
	return TRUE;
}
Cdx_control* CRenderSCtrl::GetPdx()
{
	return m_pdx;
}

bool CRenderSCtrl::SetCur(UINT uiCur)
{
	switch(uiCur)
	{
	case DxCur_No:
	case DxCur_Hand:
	case DxCur_Arrow:
	case DxCur_Cross:
	case DxCur_HandUp:
	case DxCur_HandDn: SetCursor(GetCur(uiCur)); return true;
	}
	return false;
}
HCURSOR CRenderSCtrl::GetCur(UINT uiCur)
{
	switch(uiCur)
	{
	case DxCur_No:
		return LoadCursor(NULL, IDC_NO); break;
	case DxCur_Hand:
		return LoadCursor(NULL, IDC_HAND); break;
	case DxCur_Arrow:
		return LoadCursor(NULL, IDC_ARROW); break;
	case DxCur_Cross:
		return LoadCursor(NULL, IDC_CROSS); break;
	case DxCur_HandUp: if(m_hHandUp) return m_hHandUp; break;
	case DxCur_HandDn: if(m_hHandDn) return m_hHandDn; break;
	}
	return LoadCursor(NULL, IDC_ARROW);
}

int CRenderSCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pdx = _dxs_Init(this);
	if(m_pdx)
	{
		_dxs_SendCleanMsg(m_pdx, SCR_IMG_LOGO);
	}

	WCHAR szPath[MAX_PATH];

	swprintf_s(szPath, L"%s\\hand_rel.cur", _cmn_CurPath());
	m_hHandUp = (HCURSOR)::LoadImage(NULL, szPath, IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);

	swprintf_s(szPath, L"%s\\hand_gri.cur", _cmn_CurPath());
	m_hHandDn = (HCURSOR)::LoadImage(NULL, szPath, IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);

	m_bInitial = TRUE;
	return 0;
}

void CRenderSCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	m_bInitial = FALSE;

	if(m_pdx)
	{
		_dxs_Exit(m_pdx);
		m_pdx = NULL;
	}

	if(m_hHandUp) DestroyCursor(m_hHandUp);
	if(m_hHandDn) DestroyCursor(m_hHandDn);
}

void CRenderSCtrl::OnPaint()
{
	if (!m_bInitial) 
		return;

	CPaintDC dc(this);

	if(m_pdx)
	{
		_dxs_Send_Refresh(m_pdx);
	}
}

void CRenderSCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(!m_bInitial || cx <= 0)
		return;

	if(m_pdx)
	{
		_dxs_ChangeSplit(m_pdx);
	}
}

void CRenderSCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
	if(m_bInitial)
	{
		CWnd* pWnd = GetParent();
		if(pWnd)
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			pWnd->ScreenToClient(&pt);
			pWnd->SendMessage(WM_RBUTTONDOWN, nFlags, MAKELPARAM(pt.x, pt.y));
		}
	}
}
void CRenderSCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
	if(m_bInitial)
	{
		CWnd* pWnd = GetParent();
		if(pWnd)
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			pWnd->ScreenToClient(&pt);
			pWnd->SendMessage(WM_RBUTTONUP, nFlags, MAKELPARAM(pt.x, pt.y));
		}
	}
}
void CRenderSCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	if(m_bInitial)
	{
		bool bCurSet = false;

		CWnd* pWnd = GetParent();
		if(pWnd)
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			pWnd->ScreenToClient(&pt);
			pWnd->SendMessage(WM_LBUTTONDOWN, nFlags, MAKELPARAM(pt.x, pt.y));
		}
		m_fLButtonDn = 1;

		if(!bCurSet) SetCur(DxCur_Arrow);
	}
}
void CRenderSCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	if(m_bInitial)
	{
		bool bCurSet = false;

		if(m_fLButtonDn) m_fLButtonDn = 2;

		if(!m_tracking)
		{
			TRACKMOUSEEVENT t = {
				sizeof(TRACKMOUSEEVENT),
				TME_LEAVE,
				GetSafeHwnd(),
				0
			};
			if(::_TrackMouseEvent(&t)) m_tracking = true;
		}

		if(!bCurSet) SetCur(DxCur_Arrow);
	}
}
void CRenderSCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	if(m_bInitial)
	{
		bool bCurSet = false;

		CWnd* pWnd = GetParent();
		if(pWnd)
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			pWnd->ScreenToClient(&pt);
			pWnd->SendMessage(WM_LBUTTONUP, nFlags, MAKELPARAM(pt.x, pt.y));
		}
		m_fLButtonDn = 0;

		if(!bCurSet) SetCur(DxCur_Arrow);
	}
}
void CRenderSCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
	if(m_bInitial)
	{
		CWnd* pWnd = GetParent();
		if(pWnd)
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			pWnd->ScreenToClient(&pt);
			pWnd->SendMessage(WM_LBUTTONDBLCLK, nFlags, MAKELPARAM(pt.x, pt.y));
		}
	}
}
void CRenderSCtrl::OnMouseLeave()
{
	CWnd::OnMouseLeave();
	if(m_bInitial)
	{
		m_fLButtonDn = 0;

		m_tracking = false;
	}
}
