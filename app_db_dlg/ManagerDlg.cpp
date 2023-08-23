// ManagerDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ManagerDlg.h"

#include "DefsGridList.h"
#include "ListDemoViewPrint.h"

enum
{
	lst_mb_Insert = 0,

	lst_mb_No,
	lst_mb_Name,
	lst_mb_User,
	lst_mb_Start,
	lst_mb_End,
	lst_mb_Duration,
};

// CManagerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CManagerDlg, CDialog)
CManagerDlg::CManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManagerDlg::IDD, pParent)
{
	m_hIcon = NULL;
	m_hIcon2 = NULL;

	m_bInitial = FALSE;

	m_iScale = 1;
}

CManagerDlg::~CManagerDlg()
{
}

void CManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_MEMBER, m_Member_ctl);

	DDX_Control(pDX, IDC_BTN_DBSEARCH, m_DBSearch_btn);
	DDX_Control(pDX, IDC_CTRL_DATE, m_Date_edt);
	DDX_Control(pDX, IDC_COMB_USER, m_User_cmb);
	DDX_Control(pDX, IDC_EDIT_NOTE, m_Note_edt);

}


BEGIN_MESSAGE_MAP(CManagerDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_DBSEARCH, OnBnClickedBtnDbSearch)
END_MESSAGE_MAP()


// CManagerDlg 메시지 처리기입니다.

BOOL CManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	USER_Search usSch;
	_db_user_Search(&usSch, &m_usTbl);

	if(Initialize(TRUE) == FALSE)
	{
		Initialize(FALSE);
		return FALSE;
	}

	m_bInitial = TRUE;

	LTime lt; lt = _Get_LTime();
	m_Date_edt.SetTime(&lt.stm);
	SetComboUint(m_User_cmb, (UINT)m_usTbl.Cnt());

	SetMemberList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CManagerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	m_bInitial = FALSE;

	Initialize(FALSE);
}

void CManagerDlg::OnSize(UINT nType, int cx, int cy)
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

HBRUSH CManagerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CManagerDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CManagerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	WORD wNotifyCode = HIWORD(wParam);
	if(wNotifyCode == BN_CLICKED)
	{
	}

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CManagerDlg::Initialize(BOOL fInitialize)
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
			SetWindowTheme(GetDlgItem(IDC_CTRL_DATE)->m_hWnd, L"", L"");
		}

		swprintf_s(szPath, L"%s\\btn_dlg.bmp", _cmn_ImgPath());
		m_DBSearch_btn.SetSkinStretch(szPath);
		m_DBSearch_btn.SetTextColor(btn_crtext);
		m_DBSearch_btn.SetTextColorD(btn_crtextd);
		m_DBSearch_btn.SetTextDraw(TRUE, (LOGFONT*)&BTN_FONT);

		WCHAR szContent[MAX_PATH], szBtnList[MAX_PATH];
		swprintf_s(szContent, L"%s\\cmb_content.bmp", _cmn_ImgPath());
		swprintf_s(szBtnList, L"%s\\cmb_btnlist.bmp", _cmn_ImgPath());
		if(m_User_cmb.ReCreate())
		{
			m_User_cmb.SetSkin(szContent, szBtnList);
			m_User_cmb.SetFont((LOGFONT*)&COMB_FONT);
			m_User_cmb.SetTextColor(cmb_crtext);
			m_User_cmb.SetSelColor(cmb_crsel, cmb_crback);
		}



		m_Date_edt.SetFormat(_T(" yyyy-MM-dd"));
		
		Init_ListCtrl();
		Init_ComboBox();

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
void CManagerDlg::Init_ListCtrl()
{
	m_Member_ctl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FLATSB);

	m_Member_ctl.SetCellMargin(1.2);
	CGridRowTraitXP* pRowTrait = new CGridRowTraitXP;
	m_Member_ctl.SetDefaultRowTrait(pRowTrait);
	
	m_Member_ctl.InsertHiddenLabelColumn();

	m_iScale = GetStringColumnScale(&m_Member_ctl, L"0000-00-00 00:00:00", 130);

	G_InsColumn(&m_Member_ctl, lst_mb_No, L"No.", 60*m_iScale, G_Column(G_CTRL_Text), false, LVCFMT_LEFT);
	G_InsColumn(&m_Member_ctl, lst_mb_Name, L"Name", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_Member_ctl, lst_mb_User, L"User", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_Member_ctl, lst_mb_Start, L"Start Time", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_Member_ctl, lst_mb_End, L"End Time", 100*m_iScale, G_Column(G_CTRL_Text), false);
	G_InsColumn(&m_Member_ctl, lst_mb_Duration, L"Duration", 210*m_iScale, G_Column(G_CTRL_Text), false, LVCFMT_LEFT);
}
void CManagerDlg::Init_ComboBox()
{
	int nIndex;
	WCHAR szText[64];
	m_User_cmb.ResetContent();

	nIndex = 0;
	_lang_(IDL_ALL, szText);
	m_User_cmb.AddString(szText);
	m_User_cmb.SetItemData(nIndex, m_usTbl.Cnt());
	for(int itm = 0; itm < m_usTbl.Cnt(); itm++)
	{
		USER_Item* pItem = m_usTbl.Obj(itm);
		if(pItem)
		{
			nIndex++;
			m_User_cmb.AddString(pItem->szUser);
			m_User_cmb.SetItemData(nIndex, itm);
		}
	}
}

