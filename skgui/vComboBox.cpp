// vComboBox.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "vComboBox.h"

// CvComboBox

IMPLEMENT_DYNAMIC(CvComboBox, CComboBox)

CvComboBox::CvComboBox()
: m_bCmbBtnHover(FALSE)
, m_bLButtonDown(FALSE)
, m_bSelchange(FALSE)
{
	memset(&m_stTrackMouse, 0 ,sizeof(m_stTrackMouse));

	m_crText = RGB(0,0,0);
	m_crBack = RGB(255,255,255);
	m_crSelect = RGB(255,255,0);

	m_x = 0;
	m_y = 0;
	m_cx = 0;
	m_cy = 0;
}

CvComboBox::~CvComboBox()
{
	m_font.DeleteObject();
	if(m_brBrush.m_hObject) m_brBrush.DeleteObject();
}


BEGIN_MESSAGE_MAP(CvComboBox, CComboBox)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONUP()
	ON_CONTROL_REFLECT_EX(CBN_SELCHANGE, &CvComboBox::OnCbnSelchange) // 2010-12-06 hkeins: change reflect to reflect_Ex for parent message process
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

// called only if create directly
BOOL CvComboBox::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_VSCROLL;
	
	return CComboBox::PreCreateWindow(cs);
}

int CvComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	CWnd * pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));

		CFont * pFont = pWnd->GetFont();
		pFont->GetLogFont(&lf);
		m_font.CreateFontIndirect(&lf);
	}
	if(!m_brBrush.m_hObject) m_brBrush.CreateSolidBrush(m_crBack);

	m_stTrackMouse.cbSize       = sizeof(TRACKMOUSEEVENT);   
	m_stTrackMouse.dwFlags      = TME_LEAVE;   
	m_stTrackMouse.dwHoverTime  = HOVER_DEFAULT;   
	m_stTrackMouse.hwndTrack    = GetSafeHwnd();

	SetItemHeight(-1, 15);
	SetItemHeight(0, 15);

	return 0;
}

void CvComboBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.SetBkMode(TRANSPARENT);

	CRect rc;
	CString strMsg;
	BOOL bFocus = (GetFocus() == this);
	COLORREF crTextColor = m_crText;

	int		 i;
	CFont* pOldFont = (CFont*)dc.SelectObject(&m_font);

	GetClientRect(&rc);
	CRect rcText(4, 3, rc.right-20, rc.bottom-3);

	POINT pt = {0, 0};

	if(m_imgContent.m_hImageList)
	{
		// left
		m_imgContent.Draw(&dc, 0, pt, ILD_TRANSPARENT); pt.x+=10;
		// middle
		int nCnt = (rc.right - 20) / 10;
		if ( (rc.right - 20) % 10 ) nCnt++;
		for(i = 0; i < nCnt; i++,pt.x+=10)
		{
			m_imgContent.Draw(&dc, 1, pt, ILD_TRANSPARENT);
		}
		// right
		pt.x = rc.right - 10;
		m_imgContent.Draw(&dc, 2, pt, ILD_TRANSPARENT);
	}

	if(!IsWindowEnabled())
	{
		crTextColor = GetSysColor(COLOR_GRAYTEXT);
		dc.FillSolidRect(&rcText, m_crBack);
	}
	else if(0)//bFocus && m_bSelchange)
	{
		crTextColor = m_crText;
		dc.FillSolidRect(&rcText, m_crSelect);
	}
	rcText.DeflateRect(1,1);

	int nCurSel = GetCurSel();
	if(nCurSel >= 0)
	{
		GetLBText(nCurSel, strMsg);
		dc.SetTextColor(crTextColor);
		dc.DrawText(strMsg, rcText, DT_LEFT|DT_VCENTER);
	}

	if(pOldFont)
		dc.SelectObject(pOldFont);

	pt.x = 0;
	pt.y = 0;
	int nImage = 0;
	IMAGEINFO imgInfo;

	if(!IsWindowEnabled())
		nImage = 3;
	else if(m_bCmbBtnHover) {
		if ( m_bLButtonDown ) nImage = 1;
		else nImage = 2;
	}

	if(m_imgBtnList.m_hImageList)
	{
		m_imgBtnList.GetImageInfo(nImage, &imgInfo);
		int nImgWidth  = imgInfo.rcImage.right - imgInfo.rcImage.left;

		pt.x = rc.Width() - nImgWidth;
		m_imgBtnList.Draw(&dc, nImage, pt, ILD_NORMAL);
	}
}

