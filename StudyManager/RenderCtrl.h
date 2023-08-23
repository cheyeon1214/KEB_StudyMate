#pragma once

enum DxCursor
{
	DxCur_No = 0,
	DxCur_Wait,
	DxCur_Hand,
	DxCur_Arrow,
	DxCur_Cross,

	DxCur_HandUp,
	DxCur_HandDn,
};

class CRenderCtrl : public CWnd
{
	DECLARE_DYNAMIC(CRenderCtrl)

private:
	BOOL m_bInitial;
	bool m_tracking;

	Cdx_control* m_pdx;

private:
	int m_SelCh;
	int m_SelDiv;
	int m_SelRot;

public:
	BOOL Create(DWORD dwStyle, CWnd* pParent);

	BOOL DxCheck();
	Cdx_control* GetPdx();
	Cdx_control* ReCreateDx();

	bool SetCur(UINT uiCur);
	HCURSOR GetCur(UINT uiCur);

private:
	UINT m_fLButtonDn;

	HCURSOR	m_hHandUp;
	HCURSOR	m_hHandDn;

public:
	CRenderCtrl();
	virtual ~CRenderCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};
