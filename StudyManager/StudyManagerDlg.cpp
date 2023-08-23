
// StudyManagerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "StudyManager.h"
#include "StudyManagerDlg.h"

#include "ConfigManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStudyManagerDlg 대화 상자


CStudyManagerDlg::CStudyManagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_STUDYMANAGER_DIALOG, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;

	m_bInitial = FALSE;

	m_pdx = NULL;
}

void CStudyManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStudyManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_GETMINMAXINFO()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CStudyManagerDlg 메시지 처리기

BOOL CStudyManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	WORD wversionRequested = MAKEWORD( 2, 2 ); 

	WSADATA wsadata;
	if(WSAStartup(wversionRequested, &wsadata) != 0) return FALSE;
	if(LOBYTE( wsadata.wVersion ) != 2 || HIBYTE( wsadata.wVersion ) != 2 ) return FALSE; 

	SetServerApp();

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CStudyManagerDlg::OnPaint()
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

			CRect rc;
			GetClientRect(&rc);
			GetCtrlRect(rc.Width(), rc.Height());

			CMemoryDC pDC(&dc, rc);

			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			int nSaveDC = MemDC.SaveDC();

			// Main draw /////////////////////////////////////////
			_cmn_gui_background(
				pDC, &MemDC,
				&m_bmpMain,
				&rc
			);
			// Main draw /////////////////////////////////////////

			// Scrn draw /////////////////////////////////////////
			_cmn_gui_stretchframe(
				pDC, &MemDC,
				&m_bmpScrn,
				&m_reScrn
			);
			// Scrn draw /////////////////////////////////////////

			MemDC.RestoreDC(nSaveDC);
			MemDC.DeleteDC();
		}
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CStudyManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CStudyManagerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);

	WSACleanup();
}

void CStudyManagerDlg::OnSize(UINT nType, int cx, int cy)
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

void CStudyManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch(nIDEvent)
	{
	case TMR_CLOCKID:
		{
		}
		break;
	case TMR_FDETECT:
		{
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CStudyManagerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if(m_bInitial)
	{
		lpMMI->ptMinTrackSize.x = m_xyDlg.Width();
		lpMMI->ptMinTrackSize.y = m_xyDlg.Height();
		lpMMI->ptMaxTrackSize.x = m_xyMAX.cx;
		lpMMI->ptMaxTrackSize.y = m_xyMAX.cy;
	}
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CStudyManagerDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDialog::OnRButtonUp(nFlags, point);

	if(m_bInitial)
	{
		if(m_reRender.PtInRect(point))
		{
			int SelCh = m_pdx ? _dxm_GetSelCh(m_pdx) : -1;
			if(SelCh < MAX_CLD)
			{
			}
		}
	}
}

BOOL CStudyManagerDlg::PreTranslateMessage(MSG* pMsg)
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
BOOL CStudyManagerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CStudyManagerDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		if(!_db_control_Init())
			return FALSE;
		if(!_codec_ffmpeg_Init())
			return FALSE;

		_lang_Set(ID_Korean);

		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\StudyManager.ico", _cmn_IcoPath());
		m_hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		m_hIcon2 = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon2, FALSE);

		swprintf_s(szPath, L"%s\\bg.bmp", _cmn_ImgPath());
		m_bmpMain.Attach((HBITMAP)::LoadImage(NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		swprintf_s(szPath, L"%s\\bgdisp.bmp", _cmn_ImgPath());
		m_bmpScrn.Attach((HBITMAP)::LoadImage(NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

		CSize xyWIN;
		xyWIN.cx = xyRECT.right - xyRECT.left;
		xyWIN.cy = xyRECT.bottom - xyRECT.top;

		CRect re, rc;
		GetWindowRect(&re);
		GetClientRect(&rc);
		xyWIN.cx += (re.Width() - rc.Width());
		xyWIN.cy += (re.Height() - rc.Height());

		SetWindowPos(NULL, 0, 0, xyWIN.cx, xyWIN.cy, SWP_NOMOVE);

		if(!m_ctlRender.Create(WS_CHILD | WS_VISIBLE, this))
			return FALSE;
		m_pdx = m_ctlRender.GetPdx();

		if(!m_ctlSvr.Init(this))
			return FALSE;
		for(int i = 0; i < MAX_CLD; i++)
		{
			if(!m_ctlCld[i].Init(this, m_pdx, i))
				return FALSE;
		}

		MoveCtrl(TRUE);

		SetLanguage();
	}
	else
	{
		m_ctlSvr.Exit();
		for(int i = 0; i < MAX_CLD; i++)
		{
			m_ctlCld[i].Exit();
		}
		Sleep(1000);

		m_pdx = NULL;
		m_ctlRender.DestroyWindow();

		if(m_bmpMain.m_hObject) m_bmpMain.DeleteObject();
		if(m_bmpScrn.m_hObject) m_bmpScrn.DeleteObject();

		if(m_hIcon) DestroyIcon(m_hIcon);
		if(m_hIcon2) DestroyIcon(m_hIcon2);

		_codec_ffmpeg_Exit();
		_db_control_Exit();
	}
	return TRUE;
}

void CStudyManagerDlg::SetLanguage()
{
	WCHAR szLan[64];
	_lang_(IDL_STUDY_MANAGER, szLan); SetWindowText(szLan);
}

void CStudyManagerDlg::GetCtrlRect(int cx, int cy)
{
	int nXInc = cx - m_xyDlg.Width();
	int nYInc = cy - m_xyDlg.Height();

	m_reTool = m_xyTool;
	m_reTool.right += nXInc;

	m_reSide = m_xySide;
	m_reSide.OffsetRect(nXInc, 0);
	m_reSide.bottom += nYInc;

	m_reScrn = m_xyScrn;
	m_reScrn.right += nXInc;
	m_reScrn.bottom += nYInc;

	m_reRender = m_reScrn;
	m_reRender.DeflateRect(8,8);
}
void CStudyManagerDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;

	if(bCreate)
	{
		m_xyMAX.cx = GetSystemMetrics(SM_CXSCREEN);
		m_xyMAX.cy = GetSystemMetrics(SM_CYSCREEN);

		m_xyDlg = xyRECT;
		m_xyTool = xyTOOL;
		m_xySide = xySIDE;
		m_xyScrn = xySCRN;
	}

	CRect rc;
	GetClientRect(&rc);
	GetCtrlRect(rc.Width(), rc.Height());

	if(bCreate)
	{
		m_dlgTool.Create(this);
		m_dlgTool.ShowWindow(SW_SHOW);
	}
	m_dlgTool.MoveWindow(&m_reTool);

	if(bCreate)
	{
		m_dlgSide.Create(this);
		m_dlgSide.ShowWindow(SW_SHOW);
	}
	m_dlgSide.MoveWindow(&m_reSide);


	m_ctlRender.MoveWindow(&m_reRender);
	m_ctlRender.SetFocus();
	
	Invalidate(FALSE);
}

BOOL CStudyManagerDlg::ACCEPT(SOCKET sdAccept)
{
	for(int i = 0; i < MAX_CLD; i++)
	{
		if(!m_ctlCld[i].IsValidSock())
		{
			return m_ctlCld[i].ACCEPT(sdAccept);
		}
	}
	return FALSE;
}
void CStudyManagerDlg::SendMsgTextSrc(WCHAR* szMsg)
{
	for(int i = 0; i < MAX_CLD; i++)
	{
		m_ctlCld[i].SendMsgTextSrc(szMsg);
	}
}
void CStudyManagerDlg::RecvMsgTextSrc(WCHAR* szMsg)
{
	m_dlgSide.RecvMsgTextSrc(szMsg);
}
void CStudyManagerDlg::SetDivMode(int nDiv)
{
	if(m_pdx)
	{
		if(_dxm_GetSelDiv(m_pdx) != nDiv)
			_dxm_SetDivMode(m_pdx, MSG_MULTISCREEN, nDiv);
		else _dxm_SetDivMode(m_pdx, MSG_SELROTATION, -1);
	}
}
void CStudyManagerDlg::ManagerSetting()
{
	CConfigManagerDlg dlg;
	dlg.Init(m_ctlSvr.GetCfg());
	if(dlg.DoModal() == IDOK)
	{
		m_ctlSvr.SetCfg(dlg.GetCfg());
	}
}
