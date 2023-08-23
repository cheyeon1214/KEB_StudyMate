#if !defined(_MY_XSKIN_BUTTON_)
#define _MY_XSKIN_BUTTON_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_CXSHADE_RADIO	WM_USER+0x100

/////////////////////////////////////////////////////////////////////////////
// CxSkinButton window
class CxSkinButton : public CButton
{
// Construction
public:
	CxSkinButton();

// Attributes
private:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
// Implementation
public:
	void SetSkin(UINT nIDBtn);
	void SetSkin(LPWSTR strBtn);
	void SetSkin(UINT nIDBtn, CRect re);
	void SetSkin(LPWSTR strBtn, CRect re);
	void SetSkin(CBitmap* pBMBtn, CRect re);
	void SetSkinStretch(LPWSTR strBtn);

	void SetToolTipText(CString s);
	void SetTextColor(COLORREF text, COLORREF back = COLORREF(-1));
	void SetTextColorD(COLORREF text, COLORREF back = COLORREF(-1));
	bool SetTextDraw(BOOL bText, LOGFONT* pNewStyle = NULL);
	bool SetTextDraw(BOOL bText, CString sFontName=_T("Tahoma Bold"),long lSize=12, long lWeight=400,BYTE bItalic=0, BYTE bUnderline=0);
	void SetTextAlign(UINT nTextAlign);
	virtual ~CxSkinButton();
	// Generated message map functions

protected:
	BOOL	m_bText;
	CFont	m_Font;					//font object
	LOGFONT* m_pLF;					//font structure
	UINT	m_TextAlign;
	bool	m_Checked;				//radio & check buttons
	DWORD	m_Style;				//radio & check buttons
    bool m_tracking;
    bool m_button_down;
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	CToolTipCtrl m_tooltip;
	CBitmap m_bNormal,m_bDown,m_bDisabled,m_bMask,m_bOver,m_bFocus; //skin bitmaps
	short	m_FocusRectMargin;		//dotted margin offset
	COLORREF m_crText, m_crBack, m_crTextD, m_crBackD;
	HRGN	hClipRgn;				//clipping region
	BOOL	m_Border;				//0=flat; 1=3D;
	short	m_DrawMode;				//0=normal; 1=stretch; 2=tiled;
	HRGN	CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);
	void	FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r);
	void	FillStretchBitmap(CDC* dc, HBITMAP hbmp, RECT r);
	void	DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode);
	int		GetBitmapWidth (HBITMAP hBitmap);
	int		GetBitmapHeight (HBITMAP hBitmap);
	//{{AFX_MSG(CxSkinButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnClicked();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(_MY_XSKIN_BUTTON_)
