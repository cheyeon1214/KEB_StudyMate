// MembershipDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "MembershipDlg.h"

#include "DefsGridList.h"
#include "ListDemoViewPrint.h"

enum
{
	lst_itm_Insert = 0,

	lst_itm_Name,
	lst_itm_User,
	lst_itm_Pass,
	lst_itm_Note,
};

// CMembershipDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMembershipDlg, CDialog)
CMembershipDlg::CMembershipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMembershipDlg::IDD, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;

	m_bInitial = FALSE;

	SelItem = -1;

	m_iScale = 1;
}

CMembershipDlg::~CMembershipDlg()
{
}

void CMembershipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_USER, m_User_lst);

	DDX_Control(pDX, IDC_EDIT_NAME, m_Name_edt);
	DDX_Control(pDX, IDC_EDIT_USER, m_User_edt);
	DDX_Control(pDX, IDC_EDIT_PASS, m_Pass_edt);
	DDX_Control(pDX, IDC_EDIT_NOTE, m_Note_edt);

	DDX_Control(pDX, IDC_BTN_ADD, m_Add_btn);
	DDX_Control(pDX, IDC_BTN_EDT, m_Edt_btn);
	DDX_Control(pDX, IDC_BTN_DEL, m_Del_btn);
	DDX_Control(pDX, IDC_BTN_APPLY, m_Apply_btn);
	DDX_Control(pDX, IDC_BTN_OPEN_FILE, m_OpenFile_btn);
}


BEGIN_MESSAGE_MAP(CMembershipDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_LIST_USER, OnNMClickListUser)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_EDT, OnBnClickedBtnEdt)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_OPEN_FILE, OnBnClickedBtnOpenFile)
END_MESSAGE_MAP()


// CMembershipDlg 메시지 처리기입니다.

