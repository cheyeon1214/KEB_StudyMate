// RenderCtrl.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "RenderCtrl.h"

IMPLEMENT_DYNAMIC(CRenderCtrl, CWnd)

CRenderCtrl::CRenderCtrl()
{
	m_bInitial = FALSE;
	m_tracking = false;

	m_pdx = NULL;

	m_SelCh = -1;
	m_SelDiv = -1;
	m_SelRot = -1;

	m_fLButtonDn = 0;

	m_hHandUp = NULL;
	m_hHandDn = NULL;
}

CRenderCtrl::~CRenderCtrl()
{
}

BEGIN_MESSAGE_MAP(CRenderCtrl, CWnd)
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

// CRenderCtrl 메시지 처리기입니다.

BOOL CRenderCtrl::Create(DWORD dwStyle, CWnd* pParent)
{
	LPCTSTR lpszClassName =
		AfxRegisterWndClass(CS_DBLCLKS);
	return CWnd::CreateEx(0,lpszClassName,L"CRenderCtrl",dwStyle,0,0,0,0,pParent->GetSafeHwnd(),NULL);
}

BOOL CRenderCtrl::DxCheck()
{
	if(m_pdx)
	{
		if(!_dxm_DxCheck(m_pdx))
			return FALSE;
	}
	return TRUE;
}
Cdx_control* CRenderCtrl::GetPdx()
{
	return m_pdx;
}
Cdx_control* CRenderCtrl::ReCreateDx()
{
	if(m_bInitial)
	{
		int SelCh = -1;
		DX_Config dx_Cfg;
		int status[MAX_SCH];
		for(int sc=0; sc<_UseSch(); sc++) status[sc] = SCR_IMG_LOGO;

		if(m_pdx)
		{
			SelCh = _dxm_GetSelCh(m_pdx);
			dx_Cfg = *((DX_Config*)_dxm_GetDxCfg(m_pdx));
			for(int sc=0; sc<_UseSch(); sc++) status[sc] = _dxm_GetStatus(m_pdx, sc);

			_dxm_ScreenCover(m_pdx);
			_dxm_Exit(m_pdx);
			m_pdx = NULL;
		}

		m_pdx = _dxm_Init(this);
		if(m_pdx)
		{
			for(int sc=0; sc<_UseSch(); sc++)
			{
				if(status[sc] == SCR_CAM_CONNECT)
					_dxm_SendCleanMsg(m_pdx, sc, SCR_IMG_BLK);
				else _dxm_SendCleanMsg(m_pdx, sc, status[sc]);
			}
			_dxm_ScreenSplit(m_pdx);
			if(_UseSch() < 16)
				_dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, SCR_NXN_04);
			else _dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, SCR_NXN_16);

			if(SelCh > -1)
			{
				_dxm_SetDxCfg(m_pdx, (void*)&dx_Cfg);
				_dxm_SetSelCh(m_pdx, SelCh, TRUE);
			}

			return m_pdx;
		}
	}
	return NULL;
}

bool CRenderCtrl::SetCur(UINT uiCur)
{
	switch(uiCur)
	{
	case DxCur_No:
	case DxCur_Wait:
	case DxCur_Hand:
	case DxCur_Arrow:
	case DxCur_Cross:
	case DxCur_HandUp:
	case DxCur_HandDn: SetCursor(GetCur(uiCur)); return true;
	}
	return false;
}
HCURSOR CRenderCtrl::GetCur(UINT uiCur)
{
	switch(uiCur)
	{
	case DxCur_No:
		return LoadCursor(NULL, IDC_NO); break;
	case DxCur_Wait:
		return LoadCursor(NULL, IDC_WAIT); break;
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

int CRenderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pdx = _dxm_Init(this);
	if(m_pdx)
	{
		for(int sc=0; sc<_UseSch(); sc++)
		{
			_dxm_SendCleanMsg(m_pdx, sc, SCR_IMG_LOGO);
		}
		_dxm_ScreenSplit(m_pdx);
		if(_UseSch() < 16)
			_dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, SCR_NXN_04);
		else _dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, SCR_NXN_16);
	}

	WCHAR szPath[MAX_PATH];

	swprintf_s(szPath, L"%s\\hand_rel.cur", _cmn_CurPath());
	m_hHandUp = (HCURSOR)::LoadImage(NULL, szPath, IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);

	swprintf_s(szPath, L"%s\\hand_gri.cur", _cmn_CurPath());
	m_hHandDn = (HCURSOR)::LoadImage(NULL, szPath, IMAGE_CURSOR, 32, 32, LR_LOADFROMFILE);

	m_bInitial = TRUE;

	return 0;
}

