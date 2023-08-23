#pragma once

//------------------------------------------------------------------------
// Author:  Rolf Kristensen	
// Source:  http://www.codeproject.com/KB/list/CGridListCtrlEx.aspx
// License: Free to use for all (New BSD License)
//------------------------------------------------------------------------

#include "CGridColumnTraitImage.h"

//------------------------------------------------------------------------
//! CGridColumnTraitTextButton that can launch a link using the web-browser
//------------------------------------------------------------------------
class CGridColumnTraitTextButton : public CGridColumnTraitImage
{
public:
	CGridColumnTraitTextButton();

	void SetLinkColor(COLORREF linkColor);
	COLORREF GetLinkColor() const;

	void SetLinkColorHot(COLORREF linkColor);
	COLORREF GetLinkColorHot() const;

protected:
	virtual bool UpdateTextColor(NMLVCUSTOMDRAW* pLVCD, COLORREF& textColor);
	virtual bool UpdateTextFont(NMLVCUSTOMDRAW* pLVCD, LOGFONT& textFont);
	virtual CWnd* OnEditBegin(CGridListCtrlEx& owner, int nRow, int nCol);
	virtual CWnd* OnEditBegin(CGridListCtrlEx& owner, int nRow, int nCol, CPoint pt) { return CGridColumnTraitImage::OnEditBegin(owner, nRow, nCol, pt); }
	virtual int OnClickEditStart(CGridListCtrlEx& owner, int nRow, int nCol, CPoint pt, bool bDblClick);

	virtual CRect GetTextRect(CGridListCtrlEx& owner, int nRow, int nCol, const CString& cellText);

	COLORREF m_LinkColor;			//!< Standard link Color
	COLORREF m_LinkColorHot;		//!< Hot link color (mouse over)
};