BOOL CMembershipDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	m_usTbl.Clear();

	USER_Search usSch;
	if(_db_user_Search(&usSch, &m_usTbl))
	{
		m_oldTbl = m_usTbl;

		SetUserList();
		SetUserItem();

		USER_Item* pLogin = _db_user_GetLoginUser();
		if(pLogin)
		{
			int nItem = GetUserList(pLogin->szUser, pLogin->szPass);
			if(nItem >= 0)
			{
				SetUserItem(nItem);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMembershipDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CMembershipDlg::OnPaint()
{
	CPaintDC dc(this);

	if(m_bInitial)
	{
		CDC* pDC = &dc;
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetStretchBltMode(HALFTONE);

		if(!m_imgUser.IsNull())
		{
			CRect reDst = GetScreen(this, IDC_STATIC_FRAME);
			m_imgUser.StretchBlt(dc, reDst.left, reDst.top, reDst.Width(), reDst.Height(), SRCCOPY);
		}
	}
}

void CMembershipDlg::OnSize(UINT nType, int cx, int cy)
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

HBRUSH CMembershipDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CMembershipDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CMembershipDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	WORD wNotifyCode = HIWORD(wParam);
	if(wNotifyCode == BN_CLICKED)
	{
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CMembershipDlg::Initialize(BOOL fInitialize)
{
	if(fInitialize)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s\\Dlg_Logview.ico", _cmn_IcoPath());
		m_hIcon = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
		m_hIcon2 = (HICON)::LoadImage(NULL, szPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon2, FALSE);

		if(bg_crback != COLORREF(-1))
		{
			m_BgBrush.CreateSolidBrush(bg_crback);
			SetWindowTheme(GetDlgItem(IDC_STATIC_INFORM)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_STATIC_PICTURE)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_NAME)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_USER)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_PASS)->m_hWnd, L"", L"");
			SetWindowTheme(GetDlgItem(IDC_EDIT_NOTE)->m_hWnd, L"", L"");
		}

		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());
		m_Add_btn.SetSkinStretch(szPath);
		m_Add_btn.SetTextColor(btn_crtext);
		m_Add_btn.SetTextColorD(btn_crtextd);
		m_Add_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Edt_btn.SetSkinStretch(szPath);
		m_Edt_btn.SetTextColor(btn_crtext);
		m_Edt_btn.SetTextColorD(btn_crtextd);
		m_Edt_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Del_btn.SetSkinStretch(szPath);
		m_Del_btn.SetTextColor(btn_crtext);
		m_Del_btn.SetTextColorD(btn_crtextd);
		m_Del_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_Apply_btn.SetSkinStretch(szPath);
		m_Apply_btn.SetTextColor(btn_crtext);
		m_Apply_btn.SetTextColorD(btn_crtextd);
		m_Apply_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);
		m_OpenFile_btn.SetSkinStretch(szPath);
		m_OpenFile_btn.SetTextColor(btn_crtext);
		m_OpenFile_btn.SetTextColorD(btn_crtextd);
		m_OpenFile_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);

		Init_ListCtrl();

		MoveCtrl(TRUE);

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
void CMembershipDlg::Init_ListCtrl()
{
	m_User_lst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB);

	m_User_lst.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
	m_User_lst.SetDefaultRowTrait(pRowTrait);
	
	m_User_lst.InsertHiddenLabelColumn();

	m_iScale = GetStringColumnScale(&m_User_lst, L"0000-00-00 00:00:00", 130);

	G_InsColumn(&m_User_lst, lst_itm_Name, L"Name", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_User_lst, lst_itm_User, L"User ID", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_User_lst, lst_itm_Pass, L"Password", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_User_lst, lst_itm_Note, L"Description", 300*m_iScale, G_Column(G_CTRL_Text), false, LVCFMT_LEFT);
}
int CMembershipDlg::GetUserList(WCHAR* szUser, WCHAR* szPass)
{
	for(int itm = 0; itm < m_usTbl.Cnt(); itm++)
	{
		USER_Item* pItem = m_usTbl.Obj(itm);
		if(pItem)
		{
			if(wcscmp(szUser, pItem->szUser) == 0)
			{
				if(szPass != NULL)
				{
					if(wcscmp(szPass, pItem->szPass) == 0)
						return itm;
				}
				else return itm;
			}
		}
	}
	return -1;
}

void CMembershipDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;
	
	if(bCreate)
	{
		GetClientRect(&m_xyDlg);

		GetDlgItem(IDC_LIST_USER)->GetWindowRect(&m_xyList);
		ScreenToClient(&m_xyList);
		GetDlgItem(IDC_BTN_APPLY)->GetWindowRect(&m_xyApply);
		ScreenToClient(&m_xyApply);
		GetDlgItem(IDC_STATIC_PICTURE)->GetWindowRect(&m_xyPicture);
		ScreenToClient(&m_xyPicture);
	}
	else
	{
		CRect re;
		GetClientRect(&re);
		int cx = re.Width() - m_xyDlg.Width();
		int cy = re.Height() - m_xyDlg.Height();

		CRect reList(&m_xyList);
		reList.right += cx;
		reList.bottom += cy;
		GetDlgItem(IDC_LIST_USER)->MoveWindow(&reList);

		CRect reApply(&m_xyApply);
		reApply.right += cx;
		GetDlgItem(IDC_BTN_APPLY)->MoveWindow(&reApply);

		CRect rePicture(&m_xyPicture);
		rePicture.right += cx;
		GetDlgItem(IDC_STATIC_PICTURE)->MoveWindow(&rePicture);

		CRect reUser;
		m_User_lst.GetClientRect(&reUser);

		int LogColumnWidth = 20*m_iScale;
		LogColumnWidth += m_User_lst.GetColumnWidth(lst_itm_Name);
		LogColumnWidth += m_User_lst.GetColumnWidth(lst_itm_User);
		LogColumnWidth += m_User_lst.GetColumnWidth(lst_itm_Pass);

		int nWidth = reUser.Width() - LogColumnWidth;
		if(nWidth < 300*m_iScale) nWidth = 300*m_iScale;

		m_User_lst.SetColumnWidth(lst_itm_Note, nWidth);
	}
}

