//------------------------------------------------------------------------
// Author:  Rolf Kristensen	
// Adapted by Gordon Eldest using codes from "Alan W" (http://www.codeproject.com/KB/wtl/WTL_ListCtrl.aspx))
// Source:  http://www.codeproject.com/KB/list/CGridListCtrlEx.aspx
// License: Free to use for all (New BSD License)
//------------------------------------------------------------------------

#include "pch.h"

#include "CGridColumnTraitProgress.h"
#include "CGridColumnTraitVisitor.h"
#include "CGridListCtrlEx.h"
#if (_WIN32_WINNT >= 0x501)
 #pragma comment(lib, "UxTheme.lib")
#endif

//-------------------------------- Non CTheme ----------------------------------------
static void DrawRoundRect( CDC* pDC,const CRect& rcRect, COLORREF rgbOuter, COLORREF rgbInner )
{
	CBrush bshInterior;
	bshInterior.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	CBrush* pbrshOld = pDC->SelectObject( &bshInterior );
	
	CPen penBorder;
	penBorder.CreatePen( PS_SOLID, 1, rgbOuter );
	CPen* ppenOld = pDC->SelectObject( &penBorder );
	
	pDC->RoundRect( rcRect, CPoint( 5, 5 ) );
	
	CPen penInnerBorder;
	penInnerBorder.CreatePen( PS_SOLID, 1, rgbInner );

	pDC->SelectObject( &penInnerBorder );

	CRect rcRoundRect( rcRect );
	rcRoundRect.DeflateRect( 1, 1 );
	pDC->RoundRect( rcRoundRect, CPoint( 2, 2 ) );
	
	pDC->SelectObject(ppenOld);
	pDC->SelectObject(pbrshOld);
}
	
//-------------------------------- Non CTheme ----------------------------------------
static void DrawGradient( CDC* pDC, CRect& rcRect, COLORREF rgbTop, COLORREF rgbBottom )
{
	GRADIENT_RECT grdRect = { 0, 1 };
	TRIVERTEX triVertext[ 2 ] = {
		rcRect.left,rcRect.top,
		COLOR16(GetRValue( rgbTop ) << 8),
		COLOR16(GetGValue( rgbTop ) << 8),
		COLOR16(GetBValue( rgbTop ) << 8),
		0x0000,
		rcRect.right,rcRect.bottom,
		COLOR16(GetRValue( rgbBottom ) << 8),
		COLOR16(GetGValue( rgbBottom ) << 8),
		COLOR16(GetBValue( rgbBottom ) << 8),
		0x0000
	};
	pDC->GradientFill( triVertext, 2, &grdRect, 1, GRADIENT_FILL_RECT_V );
}
	
//------------------------------------------------------------------------
//! CGridColumnTraitImage - Constructor
//------------------------------------------------------------------------
CGridColumnTraitProgress::CGridColumnTraitProgress()
{
#if (_WIN32_WINNT >= 0x501)
	m_hTheme = NULL;
#endif
	m_rgbProgressTop = RGB( 170, 240, 170 );
	m_rgbProgressBottom = RGB( 45, 210, 50 );
	m_bHooverValue=false;
	UseTheme();			// Init it by default
}
	
//------------------------------------------------------------------------
//! CGridColumnTraitImage - Destructor
//------------------------------------------------------------------------
CGridColumnTraitProgress::~CGridColumnTraitProgress()
{
#if (_WIN32_WINNT >= 0x501)
	if (m_hTheme)
		VERIFY(CloseThemeData(m_hTheme)== S_OK);
	m_hTheme=NULL;
#endif
}
	
//------------------------------------------------------------------------
//! CGridColumnTraitImage - UseTheme
//! Try to load the current theme (Setup only once, no check done of change of theme while draw once)
//!
//! @param b the choice, false=> delete/clear any previous
//------------------------------------------------------------------------
void	CGridColumnTraitProgress::UseTheme(bool b){
	m_bUseTheme=b;
#if (MYWINVER >= 0x501)
	if (m_hTheme)
		VERIFY(CloseThemeData(m_hTheme)== S_OK);
	m_hTheme=NULL;				// all the load work is done on first OnCustomDraw
#endif
}

