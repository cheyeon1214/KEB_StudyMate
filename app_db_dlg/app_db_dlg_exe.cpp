// app_db_dlg_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "app_db_dlg_exe.h"

app_db_dlg_exe::app_db_dlg_exe()
{
}
app_db_dlg_exe::~app_db_dlg_exe()
{
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

/*****************************************************************************
 *  Login
 ****************************************************************************/

#include "LoginDlg.h"

BOOL _db_Login(CWnd* pWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CLoginDlg dlg(pWnd);
	if(dlg.DoModal() == IDOK)
	{
		return TRUE;
	}
	return FALSE;
}

#include "MemberDlg.h"

BOOL _db_Member(CWnd* pWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMemberDlg dlg(pWnd);
	if(dlg.DoModal() == IDOK)
	{
		return TRUE;
	}
	return FALSE;
}

#include "MembershipDlg.h"

BOOL _db_Membership(CWnd* pWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CMembershipDlg dlg(pWnd);
	if(dlg.DoModal() == IDOK)
	{
		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************
 *  Manager
 ****************************************************************************/

#include "ManagerDlg.h"

BOOL _db_Manager(CWnd* pWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CManagerDlg dlg(pWnd);
	if(dlg.DoModal() == IDOK)
	{
		return TRUE;
	}
	return FALSE;
}
