// ToolDlg.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "ToolDlg.h"

#include "StudyManagerDlg.h"

// CToolDlg ��ȭ �����Դϴ�.

#define IDC_BTN_DIV		10000
#define IDC_BTN_SETTING	10100

IMPLEMENT_DYNAMIC(CToolDlg, CDialog)

CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolDlg::IDD, pParent)
{
	m_bInitial = FALSE;
	m_pDlg = NULL;
}

CToolDlg::~CToolDlg()
{
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CToolDlg �޽��� ó�����Դϴ�.

BOOL CToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CToolDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CToolDlg::OnPaint()
{
	CPaintDC dc(this);
	
	if(m_bInitial)
	{
		CRect rc;
		GetClientRect(&rc);
		GetCtrlRect(rc.Width(), rc.Height());

		CMemoryDC pDC(&dc, rc);
		pDC->SetBkMode(TRANSPARENT);

		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		int nSaveDC = MemDC.SaveDC();

		// Main bitmap Draw /////////////////////////////////////////
		_cmn_gui_stretchframe(
			pDC, &MemDC,
			&m_bmpMain,
			&rc
			);
		////////////////////////////////////////////////////////////

		MemDC.RestoreDC(nSaveDC);
		MemDC.DeleteDC();
	}
}

void CToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if(!m_bInitial)
		return;
	if(nType == SIZE_RESTORED)
	{
		MoveCtrl();
	}
}

BOOL CToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
		case VK_SPACE: return TRUE; break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
BOOL CToolDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
		if(nID >= IDC_BTN_DIV && nID < IDC_BTN_DIV + _UseScr())
		{
			int nDiv = nID - IDC_BTN_DIV;
			if(m_pDlg)
			{
				m_pDlg->SetDivMode(nDiv);
			}
		}
		else
		{
			switch(nID)
			{
			case IDC_BTN_SETTING:
				if(m_pDlg)
				{
					m_pDlg->ManagerSetting();
				}
				break;
			}
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CToolDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\bg_tool.bmp", _cmn_ImgPath());
		m_bmpMain.Attach((HBITMAP)::LoadImage(NULL, szPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

		MoveCtrl(TRUE);
	}
	else
	{
		if(m_bmpMain.m_hObject) m_bmpMain.DeleteObject();
	}
	return TRUE;
}

void CToolDlg::GetCtrlRect(int cx, int cy)
{
	int nXInc = cx - m_xyDlg.Width();
	int nYInc = cy - m_xyDlg.Height();

	m_reSettingBtn = xyTOOL_setting_btn;
	m_reSettingBtn.OffsetRect(nXInc, 0);
}
void CToolDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;

	if(bCreate)
	{
		m_xyDlg = xyTOOL;
	}

	CRect rc;
	GetClientRect(&rc);
	GetCtrlRect(rc.Width(), rc.Height());

	if(bCreate)
	{
		WCHAR szPath[MAX_PATH], szLan[64];

		CRect reBtn(xyTOOL_div_btn);
		for(int div = 0; div < _UseScr(); div++)
		{
			m_div_btn[div].Create(NULL, dwPushStyle, reBtn, this, IDC_BTN_DIV + div);
			reBtn.OffsetRect(xyTOOL_div_interval, 0);
		}
		swprintf_s(szPath, L"%s\\TOOL\\div01_NXN.bmp", _cmn_ImgPath()); m_div_btn[SCR_NXN_01].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div02_1X2.bmp", _cmn_ImgPath()); m_div_btn[SCR_1X2_02].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div04_NXN.bmp", _cmn_ImgPath()); m_div_btn[SCR_NXN_04].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div06_1XN.bmp", _cmn_ImgPath()); m_div_btn[SCR_1XN_06].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div09_NXN.bmp", _cmn_ImgPath()); m_div_btn[SCR_NXN_09].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div10_2XN.bmp", _cmn_ImgPath()); m_div_btn[SCR_2XN_10].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div13_1XN.bmp", _cmn_ImgPath()); m_div_btn[SCR_1XN_13].SetSkin(szPath);
		swprintf_s(szPath, L"%s\\TOOL\\div16_NXN.bmp", _cmn_ImgPath()); m_div_btn[SCR_NXN_16].SetSkin(szPath);

		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());

		_lang_(IDL_SETTING, szLan);
		CRect reSettingBtn(xyTOOL_setting_btn);
		m_setting_btn.Create(szLan, dwPushStyle, reSettingBtn, this, IDC_BTN_SETTING);
		m_setting_btn.SetSkinStretch(szPath);
		m_setting_btn.SetTextColor(btn_crtext);
		m_setting_btn.SetTextColorD(btn_crtextd);
		m_setting_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
	}
	else
	{
		m_setting_btn.MoveWindow(&m_reSettingBtn);
	}
}

BOOL CToolDlg::Create(CStudyManagerDlg* pParent)
{
	m_pDlg = pParent;
	return CDialog::Create(CToolDlg::IDD, pParent);
}
