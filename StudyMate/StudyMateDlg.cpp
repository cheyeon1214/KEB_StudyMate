
// StudyMateDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "StudyMate.h"
#include "StudyMateDlg.h"

#include "ConfigMateDlg.h"
#include "ConnectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStudyMateDlg 대화 상자


CStudyMateDlg::CStudyMateDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_STUDYMATE_DIALOG, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;

	m_bInitial = FALSE;

	m_pdx = NULL;

	wcscpy_s(m_szDuration, L"00:00:00");
}

void CStudyMateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_FRAME, m_Frame_stc);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_Msg_edt);
	DDX_Control(pDX, IDC_LIST_MESSAGE, m_Msg_lst);
	DDX_Control(pDX, IDC_BTN_MESSAGE, m_Msg_btn);
	DDX_Control(pDX, IDC_STATIC_LOGIN, m_Login_stc);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_Login_btn);
	DDX_Control(pDX, IDC_BTN_SETTING, m_Setting_btn);
	DDX_Control(pDX, IDC_BTN_MEMBER, m_Member_btn);
	DDX_Control(pDX, IDC_BTN_MANAGER, m_Manager_btn);
	DDX_Control(pDX, IDC_BTN_CONNECT, m_Connect_btn);
	DDX_Control(pDX, IDC_STATIC_DURATION, m_Duration_stc);
}

BEGIN_MESSAGE_MAP(CStudyMateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_MESSAGE, OnBnClickedBtnMessage)
END_MESSAGE_MAP()


// CStudyMateDlg 메시지 처리기

BOOL CStudyMateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	WORD wversionRequested = MAKEWORD( 2, 2 ); 

	WSADATA wsadata;
	if(WSAStartup(wversionRequested, &wsadata) != 0) return FALSE;
	if(LOBYTE( wsadata.wVersion ) != 2 || HIBYTE( wsadata.wVersion ) != 2 ) return FALSE; 

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	SetTimer(TMR_FDETECT, 200, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CStudyMateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(m_bInitial)
		{
			CPaintDC dc(this);

			CRect reDuration = GetScreen(m_Duration_stc, 2);
			dc.SetTextColor(RGB(255,255,255));
			dc.SetBkMode(TRANSPARENT);
			if(*m_szDuration != L'\0')
			{
				CFont* pOldFont = (CFont*)dc.SelectObject(&m_font);
				dc.DrawText(m_szDuration, reDuration, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
				dc.SelectObject(pOldFont);
			}
		}
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CStudyMateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CStudyMateDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	KillTimer(TMR_FDETECT);

	Initialize(FALSE);

	WSACleanup();
}

void CStudyMateDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(!m_bInitial)
		return;
	if((nType == SIZE_RESTORED) || (nType == SIZE_MAXIMIZED))
	{
		MoveCtrl();
	}
}

void CStudyMateDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnTimer(nIDEvent);

	switch(nIDEvent)
	{
	case TMR_CLOCKID:
		{
		}
		break;
	case TMR_FDETECT:
		{
			m_ctlCnt.Detection(TRUE);
		}
		break;
	}
}