// CvComboBox 메시지 처리기입니다.
void CvComboBox::DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	if (lpdis->itemID==(UINT)-1) 
	{
		return;
	}

	CRect	rectItem(lpdis->rcItem);
	UINT	nState = lpdis->itemState;
	CBrush	brFrameBrush;

	CDC	dc;
	if (!dc.Attach(lpdis->hDC))			// Attach CDC Object
		return;							// Stop If Attach Failed

	CFont* pOldFont = (CFont*)dc.SelectObject(&m_font);

	rectItem.DeflateRect(2, 1);
	if((nState & ODS_COMBOBOXEDIT) == ODS_COMBOBOXEDIT)
	{

	}
	else
	{
		if(nState & ODS_SELECTED)
		{
			dc.FillSolidRect(&rectItem, m_crSelect);		// draw color block
		}
		else
		{
			dc.FillSolidRect(&rectItem, RGB(255, 255, 255));		// draw color block
		}
	}

	CString strItem;
	GetLBText(lpdis->itemID, strItem);
	int nSavedDC = dc.SaveDC();
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_crText);
	dc.DrawText(strItem, rectItem, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	dc.SelectObject(pOldFont);
	dc.RestoreDC(nSavedDC);
	dc.Detach();
}

void CvComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpmis)
{

}

int CvComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	// TODO:  지정된 항목의 정렬 순서를 확인하는 코드를 추가합니다.
	// -1 반환 = 항목 1이 항목 2 앞에 정렬되는 경우
	// 0 반환 = 항목 1과 항목 2가 같이 정렬되는 경우
	// 1 반환 = 항목 1이 항목 2 뒤에 정렬되는 경우
	if(lpCompareItemStruct->itemData1 > lpCompareItemStruct->itemData2)
		return -1;
	else if(lpCompareItemStruct->itemData1 == lpCompareItemStruct->itemData2)
		return 0;

	return 1;
}

void CvComboBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	CComboBox::DeleteItem(lpDeleteItemStruct);
}

void CvComboBox::PreSubclassWindow()
{
	CComboBox::PreSubclassWindow();

	ModifyStyle(CBS_DROPDOWN | CBS_SIMPLE, CBS_HASSTRINGS | CBS_OWNERDRAWFIXED | CBS_DROPDOWNLIST | WS_CHILD);

	SetItemHeight(-1, 15);
	SetItemHeight(0, 15);

	m_stTrackMouse.cbSize       = sizeof(TRACKMOUSEEVENT);   
	m_stTrackMouse.dwFlags      = TME_LEAVE;   
	m_stTrackMouse.dwHoverTime  = HOVER_DEFAULT;   
	m_stTrackMouse.hwndTrack    = GetSafeHwnd();
}

void CvComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bSelchange = FALSE;
	m_bLButtonDown = TRUE;
	CRect rc; GetClientRect(&rc);
	rc.left = rc.right - 19;
	InvalidateRect(&rc,FALSE);

	CComboBox::OnLButtonDown(nFlags, point);
}

void CvComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = FALSE;
	CRect rc; GetClientRect(&rc);
	rc.left = rc.right - 19;
	InvalidateRect(&rc,FALSE);

	CComboBox::OnLButtonUp(nFlags, point);
}

void CvComboBox::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bCmbBtnHover)
	{
		m_bCmbBtnHover = TRUE;
		_TrackMouseEvent(&m_stTrackMouse);

		CRect rc; GetClientRect(&rc);
		rc.left = rc.right - 19;
		InvalidateRect(&rc,FALSE);
	}

	CComboBox::OnMouseMove(nFlags, point);
}

void CvComboBox::OnMouseLeave()
{
	m_bCmbBtnHover = FALSE;
	CRect rc; GetClientRect(&rc);
	rc.left = rc.right - 19;
	InvalidateRect(&rc,FALSE);
	CComboBox::OnMouseLeave();
}

BOOL CvComboBox::OnCbnSelchange()
{
	m_bSelchange = TRUE;
	Invalidate(FALSE);
	return FALSE; // parent process CBN_SELCHANGE message
}

