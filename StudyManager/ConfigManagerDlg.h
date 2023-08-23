#pragma once

// CConfigManagerDlg ��ȭ �����Դϴ�.

#include "xSkinButton.h"

class CConfigManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigManagerDlg)

private:
	ConfigManager m_cfg;

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);
	void SetLanguage();

public:
	void Init(ConfigManager* pCfg)
	{
		m_cfg = *pCfg;
	}
	ConfigManager* GetCfg() { return &m_cfg; }

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CButton m_UseSvr_btn;
	CEdit m_IP_edt;
	CEdit m_Port_edt;

	CxSkinButton m_OK_btn;
	CxSkinButton m_Cancel_btn;

	CBrush m_BgBrush;

public:
	CConfigManagerDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CConfigManagerDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CONFIG_MANAGER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnOK();
	afx_msg void OnBnClickedBtnCancel();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
