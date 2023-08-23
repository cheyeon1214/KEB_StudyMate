// CxSkinButton.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xSkinButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CxSkinButton
CxSkinButton::CxSkinButton()
{
	m_DrawMode=0;
	m_Border=0;
	m_FocusRectMargin=0;
	hClipRgn=NULL;			// no clipping region
	m_crText = GetSysColor(COLOR_BTNTEXT);
	m_crBack = RGB(255 - GetRValue(m_crText), 255 - GetGValue(m_crText), 255 - GetBValue(m_crText));
	m_crTextD = COLORREF(-1);
	m_crBackD = COLORREF(-1);
	m_button_down = m_tracking = m_Checked = false;

	m_bText = FALSE;
	m_TextAlign = (DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	m_pLF = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CxSkinButton::~CxSkinButton()
{
	if (m_pLF)	free(m_pLF);
	if (m_Font.m_hObject) m_Font.DeleteObject();
	if (hClipRgn) DeleteObject(hClipRgn);	// free clip region

	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CxSkinButton, CButton)
	//{{AFX_MSG_MAP(CxSkinButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	//ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_CXSHADE_RADIO , OnRadioInfo)
	ON_MESSAGE(BM_SETCHECK , OnBMSetCheck)
	ON_MESSAGE(BM_GETCHECK , OnBMGetCheck)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton message handlers
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::PreSubclassWindow() 
{
	m_Style=GetButtonStyle();	///get specific BS_ styles
	if ((m_Style & BS_AUTOCHECKBOX)==BS_AUTOCHECKBOX)
//		||((m_Style & BS_CHECKBOX)==BS_CHECKBOX))
		m_Style=BS_CHECKBOX;
	else if ((m_Style & BS_AUTORADIOBUTTON)==BS_AUTORADIOBUTTON)
//			||((m_Style & BS_RADIOBUTTON)==BS_RADIOBUTTON))
		m_Style=BS_RADIOBUTTON;
	else { m_Style=BS_PUSHBUTTON; }

	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CxSkinButton::OnEraseBkgnd(CDC* pDC) 
{	return 1; }	// doesn't erase the button background
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    ASSERT (lpDrawItemStruct);
    //TRACE("* Captured: %08X\n", ::GetCapture());

    //Check if the button state in not in inconsistent mode...
    POINT mouse_position;
    if ((m_button_down) && (::GetCapture() == m_hWnd) && (::GetCursorPos(&mouse_position))){
		if (::WindowFromPoint(mouse_position) == m_hWnd){
			if ((GetState() & BST_PUSHED) != BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(TRUE);
				return;
			}
		} else {
			if ((GetState() & BST_PUSHED) == BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(FALSE);
				return;
			}
		}
	}
	
    //TRACE("* Drawing: %08x\n", lpDrawItemStruct->itemState);
	CString sCaption;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	// get device context
	RECT r=lpDrawItemStruct->rcItem;					// context rectangle
	int cx = r.right  - r.left ;						// get width
	int cy = r.bottom - r.top  ;						// get height
	// get text box position
	RECT tr={r.left+m_FocusRectMargin+2,r.top+1,r.right-m_FocusRectMargin-2,r.bottom-1};

	GetWindowText(sCaption);							// get button text
	pDC->SetBkMode(TRANSPARENT);

	CFont* pOldFont=NULL;
	if (m_bText){
		if (m_Font.m_hObject) pOldFont = pDC->SelectObject(&m_Font);
		else pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
	}

	// Select the correct skin 
	if (lpDrawItemStruct->itemState & ODS_DISABLED){	// DISABLED BUTTON
		if(m_bDisabled.m_hObject==NULL)
			 // no skin selected for disabled state -> standard button
			pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
		else // paint the skin
			DrawBitmap(pDC,(HBITMAP)m_bDisabled,r,m_DrawMode);
		// if needed, draw the standard 3D rectangular border
		if (m_Border) pDC->DrawEdge(&r,EDGE_RAISED,BF_RECT);
		if (m_bText){
			OffsetRect(&tr,1,1);
			pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
			pDC->DrawText(sCaption,&tr,m_TextAlign);
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			OffsetRect(&tr,-1,-1);
			pDC->DrawText(sCaption,&tr,m_TextAlign);
		}
	} 
	else 
	{										// SELECTED (DOWN) BUTTON
		if ((lpDrawItemStruct->itemState & ODS_SELECTED)||m_Checked || m_button_down)
		{
			if(m_bDown.m_hObject==NULL)
				 // no skin selected for selected state -> standard button
				pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
            else { // paint the skin
				DrawBitmap(pDC,(HBITMAP)m_bDown,r,m_DrawMode);
            }
			// if needed, draw the standard 3D rectangular border
			if (m_Border) pDC->DrawEdge(&r,EDGE_SUNKEN,BF_RECT);
			if (m_bText){
				COLORREF crText = m_crTextD, crBack = m_crBackD;
				if(crText == COLORREF(-1))
				{
					crText = m_crText;
					crBack = m_crBack;
				}
				OffsetRect(&tr,1,1);
				pDC->SetTextColor(crBack);
				pDC->DrawText(sCaption,&tr,m_TextAlign);
				OffsetRect(&tr,-1,-1);
				pDC->SetTextColor(crText);
				pDC->DrawText(sCaption,&tr,m_TextAlign);
			}
		} else {											// DEFAULT BUTTON
			if(m_bNormal.m_hObject==NULL)
				 // no skin selected for normal state -> standard button
				pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
			else // paint the skin
                if ((m_tracking)&&(m_bOver.m_hObject!=NULL)){
					DrawBitmap(pDC,(HBITMAP)m_bOver,r,m_DrawMode);
				} else {
					if ((lpDrawItemStruct->itemState & ODS_FOCUS)&&(m_bFocus.m_hObject!=NULL)){
						DrawBitmap(pDC,(HBITMAP)m_bFocus,r,m_DrawMode);
					} else {
						DrawBitmap(pDC,(HBITMAP)m_bNormal,r,m_DrawMode);
					}
				}
			// if needed, draw the standard 3D rectangular border
			if(m_Border) pDC->DrawEdge(&r,EDGE_RAISED,BF_RECT);
			if (m_bText){
				COLORREF crText = m_crText, crBack = m_crBack;
				OffsetRect(&tr,1,1);
				pDC->SetTextColor(crBack);
				pDC->DrawText(sCaption,&tr,m_TextAlign);
				OffsetRect(&tr,-1,-1);
				pDC->SetTextColor(crText);
				pDC->DrawText(sCaption,&tr,m_TextAlign);
			}
		}
		// paint the focus rect
		if ((lpDrawItemStruct->itemState & ODS_FOCUS)&&(m_FocusRectMargin>0)){
			r.left   += m_FocusRectMargin ;
			r.top    += m_FocusRectMargin ;
			r.right  -= m_FocusRectMargin ;
			r.bottom -= m_FocusRectMargin ;
			DrawFocusRect (lpDrawItemStruct->hDC, &r) ;
		}
		// paint the enabled button text
	}

	if (pOldFont) pDC->SelectObject(pOldFont);	//restore the old font object
}
/////////////////////////////////////////////////////////////////////////////
int CxSkinButton::GetBitmapWidth (HBITMAP hBitmap)
{ BITMAP bm; GetObject(hBitmap,sizeof(BITMAP),(PSTR)&bm); return bm.bmWidth;}
/////////////////////////////////////////////////////////////////////////////
int CxSkinButton::GetBitmapHeight (HBITMAP hBitmap)
{ BITMAP bm; GetObject(hBitmap,sizeof(BITMAP),(PSTR)&bm); return bm.bmHeight;}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode)
{
//	DrawMode: 0=Normal; 1=stretch; 2=tiled fill ; 3=tiled stretch fill
	if(DrawMode==2){
		FillWithBitmap(dc,hbmp,r);
		return;
	}
	else if(DrawMode==3) {
		FillStretchBitmap(dc,hbmp,r);
		return;
	}
	if(!hbmp) return;	//safe check

	int cx=r.right  - r.left;
	int cy=r.bottom - r.top;
	CDC dcBmp,dcMask;
	dcBmp.CreateCompatibleDC(dc);
	dcBmp.SelectObject(hbmp);
	
	if(m_bMask.m_hObject!=NULL)
	{
		dcMask.CreateCompatibleDC(dc);
		dcMask.SelectObject(m_bMask);

		CDC hdcMem;
		hdcMem.CreateCompatibleDC(dc);
	    CBitmap hBitmap;
		hBitmap.CreateCompatibleBitmap(dc,cx,cy);
	    hdcMem.SelectObject(hBitmap);
		
        hdcMem.BitBlt(r.left,r.top,cx,cy,dc,0,0,SRCCOPY);
		if(!DrawMode)
		{
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcMask,0,0,SRCAND);
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
		} 
		else 
		{
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcMask,0,0,bx,by,SRCAND);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
		}
        dc->BitBlt(r.left,r.top,cx,cy,&hdcMem,0,0,SRCCOPY);

		hdcMem.DeleteDC();
		hBitmap.DeleteObject();

		DeleteDC(dcMask);
	} 
	else 
	{
		if(!DrawMode)
		{
			dc->BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCCOPY);
		}
		else 
		{
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			dc->StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCCOPY);
		}
	}
	DeleteDC(dcBmp);
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r)
{
	if(!hbmp) return;
	CDC memdc;
	memdc.CreateCompatibleDC(dc);
	memdc.SelectObject(hbmp);
	int w = r.right - r.left;
	int	h = r.bottom - r.top;
	int x,y,z;
	int	bx=GetBitmapWidth(hbmp);
	int	by=GetBitmapHeight(hbmp);
	for (y = r.top ; y < h ; y += by){
		if ((y+by)>h) by=h-y;
		z=bx;
		for (x = r.left ; x < w ; x += z){
			if ((x+z)>w) z=w-x;
			dc->BitBlt(x, y, z, by, &memdc, 0, 0, SRCCOPY);
		}
	}
	DeleteDC(memdc);
}
void CxSkinButton::FillStretchBitmap(CDC* dc, HBITMAP hbmp, RECT r)
{
	if(!hbmp) return;
	CDC memdc;
	memdc.CreateCompatibleDC(dc);
	memdc.SelectObject(hbmp);
	int w = r.right - r.left;
	int	h = r.bottom - r.top;
	int	bx=GetBitmapWidth(hbmp);
	int	by=GetBitmapHeight(hbmp);

	int bx3 = bx/3;
	int by3 = by/3;

	int x = r.left + bx3;
	int y = r.top + by3;
	int xSrc = bx3;
	int ySrc = by3;
	int cx = w - 2*bx3;
	int cy = h - 2*by3;
	dc->StretchBlt(x, y, cx, cy, &memdc, xSrc, ySrc, bx3, by3, SRCCOPY);

	x = r.left;
	y = r.top;
	xSrc = 0;
	ySrc = 0;
	dc->BitBlt(x, y, bx3, by3, &memdc, xSrc, ySrc, SRCCOPY);

	x = r.left + bx3;
	y = r.top;
	xSrc = bx3;
	ySrc = 0;
	cx = w - 2*bx3;
	cy = by3;
	dc->StretchBlt(x, y, cx, cy, &memdc, xSrc, ySrc, bx3, by3, SRCCOPY);

	x = r.right - bx3;
	y = r.top;
	xSrc = bx - bx3;
	ySrc = 0;
	dc->BitBlt(x, y, bx3, by3, &memdc, xSrc, ySrc, SRCCOPY);

	x = r.right - bx3;
	y = r.top + by3;
	xSrc = bx - bx3;
	ySrc = by3;
	cx = bx3;
	cy = h - 2*by3;
	dc->StretchBlt(x, y, cx, cy, &memdc, xSrc, ySrc, bx3, by3, SRCCOPY);

	x = r.left;
	y = r.bottom - by3;
	xSrc = 0;
	ySrc = by - by3;
	dc->BitBlt(x, y, bx3, by3, &memdc, xSrc, ySrc, SRCCOPY);

	x = r.left + bx3;
	y = r.bottom - by3;
	xSrc = bx3;
	ySrc = bx - by3;
	cx = w - 2*bx3;
	cy = by3;
	dc->StretchBlt(x, y, cx, cy, &memdc, xSrc, ySrc, bx3, by3, SRCCOPY);

	x = r.right - bx3;
	y = r.bottom - by3;
	xSrc = bx - bx3;
	ySrc = by - by3;
	dc->BitBlt(x, y, bx3, by3, &memdc, xSrc, ySrc, SRCCOPY);

	x = r.left;
	y = r.top + by3;
	xSrc = 0;
	ySrc = by3;
	cx = bx3;
	cy = h - 2*by3;
	dc->StretchBlt(x, y, cx, cy, &memdc, xSrc, ySrc, bx3, by3, SRCCOPY);

	DeleteDC(memdc);
}
/////////////////////////////////////////////////////////////////////////////
HRGN CxSkinButton::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	if (!hBmp) return NULL;

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes

	CDC dcBmp;
	CDC * pDC=GetDC();
	dcBmp.CreateCompatibleDC(pDC);	//Creates a memory device context for the bitmap
	dcBmp.SelectObject(hBmp);			//selects the bitmap in the device context

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
	const DWORD MAXBUF = 40;		// size of one block in RECTs
									// (i.e. MAXBUF*sizeof(RECT) in bytes)
	LPRECT	pRects;								
	DWORD	cBlocks = 0;			// number of allocated blocks

	INT		i, j;					// current position in mask image
	INT		first = 0;				// left position of current scan line
									// where mask was found
	bool	wasfirst = false;		// set when if mask was found in current scan line
	bool	ismask;					// set when current color is mask color

	// allocate memory for region data
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
	memset( pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RDHDR;
	pRgnData->iType		= RDH_RECTANGLES;
	pRgnData->nCount	= 0;
//--------------------------------------------------------------------------------------------------------------------
	CDC dcMask;
	if(m_bMask.m_hObject)
	{
		dcMask.CreateCompatibleDC(pDC);
		dcMask.SelectObject(&m_bMask);
		CRect re(0,0,GetBitmapWidth(hBmp),GetBitmapHeight(hBmp));
		CBrush br(RGB(255,255,255));
		dcMask.FillRect(&re,&br);

	}
//--------------------------------------------------------------------------------------------------------------------
	for ( i = 0; i < bm.bmHeight; i++ )
	{
		for ( j = 0; j < bm.bmWidth; j++ )
		{
			// get color
			ismask=(dcBmp.GetPixel(j,bm.bmHeight-i-1)!=color);
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if(dcMask.m_hDC && ismask)
			{
				dcMask.SetPixel(j,bm.bmHeight-i-1,RGB(0,0,0));
			}
			if (wasfirst && ((ismask && (j==(bm.bmWidth-1)))||(ismask ^ (j<bm.bmWidth))))
			{
				// get offset to RECT array if RGNDATA buffer
				pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
				// save current RECT
				pRects[ pRgnData->nCount++ ] = CRect( first, bm.bmHeight - i - 1, j+1-(!ismask), bm.bmHeight - i );
				// if buffer full reallocate it
				if ( pRgnData->nCount >= cBlocks * MAXBUF )
				{
					LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
					memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT) );
					delete pRgnData;
					pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				}
				wasfirst = false;
			} 
			else if ( !wasfirst && ismask )
			{		// set wasfirst when mask is found
				first = j;
				wasfirst = true;
			}
		}
	}

