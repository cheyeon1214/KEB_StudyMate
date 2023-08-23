// MessageDlg.cpp : ���� �����Դϴ�.
//

#include "pch.h"
#include "MessageDlg.h"

#include "SideDlg.h"

// CMessageDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMessageDlg, CDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	m_bInitial = FALSE;
	m_pDlg = NULL;

	wcscpy_s(m_szManager, L"Manager");
}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_Msg_edt);
	DDX_Control(pDX, IDC_LIST_MESSAGE, m_Msg_lst);
	DDX_Control(pDX, IDC_BTN_MESSAGE, m_Msg_btn);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_MESSAGE, &CMessageDlg::OnBnClickedBtnMessage)
END_MESSAGE_MAP()


// CMessageDlg �޽��� ó�����Դϴ�.

BOOL CMessageDlg::OnInitDialog()
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

void CMessageDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CMessageDlg::OnSize(UINT nType, int cx, int cy)
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

HBRUSH CMessageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
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

BOOL CMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
BOOL CMessageDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotifyCode = HIWORD(wParam);

	if(wNotifyCode == BN_CLICKED)
	{
		int nID = (int)LOWORD(wParam);
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CMessageDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		if(bg_crback != COLORREF(-1))
		{
			m_BgBrush.CreateSolidBrush(bg_crback);
			SetWindowTheme(GetDlgItem(IDC_EDIT_MESSAGE)->m_hWnd, L"", L"");
		}

		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());
		m_Msg_btn.SetSkinStretch(szPath);
		m_Msg_btn.SetTextColor(btn_crtext);
		m_Msg_btn.SetTextColorD(btn_crtextd);
		m_Msg_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);

		MoveCtrl(TRUE);

		SetLanguage();
	}
	else
	{
		if(m_BgBrush.m_hObject) m_BgBrush.DeleteObject();
	}
	return TRUE;
}
void CMessageDlg::SetListBoxText(CListBox& lst, const WCHAR* sz)
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

void CMessageDlg::SetLanguage()
{
	WCHAR szLan[64];
	_lang_(IDL_SEND, szLan);
	GetDlgItem(IDC_BTN_MESSAGE)->SetWindowText(szLan);

	_lang_(IDL_MANAGER, m_szManager);
}

void CMessageDlg::GetCtrlRect(int cx, int cy)
{
	int nXInc = cx - m_xyDlg.Width();
	int nYInc = cy - m_xyDlg.Height();

	m_reMsg_List = m_xyMsg_List;
	m_reMsg_List.bottom += nYInc;
}
void CMessageDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;

	if(bCreate)
	{
		GetClientRect(&m_xyDlg);
		GetDlgItem(IDC_LIST_MESSAGE)->GetWindowRect(&m_xyMsg_List);
		ScreenToClient(&m_xyMsg_List);
	}

	CRect rc;
	GetClientRect(&rc);
	GetCtrlRect(rc.Width(), rc.Height());
	GetDlgItem(IDC_LIST_MESSAGE)->MoveWindow(&m_reMsg_List);
}

BOOL CMessageDlg::Create(CSideDlg* pParent)
{
	m_pDlg = pParent;
	return CDialog::Create(CMessageDlg::IDD, pParent);
}

void CMessageDlg::RecvMsgTextSrc(WCHAR* szMsg)
{
	SetListBoxText(m_Msg_lst, szMsg);
}

void CMessageDlg::OnBnClickedBtnMessage()
{
	WCHAR szMsg[256];
	m_Msg_edt.GetWindowText(szMsg, 256);
	if(*szMsg != L'\0')
	{
		if(m_pDlg)
		{
			m_pDlg->SendMsgTextSrc(szMsg);
		}

		WCHAR szText[256];
		swprintf_s(szText, L"%s : %s", m_szManager, szMsg);
		SetListBoxText(m_Msg_lst, szText);
	}
	m_Msg_edt.SetWindowText(L"");
}