HBRUSH CStudyMateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CStudyMateDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE: return TRUE; break;
		case VK_SPACE:
			if(GetDlgItem(IDC_EDIT_MESSAGE) != GetFocus())
				return TRUE;
			break;
		case VK_RETURN:
			if(GetDlgItem(IDC_EDIT_MESSAGE) == GetFocus())
				OnBnClickedBtnMessage();
			return TRUE;
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
BOOL CStudyMateDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
		switch(nID)
		{
		case IDC_BTN_LOGIN:
			if(m_Login_btn.GetCheck())
			{
				if(_db_Login(this) == IDOK)
				{
					USER_Item* pItem = _db_user_GetLoginUser();
					if(pItem)
					{
						m_ctlCnt.SetLoginUser(pItem);
						m_Login_stc.SetWindowText(pItem->szUser);
					}
				}
				else m_Login_btn.SetCheck(FALSE);
			}
			else
			{
				_db_user_Login(FALSE);
				m_ctlCnt.SetLoginUser(NULL);
				m_Login_stc.SetWindowText(L"log off");
			}
			break;
		case IDC_BTN_SETTING:
			{
				CConfigMateDlg dlg;
				dlg.Init(m_ctlCnt.GetCfg());
				if(dlg.DoModal() == IDOK)
				{
					m_ctlCnt.SetCfg(dlg.GetCfg());
				}
			}
			break;
		case IDC_BTN_MEMBER:
			{
				_db_Member(this);
			}
			break;
		case IDC_BTN_MANAGER:
			{
				_db_Manager(this);
			}
			break;
		case IDC_BTN_CONNECT:
			if(m_Connect_btn.GetCheck())
			{
				USER_Item* pItem = m_ctlCnt.GetLoginUser();
				if(pItem)
				{
					ConfigCon cfg;
					wcscpy_s(cfg.szUser, pItem->szUser);
					wcscpy_s(cfg.szPass, pItem->szPass);

					CConnectDlg dlg; dlg.Init(&cfg);
					if(dlg.DoModal() == IDOK)
					{
						m_ctlCnt.CONNECT(dlg.GetCfg());
					}
					else m_Connect_btn.SetCheck(FALSE);
				}
				else m_Connect_btn.SetCheck(FALSE);
			}
			else
			{
				m_ctlCnt.CLOSE();
			}
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CStudyMateDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		if(!_db_control_Init())
			return FALSE;
		if(!_codec_ffmpeg_Init())
			return FALSE;

		_lang_Set(ID_Korean);

		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\StudyMate.ico", _cmn_IcoPath());
		m_hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		m_hIcon2 = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon2, FALSE);

		m_font.CreateFontIndirect(&TIME_FONT);
		if(bg_crback != COLORREF(-1))
		{
			m_BgBrush.CreateSolidBrush(bg_crback);
			SetWindowTheme(GetDlgItem(IDC_EDIT_MESSAGE)->m_hWnd, L"", L"");
		}
		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());
		m_Setting_btn.SetSkinStretch(szPath);
		m_Setting_btn.SetTextColor(btn_crtext);
		m_Setting_btn.SetTextColorD(btn_crtextd);
		m_Setting_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Member_btn.SetSkinStretch(szPath);
		m_Member_btn.SetTextColor(btn_crtext);
		m_Member_btn.SetTextColorD(btn_crtextd);
		m_Member_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Manager_btn.SetSkinStretch(szPath);
		m_Manager_btn.SetTextColor(btn_crtext);
		m_Manager_btn.SetTextColorD(btn_crtextd);
		m_Manager_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Connect_btn.SetSkinStretch(szPath);
		m_Connect_btn.SetTextColor(btn_crtext);
		m_Connect_btn.SetTextColorD(btn_crtextd);
		m_Connect_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);

		m_Msg_btn.SetIconAuto(_cmn_IcoPath(), L"Dlg_Message.ico");
		m_Login_btn.SetIconAuto(_cmn_IcoPath(), L"Dlg_Lock.ico", BS_CENTER, L"Dlg_Unlock.ico");
		if(bg_crback != COLORREF(-1))
		{
			m_Msg_btn.SetShade(0, 8, 10, 100, bg_crback);
			m_Login_btn.SetShade(0, 8, 10, 100, bg_crback);
		}

		if(!m_ctlRender.Create(WS_CHILD | WS_VISIBLE, this))
			return FALSE;
		m_pdx = m_ctlRender.GetPdx();

		if(!m_ctlCnt.Init(this, m_pdx))
			return FALSE;

		MoveCtrl(TRUE);

		SetLanguage();
	}
	else
	{
		m_ctlCnt.Exit();

		Sleep(1000);

		m_pdx = NULL;
		m_ctlRender.DestroyWindow();

		if(m_font.m_hObject) m_font.DeleteObject();
		if(m_BgBrush.m_hObject) m_BgBrush.DeleteObject();

		if(m_hIcon) DestroyIcon(m_hIcon);
		if(m_hIcon2) DestroyIcon(m_hIcon2);

		_codec_ffmpeg_Exit();
		_db_control_Exit();
	}
	return TRUE;
}