//------------------------------------------------------------------------
//! Compares two cell values according to specified sort order; 
//!
//! @param pszLeftValue Left cell value
//! @param pszRightValue Right cell value
//! @param bAscending Perform sorting in ascending or descending order
//! @return Is left value less than right value (-1) or equal (0) or larger (1)
//------------------------------------------------------------------------
int CGridColumnTraitProgress::OnSortRows(LPCTSTR pszLeftValue, LPCTSTR pszRightValue, bool bAscending)
{
	LPCTSTR pCommaLeft  = _tcschr(pszLeftValue, ',');
	LPCTSTR pCommaRight = _tcschr(pszRightValue,',');
	double dLeftEnd, dRightEnd;
	
	if (pCommaLeft)
		dLeftEnd = _tstof(pCommaLeft+1); //The % of progress
	else 
		dLeftEnd = _tstof(pszLeftValue); //The % of progress
	if (pCommaRight)
		dRightEnd= _tstof(pCommaRight+1); //The % of progress
	else 
		dRightEnd= _tstof(pszRightValue); //The % of progress
	dLeftEnd  = max(min(dLeftEnd,100.0),0.0) / 100.0;	// Fool proof it
	dRightEnd = max(min(dRightEnd,100.0),0.0) / 100.0;	// Fool proof it

	const double dFirst = bAscending ? dLeftEnd : dRightEnd;
	const double dSecond= bAscending ? dRightEnd: dLeftEnd;
	if (dFirst > dSecond)
		return 1;
	else if (dFirst < dSecond)
		return -1;
	if (!pCommaLeft && !pCommaRight) 
		return 0;		// a very frequent case
		
	// Ok let's sort using start.
	double dLeftSrt=0.0, dRightSrt=0.0;
	if (pCommaLeft){
		dLeftSrt = _tstof(pszLeftValue);	//The start % of progress
		dLeftSrt  = max(min(dLeftSrt,100.0),0.0) / 100.0;	// Fool proof it
	}
	if (pCommaRight){
		dRightSrt= _tstof(pszRightValue);	//The start % of progress
		dRightSrt = max(min(dRightSrt,100.0),0.0) / 100.0; 	// Fool proof it
	}
	
	const double dFirstS = bAscending ? dLeftSrt : dRightSrt;
	const double dSecondS= bAscending ? dRightSrt: dLeftSrt;
	
	if (dFirstS > dSecondS)
		return 1;
	else if (dFirstS < dSecondS)
		return -1;
	return(0);
}