//--------------------------------------------------------------------------------------------------------------------
	if(dcMask.m_hDC)
	{
		dcMask.DeleteDC();
	}
//--------------------------------------------------------------------------------------------------------------------

	dcBmp.DeleteDC();	//release the bitmap
	HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
	ASSERT( hRgn!=NULL );
	pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
	for(i=0;i<(int)pRgnData->nCount;i++)
	{
		HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
		VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
		if (hr) DeleteObject(hr);
	}
	ASSERT( hRgn!=NULL );
	
	ReleaseDC(pDC);
	delete pRgnData;
	return hRgn;
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::SetTextColor(COLORREF text, COLORREF back)
{
	if(text != COLORREF(-1))
	{
		m_crText = text;
		if(back != COLORREF(-1))
			m_crBack = back;
		else m_crBack = RGB(255 - GetRValue(m_crText), 255 - GetGValue(m_crText), 255 - GetBValue(m_crText));
	}
}
void CxSkinButton::SetTextColorD(COLORREF text, COLORREF back)
{
	if(text != COLORREF(-1))
	{
		m_crTextD = text;
		if(back != COLORREF(-1))
			m_crBackD = back;
		else m_crBackD = RGB(255 - GetRValue(m_crTextD), 255 - GetGValue(m_crTextD), 255 - GetBValue(m_crTextD));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::SetToolTipText(CString s)
{
	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this))	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(1);
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
// This function will create a MSG structure, fill it in a pass it to
// the ToolTip control, m_ttip.  Note that we ensure the point is in window
// coordinates (relative to the control's window).
	if(NULL != m_tooltip.m_hWnd){
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.RelayEvent(&msg);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonDblClk
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: Used only to be forwarded as WM_LBUTTONDOWN message parameters
//Exceptions..: NONE
//------------
//Description :
//
//  > We do not care about doublelicks - handle this event
//    like an ordinary left-button-down event
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonDblClk(UINT flags, CPoint point) 
{
    SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(point.x, point.y));
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonDown
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle event when left button is pressed down
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    //TRACE("* %08X: down\n", ::GetTickCount());
	SetCapture();
	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    //If we are tracking this button, cancel it
    if (m_tracking) {
        TRACKMOUSEEVENT t = {
            sizeof(TRACKMOUSEEVENT),
            TME_CANCEL | TME_LEAVE,
            m_hWnd,
            0
        };
        if (::_TrackMouseEvent(&t)) {
            m_tracking = false;
        }
    }

    //Default-process the message
	CButton::OnLButtonDown(nFlags, point);
    m_button_down = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonUp
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle event when left button is released (goes up)
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    //TRACE("* %08X: up\n", ::GetTickCount());

	if (m_Style){ //track mouse for radio & check buttons
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);
		if (mouse_wnd == m_hWnd){ // mouse is in button
			if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
			if (m_Style==BS_RADIOBUTTON) SetCheck(1);
		}
	}
	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    //Default-process the message
    m_button_down = false;
	Invalidate(FALSE);

	CButton::OnLButtonUp(nFlags, point);
	ReleaseCapture();
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnMouseMove
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle change of mouse position: see the comments in the
//    method for further info.
//
//---------------------------------------------------------
void CxSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
    //TRACE("* %08X: Mouse\n", ::GetTickCount());

	//Pass this message to the ToolTip control
	RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    //If we are in capture mode, button has been pressed down
    //recently and not yet released - therefore check is we are
    //actually over the button or somewhere else. If the mouse
    //position changed considerably (e.g. we moved mouse pointer
    //from the button to some other place outside button area)
    //force the control to redraw
    //
    if ((m_button_down) && (::GetCapture() == m_hWnd)) {
	    POINT p2 = point;
        ::ClientToScreen(m_hWnd, &p2);
        HWND mouse_wnd = ::WindowFromPoint(p2);

        bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
        bool need_pressed = (mouse_wnd == m_hWnd);
        if (pressed != need_pressed) {
            //TRACE("* %08X Redraw\n", GetTickCount());
            SetState(need_pressed ? TRUE : FALSE);
            Invalidate();
        }
    } else {

	//Otherwise the button is released. That means we should
    //know when we leave its area - and so if we are not tracking
    //this mouse leave event yet, start now!
    //
        if (!m_tracking) {
            TRACKMOUSEEVENT t = {
                sizeof(TRACKMOUSEEVENT),
                TME_LEAVE,
                m_hWnd,
                0
            };
            if (::_TrackMouseEvent(&t)) {
                //TRACE("* Mouse enter\n");
                m_tracking = true;
                Invalidate();
            }
        }
    }

    //Forward this event to superclass
    CButton::OnMouseMove(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnMouseLeave
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NULL
//Parameters..: NOT USED
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle situation when mouse cursor leaves area of this
//    window (button). This event might be generated ONLY
//    if we explicitely call 'TrackMouseEvent'. This is
//    signalled by setting the m_tracking flag (see the assert
//    precondition) - in 'OnMouseMove' method
//
//  > When a mouse pointer leaves area of this button (i.e.
//    when this method is invoked), presumably the look of
//    the button changes (e.g. when hover/non-hover images are set)
//    and therefore we force the control to redraw.
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnMouseLeave(WPARAM, LPARAM)
{
    ASSERT (m_tracking);
    //TRACE("* Mouse leave\n");
    m_tracking = false;
    Invalidate();

	if(NULL != m_tooltip.m_hWnd){
		CString s;
		m_tooltip.GetText(s, this);
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnKillFocus
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: See superclass documentation
//Exceptions..: NONE
//------------
//Description :
//
//  > If focus is killed during capture, we may no longer
//    have the exclusive access to user input and therefore
//    release it.
//
//  > Such a situation might happens when the user left-clicks
//    this button, keeps the button down and simultaneously
//    presses TAB key.
//
//---------------------------------------------------------
void CxSkinButton::OnKillFocus(CWnd *new_wnd)
{
    if (::GetCapture() == m_hWnd) {
        ::ReleaseCapture();
        ASSERT (!m_tracking);
        m_button_down = false;
    }
    CButton::OnKillFocus(new_wnd);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnClicked
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: FALSE (do not stop in this handler - forward to parent)
//Parameters..: NONE
//Exceptions..: NONE
//------------
//Description :
//
//  > Keep consistency of attributes of this instance before
//    submitting click event to the parent.
//
//  > Currently NOT used. To use, umcomment line
//    "ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)" in message map
//    at the beginning of this file.
//
//---------------------------------------------------------
BOOL CxSkinButton::OnClicked() 
{
    if (::GetCapture() == m_hWnd) {
        ::ReleaseCapture();
        ASSERT (!m_tracking);
    }
    m_button_down = false;
    //Invalidate();
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnRadioInfo
//Class.......: CxSkinButton
//
//Author......: Rainer Mangold
//Created.....: JUN-2001
//
//Return value: NULL
//Parameters..: WPARAM and LPARAM (LPARAM not used)
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle notification, that a Button in the same group was pushed
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnRadioInfo(WPARAM wparam, LPARAM)
{
	if (m_Checked){	//only checked buttons need to be unchecked
		m_Checked = false;
		Invalidate();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((m_Style)&&(nChar==' ')){ //needed stuff for check & radio buttons
		if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
		if (m_Style==BS_RADIOBUTTON) SetCheck(1);
	}
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
//
//Method......: SetCheck
//Class.......: CxSkinButton
//
//Author......: Rainer Mangold
//Created.....: JUN-2001
//
//Return value: NONE
//Parameters..: bool
//Exceptions..: NONE
//------------
//Description :
//
//  > Set the state of this button (pushed or not). 
//    Works for both, Radio and CheckBox - Buttons
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnBMSetCheck(WPARAM wparam, LPARAM)
{
	m_Checked=wparam!=0;
	switch (m_Style)
	{
	case BS_RADIOBUTTON:
		if (m_Checked) { //uncheck the other radio buttons (in the same group)
			HWND hthis,hwnd2,hpwnd;
			hpwnd=GetParent()->GetSafeHwnd();	//get button parent handle
			hwnd2=hthis=GetSafeHwnd();			//get this button handle
			if (hthis && hpwnd){				//consistency check
				for( ; ; ){	//scan the buttons within the group
					hwnd2=::GetNextDlgGroupItem(hpwnd,hwnd2,0);
					//until we reach again this button
					if ((hwnd2==hthis)||(hwnd2==NULL)) break;
					//post the uncheck message
					::PostMessage(hwnd2, WM_CXSHADE_RADIO, 0, 0);
				}
			}
		}
		break;
	case BS_PUSHBUTTON:
		m_Checked=false;
		ASSERT(false); // Must be a Check or Radio button to use this function
	}

	Invalidate();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CxSkinButton::OnBMGetCheck(WPARAM wparam, LPARAM)
{ return m_Checked; }	//returns the state for check & radio buttons
/////////////////////////////////////////////////////////////////////////////
//EOF

/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::SetSkin(UINT nIDBtn)
{
	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();
	if(!nIDBtn) return;

//-------------------------------------------------------------------------------------------------------------------	
	CDC * pDC=GetDC();
	CBitmap bmBtn;
	CDC		SrcDC;
	SrcDC.CreateCompatibleDC(pDC);
	bmBtn.LoadBitmap(nIDBtn);
	if(!bmBtn.m_hObject)
	{
		TRACE("Error loading bitmap");
		ReleaseDC(pDC);
		return;
	}
	SrcDC.SelectObject(&bmBtn);
	int nWidth,nHeight;
	
	nWidth=GetBitmapWidth((HBITMAP)bmBtn)/4;
	nHeight=GetBitmapHeight((HBITMAP)bmBtn);
	CDC MemDC;
	CBitmap * pOldBm=NULL;
	MemDC.CreateCompatibleDC(pDC);

	m_bNormal.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bNormal);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,0*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDown.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bDown);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,1*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bOver.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bOver);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,2*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDisabled.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bDisabled);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,3*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	MemDC.DeleteDC();
	SrcDC.DeleteDC();
	bmBtn.DeleteObject();
//-------------------------------------------------------------------------------------------------------------------	

	m_bMask.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	if(hClipRgn) 
		DeleteObject(hClipRgn);
	hClipRgn = CreateRgnFromBitmap(m_bNormal,RGB(255,0,255));
	if(hClipRgn)
	{
		SetWindowRgn(hClipRgn, TRUE);
		SelectClipRgn((HDC)pDC,hClipRgn);
	}
	if(m_DrawMode==0)
	{
		SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
	}
	ReleaseDC(pDC);
}
void CxSkinButton::SetSkin(LPWSTR strBtn)
{
	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();

//-------------------------------------------------------------------------------------------------------------------	
	CDC * pDC=GetDC();
	CBitmap bmBtn;
	CDC		SrcDC;
	SrcDC.CreateCompatibleDC(pDC);
	bmBtn.Attach((HBITMAP) LoadImage(GetModuleHandle(NULL), strBtn, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE) );
	if(!bmBtn.m_hObject)
	{
		TRACE("Error loading bitmap");
		ReleaseDC(pDC);
		return;
	}
	SrcDC.SelectObject(&bmBtn);
	int nWidth,nHeight;
	
	nWidth=GetBitmapWidth((HBITMAP)bmBtn)/4;
	nHeight=GetBitmapHeight((HBITMAP)bmBtn);
	CDC MemDC;
	CBitmap * pOldBm=NULL;
	MemDC.CreateCompatibleDC(pDC);

	m_bNormal.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bNormal);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,0*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDown.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bDown);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,1*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bOver.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bOver);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,2*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDisabled.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bDisabled);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,3*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	MemDC.DeleteDC();
	SrcDC.DeleteDC();
	bmBtn.DeleteObject();
//-------------------------------------------------------------------------------------------------------------------	

	m_bMask.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	if(hClipRgn) 
		DeleteObject(hClipRgn);
	hClipRgn = CreateRgnFromBitmap(m_bNormal,RGB(255,0,255));
	if(hClipRgn)
	{
		SetWindowRgn(hClipRgn, TRUE);
		SelectClipRgn((HDC)pDC,hClipRgn);
	}
	if(m_DrawMode==0)
	{
		SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
	}
	ReleaseDC(pDC);
}
void CxSkinButton::SetSkin(UINT nIDBtn, CRect re)
{
	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();

//-------------------------------------------------------------------------------------------------------------------	
	CDC * pDC=GetDC();
	CBitmap bmBtn;
	CDC		SrcDC;
	SrcDC.CreateCompatibleDC(pDC);
	bmBtn.LoadBitmap(nIDBtn);
	if(!bmBtn.m_hObject)
	{
		TRACE("Error loading bitmap");
		ReleaseDC(pDC);
		return;
	}
	SrcDC.SelectObject(&bmBtn);
	int nWidth,nHeight;
	
	nWidth=GetBitmapWidth((HBITMAP)bmBtn)/4;
	nHeight=GetBitmapHeight((HBITMAP)bmBtn);
	CDC MemDC;
	CBitmap * pOldBm=NULL;
	MemDC.CreateCompatibleDC(pDC);

	m_bNormal.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bNormal);
	MemDC.BitBlt(0,0,re.Width(),re.Height(),&SrcDC,0*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDown.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bDown);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,1*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bOver.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bOver);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,2*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDisabled.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bDisabled);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,3*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	MemDC.DeleteDC();
	SrcDC.DeleteDC();
	bmBtn.DeleteObject();