void CManagerDlg::MoveCtrl(BOOL bCreate)
{
	if(!m_bInitial && !bCreate)
		return;
	
	if(bCreate)
	{
		GetClientRect(&m_xyDlg);

		GetDlgItem(IDC_LIST_MEMBER)->GetWindowRect(&m_xyList);
		ScreenToClient(&m_xyList);
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
		GetDlgItem(IDC_LIST_MEMBER)->MoveWindow(&reList);



		CRect reMember;
		m_Member_ctl.GetClientRect(&reMember);

		int MbColumnWidth = 20*m_iScale;
		MbColumnWidth += m_Member_ctl.GetColumnWidth(lst_mb_No);
		MbColumnWidth += m_Member_ctl.GetColumnWidth(lst_mb_Name);
		MbColumnWidth += m_Member_ctl.GetColumnWidth(lst_mb_User);
		MbColumnWidth += m_Member_ctl.GetColumnWidth(lst_mb_Start);
		MbColumnWidth += m_Member_ctl.GetColumnWidth(lst_mb_End);

		int nWidth = reMember.Width() - MbColumnWidth;
		if(nWidth < 210*m_iScale) nWidth = 210*m_iScale;

		m_Member_ctl.SetColumnWidth(lst_mb_Duration, nWidth);
	}
}

void CManagerDlg::SetLanguage()
{
	WCHAR szLan[64];
	_lang_(IDL_MEMBER_MANAGER, szLan); SetWindowText(szLan);

	_lang_(IDL_MEMBER_SEARCH, szLan);
	GetDlgItem(IDC_BTN_DBSEARCH)->SetWindowText(szLan);

	_lang_(IDL_DATE, szLan);
	GetDlgItem(IDC_STATIC_DATE)->SetWindowText(szLan);

	_lang_(IDL_USER_ID, szLan);
	GetDlgItem(IDC_STATIC_USER)->SetWindowText(szLan);

	_lang_(IDL_DURATION, szLan);
	GetDlgItem(IDC_STATIC_NOTE)->SetWindowText(szLan);

	LVCOLUMN col; col.mask = LVCF_TEXT;

	_lang_(IDL_NUMBER, szLan);
	col.pszText = szLan;
	m_Member_ctl.SetColumn(lst_mb_No, &col);

	_lang_(IDL_NAME, szLan);
	col.pszText = szLan;
	m_Member_ctl.SetColumn(lst_mb_Name, &col);

	_lang_(IDL_USER_ID, szLan);
	col.pszText = szLan;
	m_Member_ctl.SetColumn(lst_mb_User, &col);

	_lang_(IDL_START_TIME, szLan);
	col.pszText = szLan;
	m_Member_ctl.SetColumn(lst_mb_Start, &col);

	_lang_(IDL_END_TIME, szLan);
	col.pszText = szLan;
	m_Member_ctl.SetColumn(lst_mb_End, &col);

	_lang_(IDL_DURATION, szLan);
	col.pszText = szLan;
	m_Member_ctl.SetColumn(lst_mb_Duration, &col);
}

