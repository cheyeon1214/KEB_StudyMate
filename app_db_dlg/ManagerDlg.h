#pragma once


// CManagerDlg 대화 상자입니다.

#include "vComboBox.h"
#include "xSkinButton.h"
#include "xShadeButton.h"

#include "CGridListCtrlGroups.h"

class CManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CManagerDlg)

private:
	USER_Table m_usTbl;

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize = true);

	void Init_ListCtrl();
	void Init_ComboBox();

	CRect m_xyDlg;
	CRect m_xyList;
	void MoveCtrl(BOOL bCreate = FALSE);

	void SetLanguage();

	void SetMemberList();

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CGridListCtrlGroups m_Member_ctl;
	int m_iScale;

	CxSkinButton m_DBSearch_btn;

	CDateTimeCtrl m_Date_edt;
	CvComboBox	m_User_cmb;
	CEdit		m_Note_edt;

	CBrush m_BgBrush;

public:
	CManagerDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CManagerDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MANAGER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnDbSearch();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