//-------------------------------------------------------------------------------------------------------------------	

	m_bMask.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	if(hClipRgn) 
		DeleteObject(hClipRgn);
	hClipRgn = CreateRgnFromBitmap(m_bNormal,RGB(255,0,255));
	if(hClipRgn)
	{
		SetWindowRgn(hClipRgn, TRUE);
		SelectClipRgn((HDC)pDC,hClipRgn);
	}
	if(m_DrawMode==0)
	{
		SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
	}
	ReleaseDC(pDC);
}
void CxSkinButton::SetSkin(LPWSTR strBtn, CRect re)
{
	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();

//-------------------------------------------------------------------------------------------------------------------	
	CDC * pDC=GetDC();
	CBitmap bmBtn;
	CDC		SrcDC;
	SrcDC.CreateCompatibleDC(pDC);
	bmBtn.Attach((HBITMAP) LoadImage(GetModuleHandle(NULL), strBtn, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE) );
	if(!bmBtn.m_hObject)
	{
		TRACE("Error loading bitmap");
		ReleaseDC(pDC);
		return;
	}
	SrcDC.SelectObject(&bmBtn);
	int nWidth,nHeight;
	
	nWidth=GetBitmapWidth((HBITMAP)bmBtn)/4;
	nHeight=GetBitmapHeight((HBITMAP)bmBtn);
	CDC MemDC;
	CBitmap * pOldBm=NULL;
	MemDC.CreateCompatibleDC(pDC);

	m_bNormal.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bNormal);
	MemDC.BitBlt(0,0,re.Width(),re.Height(),&SrcDC,0*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDown.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bDown);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,1*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bOver.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bOver);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,2*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDisabled.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bDisabled);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,3*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	MemDC.DeleteDC();
	SrcDC.DeleteDC();
	bmBtn.DeleteObject();
