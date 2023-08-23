
// StudyMateDlg.h: 헤더 파일
//

#pragma once

#include "xSkinButton.h"
#include "xShadeButton.h"

#include "RenderSCtrl.h"

#include "ClientCtrl.h"

// CStudyMateDlg 대화 상자
class CStudyMateDlg : public CDialog
{
private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize);
	void SetLanguage();

	CRect GetScreen(CStatic& stc, int dft = 5);
	void SetListBoxText(CListBox& lst, const WCHAR* sz);

private:
	CRect m_xyDlg;
	void MoveCtrl(BOOL bCreate = FALSE);

public:
	void NetCon(BOOL bNetCon);
	void RecvMsgTextSrc(WCHAR* szMsg);
	void SetDuration(DTime dtDuration);

private:
	Cdx_control* m_pdx;
	CRenderSCtrl m_ctlRender;

	CClientCtrl m_ctlCnt;

private:
	CStatic m_Frame_stc;

	CEdit m_Msg_edt;
	CListBox m_Msg_lst;
	CxShadeButton m_Msg_btn;

	CStatic m_Login_stc;
	CxShadeButton m_Login_btn;

	CxSkinButton m_Setting_btn;
	CxSkinButton m_Member_btn;
	CxSkinButton m_Manager_btn;
	CxSkinButton m_Connect_btn;

	CStatic m_Duration_stc;
	WCHAR m_szDuration[64];

	CFont m_font;
	CBrush m_BgBrush;

// 생성입니다.
public:
	CStudyMateDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUDYMATE_DIALOG };
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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnMessage();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
