#pragma once


// CLoginDlg 대화 상자입니다.

#include "xSkinButton.h"

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	void SetLanguage();

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CEdit m_User_edt;
	CEdit m_Pass_edt;

	CxSkinButton m_OK_btn;
	CxSkinButton m_Cancel_btn;

	CBrush m_BgBrush;

public:
	CLoginDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoginDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnC();
	afx_msg void OnBnClickedBtnO();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
