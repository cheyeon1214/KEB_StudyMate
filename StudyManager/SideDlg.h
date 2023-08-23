#pragma once

#include "MessageDlg.h"

// CSideDlg 대화 상자입니다.

class CStudyManagerDlg;
class CSideDlg : public CDialog
{
	DECLARE_DYNAMIC(CSideDlg)

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	CRect m_xyDlg;
	//CRect m_xyCtl_Side;
	CRect m_reCtl_Side;
	CRect m_reMemberBtn;
	CRect m_reManagerBtn;

private:
	void GetCtrlRect(int cx, int cy);
	void MoveCtrl(BOOL bCreate = FALSE);

public:
	CStudyManagerDlg* m_pDlg;
	BOOL Create(CStudyManagerDlg* pParent);

	void SendMsgTextSrc(WCHAR* szMsg);
	void RecvMsgTextSrc(WCHAR* szMsg);

private:
	CBitmap	m_bmpMain;
	CMessageDlg m_dlgMsg;

	CxSkinButton m_member_btn;
	CxSkinButton m_manager_btn;

public:
	CSideDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSideDlg();

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