void CMembershipDlg::SetLanguage()
{
	WCHAR szLan[64];
	_lang_(IDL_MEMBERSHIP, szLan); SetWindowText(szLan);

	_lang_(IDL_INFORMATION, szLan);
	GetDlgItem(IDC_STATIC_INFORM)->SetWindowText(szLan);

	_lang_(IDL_NAME, szLan);
	GetDlgItem(IDC_STATIC_NAME)->SetWindowText(szLan);

	_lang_(IDL_USER_ID, szLan);
	GetDlgItem(IDC_STATIC_USER)->SetWindowText(szLan);

	_lang_(IDL_PASSWORD, szLan);
	GetDlgItem(IDC_STATIC_PASS)->SetWindowText(szLan);

	_lang_(IDL_DESCRIPTION, szLan);
	GetDlgItem(IDC_STATIC_NOTE)->SetWindowText(szLan);

	_lang_(IDL_PICTURE, szLan);
	GetDlgItem(IDC_STATIC_PICTURE)->SetWindowText(szLan);

	_lang_(IDL_ADD, szLan);
	GetDlgItem(IDC_BTN_ADD)->SetWindowText(szLan);

	_lang_(IDL_EDIT, szLan);
	GetDlgItem(IDC_BTN_EDT)->SetWindowText(szLan);

	_lang_(IDL_DELETE, szLan);
	GetDlgItem(IDC_BTN_DEL)->SetWindowText(szLan);

	_lang_(IDL_SAVE, szLan);
	GetDlgItem(IDC_BTN_APPLY)->SetWindowText(szLan);

	_lang_(IDL_OPEN_FILE, szLan);
	GetDlgItem(IDC_BTN_OPEN_FILE)->SetWindowText(szLan);

	LVCOLUMN col; col.mask = LVCF_TEXT;

	_lang_(IDL_NAME, szLan);
	col.pszText = szLan;
	m_User_lst.SetColumn(lst_itm_Name, &col);

	_lang_(IDL_USER_ID, szLan);
	col.pszText = szLan;
	m_User_lst.SetColumn(lst_itm_User, &col);

	_lang_(IDL_PASSWORD, szLan);
	col.pszText = szLan;
	m_User_lst.SetColumn(lst_itm_Pass, &col);

	_lang_(IDL_DESCRIPTION, szLan);
	col.pszText = szLan;
	m_User_lst.SetColumn(lst_itm_Note, &col);
}

