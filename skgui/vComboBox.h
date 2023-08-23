#pragma once

// CvComboBox

class CvComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CvComboBox)

private:
	CImageList m_imgContent;
	CImageList m_imgBtnList;

	TRACKMOUSEEVENT m_stTrackMouse;
	BOOL	m_bCmbBtnHover;
	BOOL	m_bLButtonDown;
	BOOL	m_bSelchange;
	
	CFont	m_font;
	CBrush  m_brBrush;

	COLORREF m_crText;
	COLORREF m_crBack;
	COLORREF m_crSelect;

public:
	CvComboBox();
	virtual ~CvComboBox();

	void SetSkin(LPWSTR szContent, LPWSTR szBtnList);
	void SetFont(LOGFONT* pNewStyle);
	void SetTextColor(COLORREF crText);
	void SetSelColor(COLORREF crSelect, COLORREF crBack = RGB(255,255,255));

	BOOL ReCreate();

public:
	void MoveWinref(const RECT & reRef, int x=0, int y=0, int cx=0, int cy=0);
	CRect GetWinrefRect(const RECT & reRef);

	int m_x, m_y, m_cx, m_cy;

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/);
	virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnCbnSelchange();// 2010-12-06 hkeins: change reflect to reflect_Ex for parent message process
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};
