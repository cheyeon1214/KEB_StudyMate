#pragma once

#include "xSkinButton.h"

// CToolDlg 대화 상자입니다.

class CStudyManagerDlg;
class CToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	CRect m_xyDlg;

private:
	CRect m_reSettingBtn;
	void GetCtrlRect(int cx, int cy);
	void MoveCtrl(BOOL bCreate = FALSE);

public:
	CStudyManagerDlg* m_pDlg;
	BOOL Create(CStudyManagerDlg* pParent);

private:
	CBitmap	m_bmpMain;
	CxSkinButton m_div_btn[MAX_SCR];

	CxSkinButton m_setting_btn;

public:
	CToolDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CToolDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHILD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