void CMembershipDlg::SetUserList(int nItem)
{
	if(nItem >= 0)
	{
		USER_Item* pItem = m_usTbl.Obj(nItem);
		if(!pItem) return;

		WCHAR szPass[256]; *szPass = L'\0';
		for(int i = 0; i < wcslen(pItem->szPass); i++) wcscat_s(szPass, L"*");

		int nItemCnt = m_User_lst.GetItemCount();
		if(nItem < nItemCnt)
		{
			m_User_lst.SetItemText(nItem, lst_itm_Name, pItem->szName);
			m_User_lst.SetItemText(nItem, lst_itm_User, pItem->szUser);
			m_User_lst.SetItemText(nItem, lst_itm_Pass, szPass);
			m_User_lst.SetItemText(nItem, lst_itm_Note, pItem->szNote);
		}
		else if(nItem == nItemCnt)
		{
			LVITEM	lvitem;
			lvitem.mask	= LVIF_TEXT;
			lvitem.iItem = nItem;
			lvitem.iSubItem	= lst_itm_Insert;
			lvitem.pszText = L"Insert";
			m_User_lst.InsertItem(&lvitem);
			m_User_lst.SetItemText(nItem, lst_itm_Name, pItem->szName);
			m_User_lst.SetItemText(nItem, lst_itm_User, pItem->szUser);
			m_User_lst.SetItemText(nItem, lst_itm_Pass, szPass);
			m_User_lst.SetItemText(nItem, lst_itm_Note, pItem->szNote);
		}
	}
	else
	{
		m_User_lst.DeleteAllItems();

		LVITEM	lvitem;
		lvitem.mask	= LVIF_TEXT;
		for(int itm=0; itm<m_usTbl.Cnt(); itm++)
		{
			USER_Item* pItem = m_usTbl.Obj(itm);
			if(pItem)
			{
				WCHAR szPass[256]; *szPass = L'\0';
				for(int i = 0; i < wcslen(pItem->szPass); i++) wcscat_s(szPass, L"*");

				lvitem.iItem = itm;
				lvitem.iSubItem	= lst_itm_Insert;
				lvitem.pszText = L"Insert";
				m_User_lst.InsertItem(&lvitem);
				m_User_lst.SetItemText(itm, lst_itm_Name, pItem->szName);
				m_User_lst.SetItemText(itm, lst_itm_User, pItem->szUser);
				m_User_lst.SetItemText(itm, lst_itm_Pass, szPass);
				m_User_lst.SetItemText(itm, lst_itm_Note, pItem->szNote);
			}
		}
	}
}
void CMembershipDlg::SetUserItem(int nItem)
{
	if(!m_imgUser.IsNull())
	{
		m_imgUser.Destroy();
	}
	if(nItem >= 0)
	{
		SelItem = nItem;

		USER_Item* pItem = m_usTbl.Obj(nItem);
		if(pItem)
		{
			m_Name_edt.SetWindowText(pItem->szName);
			m_User_edt.SetWindowText(pItem->szUser);
			m_Pass_edt.SetWindowText(pItem->szPass);
			m_Note_edt.SetWindowText(pItem->szNote);

			CFileFind finder;
			WCHAR szImage[MAX_PATH];
			swprintf_s(szImage, L"%s\\%s.bmp", _cmn_DbPath(), pItem->szUser);
			if(finder.FindFile(szImage)) m_imgUser.Load(szImage);
		}
	}
	else
	{
		SelItem = -1;

		m_Name_edt.SetWindowText(L"");
		m_User_edt.SetWindowText(L"");
		m_Pass_edt.SetWindowText(L"");
		m_Note_edt.SetWindowText(L"");
	}
	InvalidateRect(GetScreen(this, IDC_STATIC_FRAME), TRUE);
}
BOOL CMembershipDlg::GetUserItem(USER_Item* pItem)
{
	WCHAR szName[64];
	m_Name_edt.GetWindowText(szName, 64);
	if(*szName == L'\0') {
		WCHAR szMsg[64]; _lang_(IDL_NAME, szMsg);
		AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	WCHAR szUser[64];
	m_User_edt.GetWindowText(szUser, 64);
	if(*szUser == L'\0'){
		WCHAR szMsg[64]; _lang_(IDL_USER_ID, szMsg);
		AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	WCHAR szPass[256];
	m_Pass_edt.GetWindowText(szPass, 256);
	if(*szPass == L'\0'){
		WCHAR szMsg[64]; _lang_(IDL_PASSWORD, szMsg);
		AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	wcscpy_s(pItem->szName, szName);
	wcscpy_s(pItem->szUser, szUser);
	wcscpy_s(pItem->szPass, szPass);
	m_Note_edt.GetWindowText(pItem->szNote, 256);

	pItem->uiLevel = 1;

	return TRUE;
}

void CMembershipDlg::OnNMClickListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nItemCnt = m_User_lst.GetItemCount();
	if(nItemCnt)
	{
		POSITION pos = m_User_lst.GetFirstSelectedItemPosition();
		if( pos != NULL )
		{
			int nItem = m_User_lst.GetNextSelectedItem(pos);
			if(m_usTbl.Obj(nItem))
			{
				SetUserItem(nItem);
			}
		}
	}

	*pResult = 0;
}
void CMembershipDlg::OnBnClickedBtnAdd()
{
	USER_Item usItem;
	if(GetUserItem(&usItem))
	{
		if(GetUserList(usItem.szUser) >= 0)
		{
			WCHAR szMsg[64]; _lang_(IDL_ID_DUPLICATE, szMsg);
			AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
			return;
		}

		usItem.No = 0;
		int nItem = m_usTbl.Cnt();
		if(nItem)
		{
			if(m_usTbl.Obj(nItem-1))
			{
				usItem.No = (m_usTbl.Obj(nItem-1))->No + 1;
			}
		}
		m_usTbl.Insert(&usItem);
		
		if(!m_imgUser.IsNull())
		{
			WCHAR szImage[MAX_PATH];
			swprintf_s(szImage, L"%s\\%s.bmp", _cmn_DbPath(), usItem.szUser);
			m_imgUser.Save(szImage);
		}
		SetUserList(nItem);

		SelItem = nItem;
	}
}
void CMembershipDlg::OnBnClickedBtnEdt()
{
	USER_Item usItem;
	if(GetUserItem(&usItem))
	{
		if(SelItem < 0) return;

		USER_Item* pItem = m_usTbl.Obj(SelItem);
		if(pItem)
		{
			if(wcscmp(usItem.szUser, pItem->szUser) != 0)
			{
				WCHAR szMsg[64]; _lang_(IDL_USER_ID, szMsg);
				AfxMessageBox(szMsg, MB_OK|MB_ICONSTOP);
				return;
			}

			usItem.No = pItem->No;
			*(pItem) = usItem;

			if(!m_imgUser.IsNull())
			{
				WCHAR szImage[MAX_PATH];
				swprintf_s(szImage, L"%s\\%s.bmp", _cmn_DbPath(), usItem.szUser);
				m_imgUser.Save(szImage);
			}
			SetUserList(SelItem);
		}
	}
}
void CMembershipDlg::OnBnClickedBtnDel()
{
	if(SelItem < 0) return;

	USER_Item* pItem = m_usTbl.Obj(SelItem);
	if(pItem)
	{
		if(!m_imgUser.IsNull())
		{
			WCHAR szImage[MAX_PATH];
			swprintf_s(szImage, L"%s\\%s.bmp", _cmn_DbPath(), pItem->szUser);
			::DeleteFile(szImage);
		}
		m_usTbl.Delete(SelItem);

		SetUserList();
		SetUserItem();
	}
}

void CMembershipDlg::OnBnClickedBtnApply()
{
	if(m_oldTbl != m_usTbl)
	{
		WCHAR szMsg[64]; _lang_(IDL_SETUP_MODIFY, szMsg);
		if(AfxMessageBox(szMsg, MB_OKCANCEL) == IDOK)
		{
			if(_db_user_Clear())
			{
				for(int itm = 0; itm < m_usTbl.Cnt(); itm++)
				{
					USER_Item* pItem = m_usTbl.Obj(itm);
					if(pItem)
					{
						_db_user_Insert(pItem);
					}
				}

				m_usTbl.Clear();

				USER_Search usSch;
				if(_db_user_Search(&usSch, &m_usTbl))
				{
					m_oldTbl = m_usTbl;
				}
			}
		}
	}
}

void CMembershipDlg::OnBnClickedBtnOpenFile()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, L"Image File(*.bmp;*.png;*.jpg)|*.bmp;*.png;*.jpg|All Files(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, L"%s", (LPCTSTR)dlg.GetPathName());

		if(!m_imgUser.IsNull())
		{
			m_imgUser.Destroy();
		}
		m_imgUser.Load(szPath);

		InvalidateRect(GetScreen(this, IDC_STATIC_FRAME), TRUE);
	}
}

