#pragma once

#include "xSkinButton.h"

// CMessageDlg 대화 상자입니다.

class CSideDlg;
class CMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);
	void SetListBoxText(CListBox& lst, const WCHAR* sz);

	void SetLanguage();

	CRect m_xyDlg;
	CRect m_xyMsg_List;
	CRect m_reMsg_List;

private:
	void GetCtrlRect(int cx, int cy);
	void MoveCtrl(BOOL bCreate = FALSE);

public:
	CSideDlg* m_pDlg;
	BOOL Create(CSideDlg* pParent);

	void RecvMsgTextSrc(WCHAR* szMsg);

private:
	CEdit m_Msg_edt;
	CListBox m_Msg_lst;
	CxSkinButton m_Msg_btn;

	CBrush m_BgBrush;

	WCHAR m_szManager[64];

public:
	CMessageDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMessageDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MESSAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnMessage();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
