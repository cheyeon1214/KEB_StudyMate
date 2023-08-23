#pragma once

// CConfigMateDlg 대화 상자입니다.

#include "xSkinButton.h"

class CConfigMateDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigMateDlg)

private:
	ConfigMate m_cfg;

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);
	void SetLanguage();

public:
	void Init(ConfigMate* pCfg)
	{
		m_cfg = *pCfg;
	}
	ConfigMate* GetCfg() { return &m_cfg; }

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CButton m_UseCam_btn;
	CEdit m_Index_edt;
	CEdit m_Width_edt;
	CEdit m_Height_edt;

	CxSkinButton m_OK_btn;
	CxSkinButton m_Cancel_btn;

	CBrush m_BgBrush;

public:
	CConfigMateDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CConfigMateDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONFIG_MATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