//-------------------------------------------------------------------------------------------------------------------	

	m_bMask.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	if(hClipRgn) 
		DeleteObject(hClipRgn);
	hClipRgn = CreateRgnFromBitmap(m_bNormal,RGB(255,0,255));
	if(hClipRgn)
	{
		SetWindowRgn(hClipRgn, TRUE);
		SelectClipRgn((HDC)pDC,hClipRgn);
	}
	if(m_DrawMode==0)
	{
		SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
	}
	ReleaseDC(pDC);
}
void CxSkinButton::SetSkin(CBitmap* pBMBtn, CRect re)
{
	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();

//-------------------------------------------------------------------------------------------------------------------	
	CDC * pDC=GetDC();
	CDC		SrcDC;
	SrcDC.CreateCompatibleDC(pDC);
	SrcDC.SelectObject(pBMBtn);

	BITMAP bitmap;
	pBMBtn->GetBitmap(&bitmap);

	int nWidth,nHeight;
	
	nWidth=bitmap.bmWidth/4;
	nHeight=bitmap.bmHeight;
	CDC MemDC;
	CBitmap * pOldBm=NULL;
	MemDC.CreateCompatibleDC(pDC);

	m_bNormal.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bNormal);
	MemDC.BitBlt(0,0,re.Width(),re.Height(),&SrcDC,0*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDown.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bDown);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,1*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bOver.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bOver);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,2*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDisabled.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	pOldBm=MemDC.SelectObject(&m_bDisabled);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,3*nWidth+re.left,re.top,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	MemDC.DeleteDC();
	SrcDC.DeleteDC();