//------------------------------------------------------------------------
//! Performs custom drawing of the CListCtrl using CGridColumnTrait
//!
//! @param owner The list control drawing
//! @param pLVCD Pointer to NMLVCUSTOMDRAW structure
//! @param pResult Modification to the drawing stage (CDRF_NEWFONT, etc.)
//------------------------------------------------------------------------
void CGridColumnTraitProgress::OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult)
{
#if (_WIN32_WINNT >= 0x501)
	if (m_bUseTheme && !m_hTheme) {		// OpenTheme here to get the HWND
		m_hTheme = ::OpenThemeData(owner.m_hWnd,  (CStringW)("PROGRESS"));	ASSERT(m_hTheme);
	}
#endif	
	int nRow = (int)pLVCD->nmcd.dwItemSpec;

	const bool bIsHot = owner.GetHotItem()==nRow;
	
	// Repair the standard drawing
	switch (pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			// Progress is drawn like an image would be, after anything else
			*pResult |= CDRF_NOTIFYPOSTPAINT;
		} break;

		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{
			if (bIsHot && m_bHooverValue)
				break;		//standard drawing will display text
				
			int nCol = pLVCD->iSubItem;
			if (CRect(pLVCD->nmcd.rc)==CRect(0,0,0,0))
				break;

			CString csTxt(owner.GetItemText(nRow,nCol));		// % as double,either one or 2 numbers "end", or "start,end"
			int nPos=0;
			double dValEnd,dValSrt=0.0;
			dValEnd = _tstof(csTxt.Tokenize(_T(","),nPos));			//The % of progress
			const double dVal=_tstof(csTxt.Tokenize(_T(","),nPos));// Next
			if (nPos>=0) {										// Any start value ?
				dValSrt = dValEnd;								// Switch order, less logical but more natural for user
				dValEnd = dVal;									 //The % before start of progress
			}
			dValSrt = max(min(dValSrt,100.0),0.0) / 100.0; //The % of progress
			dValEnd = max(min(dValEnd,100.0),dValSrt) / 100.0; //The % of progress
				
			CRect rcCell;
			VERIFY( owner.GetCellRect(nRow, nCol, LVIR_BOUNDS, rcCell) );
			// When the label column is placed first it has a left-margin 
			if (nCol==0 && nCol==owner.GetFirstVisibleColumn())
			{
				int cxborder = ::GetSystemMetrics(SM_CXBORDER);
				rcCell.left += cxborder*2;
			}

			// Draw progress
			CRect rcProgress,rcIcon;
			rcProgress = rcCell;		
			if (owner.GetCellRect(nRow, nCol, LVIR_ICON, rcIcon))	// Untested, allow icon (needed with progress ?)
			{	
				rcProgress.left += rcIcon.Width();							// Shrink rcCell from rcIcon
				rcProgress.left = min(rcProgress.left,rcProgress.right);	// priority to Icon
			}
			rcProgress.DeflateRect( 1, 1 );

#if (_WIN32_WINNT >= 0x501)
			if (m_hTheme) {		ASSERT(m_bUseTheme);
				// draw progress frame
				rcProgress.DeflateRect( 1, 0, 1, 1 );
				if (::IsThemeBackgroundPartiallyTransparent(m_hTheme, PP_BAR, 0)){
					::DrawThemeParentBackground(owner.m_hWnd, pLVCD->nmcd.hdc, &rcProgress);
				}
				VERIFY(::DrawThemeBackground(m_hTheme, pLVCD->nmcd.hdc, PP_BAR, 0, &rcProgress,NULL) == S_OK);
				
				// draw progress bar															
				rcProgress.DeflateRect( 2, 2, 1, 2);
				const int w = rcProgress.Width();	// below will change this reference length
				rcProgress.right= rcProgress.left + (int)( (double)w * dValEnd );	// do this first
				rcProgress.left = rcProgress.left + (int)( (double)w * dValSrt );	// then change left
				VERIFY(::DrawThemeBackground(m_hTheme, pLVCD->nmcd.hdc, PP_CHUNK, 0, &rcProgress,NULL) == S_OK);
			} 
			else 
#endif
			{
				rcProgress.DeflateRect( 1, 1 );
				// draw progress border
				CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
				DrawRoundRect( pDC, rcProgress, GetSysColor(COLOR_3DSHADOW),GetSysColor(COLOR_BTNFACE) );
				// fill progress bar area
				rcProgress.DeflateRect( 2, 2 );
				const int w = rcProgress.Width();	// below will change this reference length
				rcProgress.right= rcProgress.left + (int)( (double)w * dValEnd );	// do this first
				rcProgress.left = rcProgress.left + (int)( (double)w * dValSrt );	// then change left
				DrawGradient( pDC, rcProgress, m_rgbProgressTop, m_rgbProgressBottom );
			}
		} break;
	}

	// Perform standard drawing
//	if (bCaryDraw) 
		CGridColumnTraitText::OnCustomDraw(owner, pLVCD, pResult);
}
