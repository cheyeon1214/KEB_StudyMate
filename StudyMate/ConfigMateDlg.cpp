// ConfigMateDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ConfigMateDlg.h"

// CConfigMateDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CConfigMateDlg, CDialog)
CConfigMateDlg::CConfigMateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigMateDlg::IDD, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;

	m_bInitial = FALSE;
}

CConfigMateDlg::~CConfigMateDlg()
{
}

void CConfigMateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECK_USE_CAMERA, m_UseCam_btn);
	DDX_Control(pDX, IDC_EDIT_INDEX, m_Index_edt);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_Width_edt);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_Height_edt);
	DDX_Control(pDX, IDC_BTN_OK, m_OK_btn);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_Cancel_btn);
}


BEGIN_MESSAGE_MAP(CConfigMateDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_OK, OnBnClickedBtnOK)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CConfigMateDlg 메시지 처리기입니다.

BOOL CConfigMateDlg::OnInitDialog()
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

void CConfigMateDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

HBRUSH CConfigMateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CConfigMateDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN: return TRUE; break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CConfigMateDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CConfigMateDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\Dlg_Setting.ico", _cmn_IcoPath());
		m_hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		m_hIcon2 = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon2, FALSE);

		if(bg_crback != COLORREF(-1))
		{
			m_BgBrush.CreateSolidBrush(bg_crback);
			SetWindowTheme(GetDlgItem(IDC_EDIT_INDEX)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_WIDTH)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_HEIGHT)->m_hWnd, L"", L"");
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

		WCHAR szCfg[64];

		m_UseCam_btn.SetCheck(m_cfg.bUseCam);
		swprintf_s(szCfg, L"%d", m_cfg.index);
		m_Index_edt.SetWindowText(szCfg);
		swprintf_s(szCfg, L"%d", m_cfg.hsize);
		m_Width_edt.SetWindowText(szCfg);
		swprintf_s(szCfg, L"%d", m_cfg.vsize);
		m_Height_edt.SetWindowText(szCfg);

		if(m_cfg.bUseCam)
		{
			m_Index_edt.SetReadOnly(TRUE);
			m_Width_edt.SetReadOnly(TRUE);
			m_Height_edt.SetReadOnly(TRUE);
		}
	}
	else
	{
		if(m_hIcon) DestroyIcon(m_hIcon);
		if(m_hIcon2) DestroyIcon(m_hIcon2);
		if(m_BgBrush.m_hObject) m_BgBrush.DeleteObject();
	}

	return TRUE;
}
void CConfigMateDlg::SetLanguage()
{
	WCHAR szLan[64], szText[64];
	_lang_(IDL_SETTING, szLan); SetWindowText(szLan);

	WCHAR szFmt[64];
	_lang_(IDL_FMT_USE, szFmt);

	_lang_(IDL_CAMERA, szText);
	swprintf_s(szLan, szFmt, szText);
	GetDlgItem(IDC_CHECK_USE_CAMERA)->SetWindowText(szLan);

	_lang_(IDL_INDEX, szLan);
	GetDlgItem(IDC_STATIC_INDEX)->SetWindowText(szLan);

	_lang_(IDL_WIDTH, szLan);
	GetDlgItem(IDC_STATIC_WIDTH)->SetWindowText(szLan);

	_lang_(IDL_HEIGHT, szLan);
	GetDlgItem(IDC_STATIC_HEIGHT)->SetWindowText(szLan);

	_lang_(IDL_OK, szLan);
	GetDlgItem(IDC_BTN_OK)->SetWindowText(szLan);

	_lang_(IDL_CANCEL, szLan);
	GetDlgItem(IDC_BTN_CANCEL)->SetWindowText(szLan);
}

void CConfigMateDlg::OnBnClickedBtnOK()
{
	WCHAR szCfg[64];

	ConfigMate cfg; cfg = m_cfg;

	cfg.bUseCam = m_UseCam_btn.GetCheck();
	m_Index_edt.GetWindowText(szCfg, 64);
	cfg.index = _wtoi(szCfg);
	m_Width_edt.GetWindowText(szCfg, 64);
	cfg.hsize = _wtoi(szCfg);
	m_Height_edt.GetWindowText(szCfg, 64);
	cfg.vsize = _wtoi(szCfg);

	if(m_cfg != cfg)
	{
		m_cfg = cfg;
		EndDialog(IDOK);
	}
	else EndDialog(IDCANCEL);
}
void CConfigMateDlg::OnBnClickedBtnCancel()
{
	EndDialog(IDCANCEL);
}