void CManagerDlg::SetMemberList()
{
	MEMBER_Search mbSch;

	m_Date_edt.GetTime(&(mbSch.ltDate.stm));
	mbSch.ltDate.SetDateFormat();

	int itm = (int)GetComboUint(m_User_cmb);
	if(itm < m_usTbl.Cnt())
	{
		USER_Item* pItem = m_usTbl.Obj(itm);
		if(pItem)
		{
			wcscpy_s(mbSch.szUser, pItem->szUser);
		}
	}

	m_Member_ctl.DeleteAllItems();

	MEMBER_Table mbTbl;
	if(_db_member_Search(&mbSch, &mbTbl))
	{
		LVITEM	lvitem;
		lvitem.mask	= LVIF_TEXT;

		LTime lt;
		WCHAR szNo[64], szStart[64], szEnd[64], szDuration[64];
		for(int itm=0; itm<mbTbl.Cnt(); itm++)
		{
			MEMBER_Item* pItem = mbTbl.Obj(itm);
			if(!pItem) continue;

			swprintf_s(szNo, L"%4d", pItem->No);

			_cmn_dtime_DTimetoLTime(&pItem->dtStart, NULL, &lt);
			swprintf_s(szStart, L"%02d:%02d:%02d", (int)lt.stm.wHour, (int)lt.stm.wMinute, (int)lt.stm.wSecond);

			_cmn_dtime_DTimetoLTime(&pItem->dtEnd, NULL, &lt);
			swprintf_s(szEnd, L"%02d:%02d:%02d", (int)lt.stm.wHour, (int)lt.stm.wMinute, (int)lt.stm.wSecond);

			DTime dtDuration = pItem->GetDuration();
			DTime hh = dtDuration/_DTIME_HOUR; dtDuration -= (hh*_DTIME_HOUR);
			DTime mm = dtDuration/_DTIME_MINUTE; dtDuration -= (mm*_DTIME_MINUTE);
			DTime ss = dtDuration/_DTIME_SECOND; dtDuration -= (ss*_DTIME_SECOND);
			swprintf_s(szDuration, L"%02d:%02d:%02d", (int)hh, (int)mm, (int)ss);

			lvitem.iItem = itm;
			lvitem.iSubItem	= lst_mb_Insert;
			lvitem.pszText = L"Insert";
			m_Member_ctl.InsertItem(&lvitem);
			m_Member_ctl.SetItemText(itm, lst_mb_No, szNo);
			m_Member_ctl.SetItemText(itm, lst_mb_Name, pItem->szName);
			m_Member_ctl.SetItemText(itm, lst_mb_User, pItem->szUser);
			m_Member_ctl.SetItemText(itm, lst_mb_Start, szStart);
			m_Member_ctl.SetItemText(itm, lst_mb_End, szEnd);
			m_Member_ctl.SetItemText(itm, lst_mb_Duration, szDuration);
		}

		DTime dtDuration = mbTbl.GetDuration();
		DTime hh = dtDuration/_DTIME_HOUR; dtDuration -= (hh*_DTIME_HOUR);
		DTime mm = dtDuration/_DTIME_MINUTE; dtDuration -= (mm*_DTIME_MINUTE);
		DTime ss = dtDuration/_DTIME_SECOND; dtDuration -= (ss*_DTIME_SECOND);
		swprintf_s(szDuration, L"%02d:%02d:%02d", (int)hh, (int)mm, (int)ss);
		m_Note_edt.SetWindowText(szDuration);
	}
}

void CManagerDlg::OnBnClickedBtnDbSearch()
{
	SetMemberList();
}