//-------------------------------------------------------------------------------------------------------------------	

	m_bMask.CreateCompatibleBitmap(pDC,re.Width(),re.Height());
	if(hClipRgn) 
		DeleteObject(hClipRgn);
	hClipRgn = CreateRgnFromBitmap(m_bNormal,RGB(255,0,255));
	if(hClipRgn)
	{
		SetWindowRgn(hClipRgn, TRUE);
		SelectClipRgn((HDC)pDC,hClipRgn);
	}
	if(m_DrawMode==0)
	{
		SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
	}
	ReleaseDC(pDC);
}
void CxSkinButton::SetSkinStretch(LPWSTR strBtn)
{
	if(m_bNormal.m_hObject!=NULL) m_bNormal.DeleteObject();
	if(m_bDown.m_hObject!=NULL) m_bDown.DeleteObject();
	if(m_bOver.m_hObject!=NULL) m_bOver.DeleteObject();
	if(m_bDisabled.m_hObject!=NULL) m_bDisabled.DeleteObject();
	if(m_bMask.m_hObject!=NULL) m_bMask.DeleteObject();
	if(m_bFocus.m_hObject!=NULL) m_bFocus.DeleteObject();

//-------------------------------------------------------------------------------------------------------------------	
	CDC * pDC=GetDC();
	CBitmap bmBtn;
	CDC		SrcDC;
	SrcDC.CreateCompatibleDC(pDC);
	bmBtn.Attach((HBITMAP) LoadImage(GetModuleHandle(NULL), strBtn, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE) );
	if(!bmBtn.m_hObject)
	{
		TRACE("Error loading bitmap");
		ReleaseDC(pDC);
		return;
	}
	SrcDC.SelectObject(&bmBtn);
	int nWidth,nHeight;
	
	nWidth=GetBitmapWidth((HBITMAP)bmBtn)/4;
	nHeight=GetBitmapHeight((HBITMAP)bmBtn);
	CDC MemDC;
	CBitmap * pOldBm=NULL;
	MemDC.CreateCompatibleDC(pDC);

	m_bNormal.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bNormal);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,0*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDown.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bDown);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,1*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bOver.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bOver);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,2*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	m_bDisabled.CreateCompatibleBitmap(pDC,nWidth,nHeight);
	pOldBm=MemDC.SelectObject(&m_bDisabled);
	MemDC.BitBlt(0,0,nWidth,nHeight,&SrcDC,3*nWidth,0,SRCCOPY);
	MemDC.SelectObject(pOldBm);

	MemDC.DeleteDC();
	SrcDC.DeleteDC();
	bmBtn.DeleteObject();
