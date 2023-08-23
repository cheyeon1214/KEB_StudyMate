#pragma once


// CMemberDlg ��ȭ �����Դϴ�.

#include "xSkinButton.h"

class CMemberDlg : public CDialog
{
	DECLARE_DYNAMIC(CMemberDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	void SetLanguage();

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CEdit m_Name_edt;
	CEdit m_User_edt;
	CEdit m_Pass_edt;

	CxSkinButton m_OK_btn;
	CxSkinButton m_Cancel_btn;

	CBrush m_BgBrush;

public:
	CMemberDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMemberDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MEMBER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
