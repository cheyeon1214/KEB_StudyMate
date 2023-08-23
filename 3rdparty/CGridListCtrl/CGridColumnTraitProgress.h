#pragma once
//------------------------------------------------------------------------
// Author:  Rolf Kristensen	for framework and idea
// Adapted by Gordon Eldest using codes from "Alan W" (http://www.codeproject.com/KB/wtl/WTL_ListCtrl.aspx))
// Source:  http://www.codeproject.com/KB/list/CGridListCtrlEx.aspx
// License: Free to use for all (New BSD License) (see Alan W Licence)
//	1.0 - Initial Release (2011-11-12)
//------------------------------------------------------------------------
#include "CGridColumnTraitText.h"


//------------------------------------------------------------------------
//! CGridColumnTraitProgress implements progress bar
//!
//! Usage:
//! Progression is read from the text value. as %, Can be int or double. 
//! Can set a 'start' of bar if under the form "start,end" like in "40.3,60" 
//! (Start is first value).
//!
//! Sorting:
//! When 2 End value is == , then use Start high as highest (smallest bar is 'bigger')
//!
//!	The progress bar can either start from 0 (default) or we can define a starting %
//! Simple progress set text "ee.dd" as % or progress where 0<= ee <=100 dd being decimal part if any
//! Start/end progress set text "nn.dd,ss.dd" where <= ee,ss <=100 dd being decimal part if any
//!
//! Theme: Default is to try to load a theme. 
//! if "solid" aspect is choosen (DrawSolid(true)) then Theme is discarded
//! Note: Theme usage require lib UxTheme.lib from MS
//! TODO:
//! Allow change of color: 
//! - Use Text color for bar, easy in case 'DrawSolid', trickier in UseTheme case (HSV conv suggested) 
//! Allow multiple section:
//! - A CSV description as text (sky is the limit!)
//------------------------------------------------------------------------

class CGridColumnTraitProgress : public CGridColumnTraitText
{
public:
			CGridColumnTraitProgress();
	virtual ~CGridColumnTraitProgress();
	void	HooverShowValue(bool b=true){m_bHooverValue=b;};	///> Display cell content (value) when line hoover 
	void	UseTheme(bool b=true);								///> Try to load the current theme. Clean existing if set to false
//	void	DrawSolid(bool b=true){UseTheme(false);};			///> Gradient drawing, replace theme

	// colors needed by Solid drawing
	COLORREF m_rgbProgressTop;									///> Top gradient of solid Bar, Ctor prefill with light Green
	COLORREF m_rgbProgressBottom;								///> Bottom gradient of solid Bar, Ctor prefill with dark Green

protected:
	virtual void OnCustomDraw(CGridListCtrlEx& owner, NMLVCUSTOMDRAW* pLVCD, LRESULT* pResult);	
	virtual int OnSortRows(LPCTSTR pszLeftValue, LPCTSTR pszRightValue, bool bAscending);	// Unfortunately stringwise "100" is < "99", so we need our own sort
	virtual int OnSortRows(const LVITEM& leftItem, const LVITEM& rightItem, bool bAscending){
		 return(OnSortRows(leftItem.pszText, rightItem.pszText, bAscending));
	}
	
#if (_WIN32_WINNT >= 0x501)
  HTHEME	m_hTheme;
#endif
	bool	m_bUseTheme;
	bool	m_bHooverValue;
};