//-------------------------------------------------------------------------------------------------------------------	

	m_DrawMode = 3;

	ReleaseDC(pDC);
}

bool CxSkinButton::SetTextDraw(BOOL bText, LOGFONT* pNewStyle)
{
	m_bText = bText;
	if(!m_bText) return true;

	if (pNewStyle){
		if (m_pLF==NULL) m_pLF=(LOGFONT*)calloc(1,sizeof(LOGFONT));
		if (m_pLF){
			memcpy(m_pLF,pNewStyle,sizeof(LOGFONT));
			m_Font.DeleteObject();
			if (m_Font.CreateFontIndirect(m_pLF)) return true;
		}
	}
	return false;
}
bool CxSkinButton::SetTextDraw(BOOL bText, CString sFontName, long lSize, long lWeight, BYTE bItalic, BYTE bUnderline)
{
	m_bText = bText;
	if(!m_bText) return true;

	if (m_pLF==NULL) m_pLF=(LOGFONT*)calloc(1,sizeof(LOGFONT));
	if (m_pLF){
		wcsncpy_s(m_pLF->lfFaceName,sFontName,31);
		m_pLF->lfHeight=lSize;
		m_pLF->lfWeight=lWeight;
		m_pLF->lfItalic=bItalic;
		m_pLF->lfUnderline=bUnderline;
		if (m_Font.m_hObject) m_Font.DeleteObject();
		if (m_Font.CreateFontIndirect(m_pLF)) return true;
	}
	return false;
}
void CxSkinButton::SetTextAlign(UINT nTextAlign)
{
//	see DrawText() styles...
	switch (nTextAlign){
	case BS_RIGHT:
		m_TextAlign=DT_RIGHT;
		break;
	case BS_LEFT:
		m_TextAlign=DT_LEFT;
		break;
	default:
		m_TextAlign=DT_CENTER;
	}
	m_TextAlign|=(DT_SINGLELINE|DT_VCENTER);
}
