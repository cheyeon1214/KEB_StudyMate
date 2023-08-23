
// StudyManagerDlg.h: 헤더 파일
//

#pragma once

#include "xSkinButton.h"

#include "ToolDlg.h"
#include "SideDlg.h"
#include "RenderCtrl.h"

#include "ServerCtrl.h"
#include "ChildCtrl.h"

#define MAX_CLD 16

// CStudyManagerDlg 대화 상자
class CStudyManagerDlg : public CDialog
{
private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);

	void SetLanguage();

	CSize m_xyMAX;
	CRect m_xyDlg;
	CRect m_xyTool;
	CRect m_xySide;
	CRect m_xyScrn;
	CRect m_reTool;
	CRect m_reSide;
	CRect m_reScrn;
	CRect m_reRender;

private:
	void GetCtrlRect(int cx, int cy);
	void MoveCtrl(BOOL bCreate = FALSE);

public:
	BOOL ACCEPT(SOCKET sdAccept);
	void SendMsgTextSrc(WCHAR* szMsg);
	void RecvMsgTextSrc(WCHAR* szMsg);
	void SetDivMode(int nDiv);
	void ManagerSetting();

private:
	Cdx_control* m_pdx;

	CBitmap	m_bmpMain;
	CBitmap	m_bmpScrn;

	CToolDlg m_dlgTool;
	CSideDlg m_dlgSide;
	CRenderCtrl m_ctlRender;

	CServerCtrl m_ctlSvr;
	CChildCtrl m_ctlCld[MAX_CLD];

// 생성입니다.
public:
	CStudyManagerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUDYMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	HICON m_hIcon2;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
