#pragma once


// CMembershipDlg 대화 상자입니다.

#include <atlimage.h>

#include "vComboBox.h"
#include "xSkinButton.h"
#include "xShadeButton.h"

#include "CGridListCtrlGroups.h"

class CMembershipDlg : public CDialog
{
	DECLARE_DYNAMIC(CMembershipDlg)

private:
	USER_Table m_usTbl;
	USER_Table m_oldTbl;

	CImage m_imgUser;

private:
	BOOL m_bInitial;
	BOOL Initialize(BOOL fInitialize = true);

	void Init_ListCtrl();
	int GetUserList(WCHAR* szUser, WCHAR* szPass = NULL);

	CRect m_xyDlg;
	CRect m_xyList;
	CRect m_xyApply;
	CRect m_xyPicture;
	void MoveCtrl(BOOL bCreate = FALSE);

	void SetLanguage();

	int SelItem;
	void SetUserList(int nItem = -1);
	void SetUserItem(int nItem = -1);
	BOOL GetUserItem(USER_Item* pItem);

private:
	HICON m_hIcon;
	HICON m_hIcon2;

	CGridListCtrlGroups m_User_lst;
	int m_iScale;

	CEdit m_Name_edt;
	CEdit m_User_edt;
	CEdit m_Pass_edt;
	CEdit m_Note_edt;

	CxSkinButton m_Add_btn;
	CxSkinButton m_Edt_btn;
	CxSkinButton m_Del_btn;

	CxSkinButton m_Apply_btn;
	CxSkinButton m_OpenFile_btn;

	CBrush m_BgBrush;

public:
	CMembershipDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMembershipDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MEMBERSHIP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMClickListUser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnEdt();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnOpenFile();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