void CStudyMateDlg::SetLanguage()
{
	WCHAR szLan[64];
	_lang_(IDL_STUDY_MATE, szLan); SetWindowText(szLan);

	_lang_(IDL_SETTING, szLan);
	GetDlgItem(IDC_BTN_SETTING)->SetWindowText(szLan);

	_lang_(IDL_MEMBERSHIP, szLan);
	GetDlgItem(IDC_BTN_MEMBER)->SetWindowText(szLan);

	_lang_(IDL_MEMBER_MANAGER, szLan);
	GetDlgItem(IDC_BTN_MANAGER)->SetWindowText(szLan);

	_lang_(IDL_STUDY_MANAGER, szLan);
	GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(szLan);
}

CRect CStudyMateDlg::GetScreen(CStatic& stc, int dft)
{
	CRect re;
	stc.GetWindowRect(&re);
	ScreenToClient(&re);
	re.DeflateRect(dft, dft);
	return re;
}
void CStudyMateDlg::SetListBoxText(CListBox& lst, const WCHAR* sz)
{
	int nItem = lst.AddString(sz);

	CSize size;
	CDC *pDC = lst.GetDC();
	
	CFont* pOld = pDC->SelectObject(lst.GetFont());

	if((lst.GetStyle() & LBS_USETABSTOPS) == 0)
	{
		size = pDC->GetTextExtent(sz, (int)wcslen(sz));
		size.cx += 3;
	}
	else
	{
		size = pDC->GetTabbedTextExtent(sz, (int)wcslen(sz), 0, NULL);
		size.cx += 2;
	}
	pDC->SelectObject(pOld);
	lst.ReleaseDC(pDC);

	if(size.cx > lst.GetHorizontalExtent()) lst.SetHorizontalExtent(size.cx);
}

void CStudyMateDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;

	if(bCreate)
	{
		GetClientRect(&m_xyDlg);
	}
	else
	{
		CRect re;
		GetClientRect(&re);
		int cx = re.Width() - m_xyDlg.Width();
		int cy = re.Height() - m_xyDlg.Height();
	}

	CRect reScreen = GetScreen(m_Frame_stc);
	m_ctlRender.MoveWindow(&reScreen);
	
	Invalidate(FALSE);
}

void CStudyMateDlg::NetCon(BOOL bNetCon)
{
	if(m_Connect_btn.GetCheck() != bNetCon)
	{
		m_Connect_btn.SetCheck(bNetCon);
	}
}
void CStudyMateDlg::RecvMsgTextSrc(WCHAR* szMsg)
{
	SetListBoxText(m_Msg_lst, szMsg);
}
void CStudyMateDlg::SetDuration(DTime dtDuration)
{
	WCHAR szDuration[64];
	wcscpy_s(szDuration, L"00:00:00");
	if(dtDuration)
	{
		DTime hh = dtDuration / _DTIME_HOUR; dtDuration -= (hh * _DTIME_HOUR);
		DTime mm = dtDuration / _DTIME_MINUTE; dtDuration -= (mm * _DTIME_MINUTE);
		DTime ss = dtDuration / _DTIME_SECOND; dtDuration -= (ss * _DTIME_SECOND);
		swprintf_s(szDuration, L"%02d:%02d:%02d", (int)hh, (int)mm, (int)ss);
	}
	if(wcscmp(m_szDuration, szDuration) != 0)
	{
		wcscpy_s(m_szDuration, szDuration);
		InvalidateRect(GetScreen(m_Duration_stc, 2));
	}
}

void CStudyMateDlg::OnBnClickedBtnMessage()
{
	WCHAR szMsg[256];
	m_Msg_edt.GetWindowText(szMsg, 256);
	if(*szMsg != L'\0')
	{
		m_ctlCnt.SendMsgTextSrc(szMsg);

		USER_Item* pItem = _db_user_GetLoginUser();
		if(pItem)
		{
			WCHAR szText[256];
			swprintf_s(szText, L"%s : %s", pItem->szName, szMsg);
			SetListBoxText(m_Msg_lst, szText);
		}
	}
	m_Msg_edt.SetWindowText(L"");
}