HBRUSH CvComboBox::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if(nCtlColor == 1)
		return (HBRUSH)m_brBrush.GetSafeHandle();

	return NULL;
}

void CvComboBox::SetSkin(LPWSTR szContent, LPWSTR szBtnList)
{
	CBitmap bmp;
	if(bmp.Attach((HBITMAP) LoadImage(GetModuleHandle(NULL), szContent, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
	{
		m_imgContent.Create(10, 21, ILC_COLOR24, 0, 0);
		m_imgContent.Add(&bmp, RGB(0, 0, 0));
		bmp.DeleteObject();
	}
	if(bmp.Attach((HBITMAP) LoadImage(GetModuleHandle(NULL), szBtnList, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
	{
		m_imgBtnList.Create(18,21, ILC_COLOR24, 0, 0);
		m_imgBtnList.Add(&bmp, RGB(0, 0, 0));
		bmp.DeleteObject();
	}
}
void CvComboBox::SetFont(LOGFONT* pNewStyle)
{
	if (pNewStyle){
		m_font.DeleteObject();
		m_font.CreateFontIndirect(pNewStyle);
	}
}
void CvComboBox::SetTextColor(COLORREF crText)
{
	if(crText != COLORREF(-1))
	{
		m_crText = crText;
	}
}
void CvComboBox::SetSelColor(COLORREF crSelect, COLORREF crBack)
{
	if(crSelect != COLORREF(-1))
	{
		m_crSelect = crSelect;

		if(crBack != COLORREF(-1))
		{
			m_crBack = crBack;
			if(m_brBrush.m_hObject)
				m_brBrush.DeleteObject();
			m_brBrush.CreateSolidBrush(m_crBack);
		}
	}
}

void CvComboBox::MoveWinref(const RECT & reRef, int x, int y, int cx, int cy)
{
	if(cx && cy)
	{
		m_x = x;
		m_y = y;
		m_cx = cx;
		m_cy = cy;
	}
	if(m_cx && m_cy)
	{
		x = reRef.left + m_x;
		if(m_x < 0)
		{
			if(m_cx + m_x <= 0)
				x = reRef.right + m_x;
			else x = reRef.left + (((reRef.right - reRef.left) - m_cx) / 2);
		}

		y = reRef.top + m_y;
		if(m_y < 0)
		{
			if(m_cy + m_y <= 0)
				y = reRef.bottom + m_y;
			else y = reRef.top + (((reRef.bottom - reRef.top) - m_cy) / 2);
		}

		MoveWindow(x, y, m_cx, m_cy);
	}
	else
	{
		MoveWindow(0,0,0,0);
	}
}
CRect CvComboBox::GetWinrefRect(const RECT & reRef)
{
	CRect re(0,0,0,0);
	if(m_cx && m_cy)
	{
		int x = reRef.left + m_x;
		if(m_x < 0)
		{
			if(m_cx + m_x <= 0)
				x = reRef.right + m_x;
			else x = reRef.left + (((reRef.right - reRef.left) - m_cx) / 2);
		}

		int y = reRef.top + m_y;
		if(m_y < 0)
		{
			if(m_cy + m_y <= 0)
				y = reRef.bottom + m_y;
			else y = reRef.top + (((reRef.bottom - reRef.top) - m_cy) / 2);
		}

		re.SetRect(x, y, re.left + m_cx, re.top + m_cy);
	}
	return re;
}

BOOL CvComboBox::ReCreate()
{
	if(GetSafeHwnd() == NULL)
		return FALSE;

	CWnd* pParent = GetParent();
	if (pParent == NULL) return FALSE;

	DWORD dwStyle = GetStyle();

	CRect rc;
	GetDroppedControlRect(&rc);
	pParent->ScreenToClient(&rc);
	
	UINT nID = GetDlgCtrlID();
	CWnd* pWndAfter = GetNextWindow(GW_HWNDPREV);

	CvComboBox comboNew;
	if(!comboNew.Create(dwStyle, rc, pParent, nID))
	  return FALSE;

	DestroyWindow();

	HWND hwnd = comboNew.Detach();
	Attach(hwnd);

	// position correctly in z-order
	SetWindowPos(pWndAfter == NULL ? &CWnd::wndBottom : pWndAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;
}
