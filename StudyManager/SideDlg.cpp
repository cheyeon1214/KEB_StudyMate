// SideDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "SideDlg.h"

#include "StudyManagerDlg.h"

// CSideDlg 대화 상자입니다.

#define IDC_BTN_MEMBER	10000
#define IDC_BTN_MANAGER	10001

IMPLEMENT_DYNAMIC(CSideDlg, CDialog)

CSideDlg::CSideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSideDlg::IDD, pParent)
{
	m_bInitial = FALSE;
	m_pDlg = NULL;
}

CSideDlg::~CSideDlg()
{
}

void CSideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSideDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSideDlg 메시지 처리기입니다.

BOOL CSideDlg::OnInitDialog()
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

void CSideDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CSideDlg::OnPaint()
{
	CPaintDC dc(this);

	if(m_bInitial)
	{
		CRect rc;
		GetClientRect(&rc);
		GetCtrlRect(rc.Width(), rc.Height());

		CMemoryDC pDC(&dc, rc);

		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		int nSaveDC = MemDC.SaveDC();

		_cmn_gui_stretchframe(
			pDC, &MemDC,
			&m_bmpMain,
			&rc
			);

		MemDC.RestoreDC(nSaveDC);
		MemDC.DeleteDC();
	}
}

void CSideDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(!m_bInitial)
		return;
	if(nType == SIZE_RESTORED)
	{
		MoveCtrl();
	}
}

BOOL CSideDlg::PreTranslateMessage(MSG* pMsg)
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
BOOL CSideDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
		switch(nID)
		{
		case IDC_BTN_MEMBER:
			{
				_db_Membership(this);
			}
			break;
		case IDC_BTN_MANAGER:
			{
				_db_Manager(this);
			}
			break;
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CSideDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\bg_side.bmp", _cmn_ImgPath());
		m_bmpMain.Attach((HBITMAP)::LoadImage(NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

		MoveCtrl(TRUE);
	}
	else
	{
		if(m_bmpMain.m_hObject) m_bmpMain.DeleteObject();
	}
	return TRUE;
}

void CSideDlg::GetCtrlRect(int cx, int cy)
{
	int nXInc = cx - m_xyDlg.Width();
	int nYInc = cy - m_xyDlg.Height();

	m_reCtl_Side = xySIDE_ctrl;
	m_reCtl_Side.bottom += nYInc;

	m_reMemberBtn = xySIDE_member_btn;
	m_reMemberBtn.OffsetRect(0, nYInc);

	m_reManagerBtn = xySIDE_manager_btn;
	m_reManagerBtn.OffsetRect(0, nYInc);
}
void CSideDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;

	if(bCreate)
	{
		m_xyDlg = xySIDE;
	}

	CRect rc;
	GetClientRect(&rc);
	GetCtrlRect(rc.Width(), rc.Height());

	if(bCreate)
	{
		m_dlgMsg.Create(this);
		m_dlgMsg.ShowWindow(SW_SHOW);
	}
	else m_dlgMsg.MoveWindow(&m_reCtl_Side);

	if(bCreate)
	{
		WCHAR szPath[MAX_PATH], szLan[64];

		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());

		_lang_(IDL_MEMBER_MANAGEMENT, szLan);
		CRect reMemberBtn(xySIDE_member_btn);
		m_member_btn.Create(szLan, dwPushStyle, reMemberBtn, this, IDC_BTN_MEMBER);
		m_member_btn.SetSkinStretch(szPath);
		m_member_btn.SetTextColor(btn_crtext);
		m_member_btn.SetTextColorD(btn_crtextd);
		m_member_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);

		_lang_(IDL_MEMBER_MANAGER, szLan);
		CRect reManagerBtn(xySIDE_manager_btn);
		m_manager_btn.Create(szLan, dwPushStyle, reManagerBtn, this, IDC_BTN_MANAGER);
		m_manager_btn.SetSkinStretch(szPath);
		m_manager_btn.SetTextColor(btn_crtext);
		m_manager_btn.SetTextColorD(btn_crtextd);
		m_manager_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
	}
	else
	{
		m_member_btn.MoveWindow(&m_reMemberBtn);
		m_manager_btn.MoveWindow(&m_reManagerBtn);
	}
}

BOOL CSideDlg::Create(CStudyManagerDlg* pParent)
{
	m_pDlg = pParent;
	return CDialog::Create(CSideDlg::IDD, pParent);
}

void CSideDlg::SendMsgTextSrc(WCHAR* szMsg)
{
	if(m_pDlg)
	{
		m_pDlg->SendMsgTextSrc(szMsg);
	}
}
void CSideDlg::RecvMsgTextSrc(WCHAR* szMsg)
{
	m_dlgMsg.RecvMsgTextSrc(szMsg);
}
