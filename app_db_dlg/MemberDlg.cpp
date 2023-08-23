// MemberDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "MemberDlg.h"

// CMemberDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMemberDlg, CDialog)
CMemberDlg::CMemberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMemberDlg::IDD, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;

	m_bInitial = FALSE;
}

CMemberDlg::~CMemberDlg()
{
}

void CMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_NAME, m_Name_edt);
	DDX_Control(pDX, IDC_EDIT_USER, m_User_edt);
	DDX_Control(pDX, IDC_EDIT_PASS, m_Pass_edt);
	DDX_Control(pDX, IDC_BTN_O, m_OK_btn);
	DDX_Control(pDX, IDC_BTN_C, m_Cancel_btn);
}


BEGIN_MESSAGE_MAP(CMemberDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_C, OnBnClickedBtnC)
	ON_BN_CLICKED(IDC_BTN_O, OnBnClickedBtnO)
END_MESSAGE_MAP()


// CMemberDlg 메시지 처리기입니다.

BOOL CMemberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMemberDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

HBRUSH CMemberDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if(m_bInitial)
	{
		switch(nCtlColor)
		{
		case CTLCOLOR_DLG:
			if(bg_crback != COLORREF(-1))
			{
				return (HBRUSH)m_BgBrush;
			}
			break;
		case CTLCOLOR_STATIC:
			if(bg_crback != COLORREF(-1))
			{
				pDC->SetBkColor(bg_crback);
				if(bg_crtext != COLORREF(-1))
				{
					pDC->SetTextColor(bg_crtext);
				}
				return (HBRUSH)m_BgBrush;
			}
			break;
		}
	}
	return hbr;
}

BOOL CMemberDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_ESCAPE)
		{
			OnBnClickedBtnC();
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN)
		{
			OnBnClickedBtnO();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CMemberDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	WORD wNotifyCode = HIWORD(wParam);
	if(wNotifyCode == BN_CLICKED)
	{
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CMemberDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\Dlg_Member.ico", _cmn_IcoPath());
		m_hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		m_hIcon2 = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon2, FALSE);

		if(bg_crback != COLORREF(-1))
		{
			m_BgBrush.CreateSolidBrush(bg_crback);
			SetWindowTheme(GetDlgItem(IDC_EDIT_NAME)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_USER)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_PASS)->m_hWnd, L"", L"");
		}
		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());
		m_OK_btn.SetSkinStretch(szPath);
		m_OK_btn.SetTextColor(btn_crtext);
		m_OK_btn.SetTextColorD(btn_crtextd);
		m_OK_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Cancel_btn.SetSkinStretch(szPath);
		m_Cancel_btn.SetTextColor(btn_crtext);
		m_Cancel_btn.SetTextColorD(btn_crtextd);
		m_Cancel_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);

		SetLanguage();
	}
	else
	{
		if(m_hIcon) DestroyIcon(m_hIcon);
		if(m_hIcon2) DestroyIcon(m_hIcon2);
		if(m_BgBrush.m_hObject) m_BgBrush.DeleteObject();
	}

	return TRUE;
}

void CMemberDlg::SetLanguage()
{
	WCHAR szLan[64];
	_lang_(IDL_MEMBERSHIP, szLan, 64); SetWindowText(szLan);

	_lang_(IDL_NAME, szLan);
	GetDlgItem(IDC_STATIC_NAME)->SetWindowText(szLan);

	_lang_(IDL_USER_ID, szLan);
	GetDlgItem(IDC_STATIC_USER)->SetWindowText(szLan);

	_lang_(IDL_PASSWORD, szLan);
	GetDlgItem(IDC_STATIC_PASS)->SetWindowText(szLan);

	_lang_(IDL_JOIN_MEMBERSHIP, szLan);
	GetDlgItem(IDC_BTN_O)->SetWindowText(szLan);

	_lang_(IDL_CANCEL, szLan);
	GetDlgItem(IDC_BTN_C)->SetWindowText(szLan);
}

void CMemberDlg::OnBnClickedBtnC()
{
	EndDialog(IDCANCEL);
}

void CMemberDlg::OnBnClickedBtnO()
{
	USER_Item usItem;
	m_Name_edt.GetWindowText(usItem.szName, 64);
	m_User_edt.GetWindowText(usItem.szUser, 64);
	m_Pass_edt.GetWindowText(usItem.szPass, 256);
	usItem.uiLevel = 1;

	if(*usItem.szName == L'\0'){
		WCHAR szMsg[64]; _lang_(IDL_NAME, szMsg);
		AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
		return;
	}
	if(*usItem.szUser == L'\0'){
		WCHAR szMsg[64]; _lang_(IDL_USER_ID, szMsg);
		AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
		return;
	}
	if(*usItem.szPass == L'\0') {
		WCHAR szMsg[64]; _lang_(IDL_PASSWORD, szMsg);
		AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
		return;
	}

	USER_Search usSch;
	wcscpy_s(usSch.szUser, usItem.szUser);

	USER_Table usTbl;
	if(_db_user_Search(&usSch, &usTbl))
	{
		if(usTbl.Cnt())
		{
			WCHAR szMsg[64]; _lang_(IDL_ID_DUPLICATE, szMsg);
			AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
			return;
		}
	}

	if(!_db_user_Insert(&usItem))
	{
		m_Name_edt.SetWindowText(L"");
		m_User_edt.SetWindowText(L"");
		m_Pass_edt.SetWindowText(L"");
		m_Name_edt.SetFocus();
		return;
	}
	EndDialog(IDOK);
}