void CRenderCtrl::OnDestroy()
{
	CWnd::OnDestroy();

	m_bInitial = FALSE;

	if(m_pdx)
	{
		_dxm_ScreenCover(m_pdx);
		_dxm_Exit(m_pdx);
		m_pdx = NULL;
	}

	if(m_hHandUp) DestroyCursor(m_hHandUp);
	if(m_hHandDn) DestroyCursor(m_hHandDn);
}

void CRenderCtrl::OnPaint()
{
	if(!m_bInitial) 
		return;

	CPaintDC dc(this);

	if(m_pdx)
	{
		_dxm_ScreenSplit(m_pdx);
	}
}

void CRenderCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(!m_bInitial || cx <= 0)
		return;

	if(m_pdx)
	{
		_dxm_ChangeSplit(m_pdx);
	}
}

void CRenderCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
	if(m_bInitial && m_pdx)
	{
		int nCh = -1;
		for(int ch=0; ch<_UseSch(); ch++)
		{
			if(_dxm_GetScreen(m_pdx, ch).PtInRect(point))
			{
				nCh = ch;
				break;
			}
		}
		int SelCh = _dxm_GetSelCh(m_pdx);

		if(SelCh != nCh)
		{
			_dxm_SetSelCh(m_pdx, nCh, TRUE);
			SelCh = nCh;
		}

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
void CRenderCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
	if(m_bInitial && m_pdx)
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
void CRenderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	if(m_bInitial && m_pdx)
	{
		bool bCurSet = false;

		SetFocus();

		int nCh = -1;
		for(int ch=0; ch<_UseSch(); ch++)
		{
			if(_dxm_GetScreen(m_pdx, ch).PtInRect(point))
			{
				nCh = ch;
				break;
			}
		}
		int SelCh = _dxm_GetSelCh(m_pdx);

		if(SelCh != nCh)
		{
			_dxm_SetSelCh(m_pdx, nCh, TRUE);
			SelCh = nCh;
		}

		bCurSet = SetCur(DxCur_HandDn);

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
void CRenderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	if(m_bInitial && m_pdx)
	{
		bool bCurSet = false;

		int nCh = -1;
		for(int ch=0; ch<_UseSch(); ch++)
		{
			if(_dxm_GetScreen(m_pdx, ch).PtInRect(point))
			{
				nCh = ch;
				break;
			}
		}
		int SelCh = _dxm_GetSelCh(m_pdx);

		if(m_fLButtonDn)
		{
			bCurSet = SetCur(DxCur_HandDn);
		}
		else
		{
			bCurSet = SetCur(DxCur_HandUp);
		}

		/*CWnd* pWnd = GetParent();
		if(pWnd)
		{
			CPoint pt(point);
			ClientToScreen(&pt);
			pWnd->ScreenToClient(&pt);
			pWnd->SendMessage(WM_MOUSEMOVE, nFlags, MAKELPARAM(pt.x, pt.y));
		}*/
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
void CRenderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	if(m_bInitial && m_pdx)
	{
		bool bCurSet = false;

		int nCh = -1;
		for(int ch=0; ch<_UseSch(); ch++)
		{
			if(_dxm_GetScreen(m_pdx, ch).PtInRect(point))
			{
				nCh = ch;
				break;
			}
		}
		int SelCh = _dxm_GetSelCh(m_pdx);

		if(m_fLButtonDn)
		{
			if(SelCh == nCh)
			{
				bCurSet = SetCur(DxCur_HandUp);
			}
			else
			{
				bCurSet = SetCur(DxCur_HandUp);

				_dxm_SetSwapCh(m_pdx, SelCh, nCh);
			}
		}

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
void CRenderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
	if(m_bInitial && m_pdx)
	{
		if(_dxm_GetSelDiv(m_pdx) > SCR_NXN_01)
		{
			m_SelCh = _dxm_GetSelCh(m_pdx);
			m_SelDiv = _dxm_GetSelDiv(m_pdx);
			m_SelRot = _dxm_GetSelRot(m_pdx);
			_dxm_SetDivMode(m_pdx, MSG_SELVCHANNEL, m_SelCh);
		}
		else
		{
			if((m_SelCh > -1) && (m_SelDiv > -1) && (m_SelRot > -1))
			{
				_dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, m_SelDiv, m_SelRot);
				_dxm_SetSelCh(m_pdx, m_SelCh, TRUE);
			}
			else
			{
				if(_UseSch() < 16)
					_dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, SCR_NXN_04);
				else _dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, SCR_NXN_16);
			}
			m_SelCh = -1;
			m_SelDiv = -1;
			m_SelRot = -1;
		}

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
void CRenderCtrl::OnMouseLeave()
{
	CWnd::OnMouseLeave();
	if(m_bInitial)
	{
		m_fLButtonDn = 0;

		m_tracking = false;
	}
}
