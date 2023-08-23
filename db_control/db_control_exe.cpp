// db_control_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "db_control_exe.h"

db_control_exe::db_control_exe()
{
	m_pUser = NULL;
	m_pMember = NULL;
}
db_control_exe::~db_control_exe()
{
}

BOOL db_control_exe::_Init()
{
	if(m_pUser == NULL)
	{
		m_pUser = new DB_User_sql;
		if(!m_pUser->Init())
		{
			m_pUser->Exit();
			delete m_pUser;
			m_pUser = NULL;
			return FALSE;
		}
	}
	if(m_pMember == NULL)
	{
		m_pMember = new DB_Member_sql;
		if(!m_pMember->Init())
		{
			m_pMember->Exit();
			delete m_pMember;
			m_pMember = NULL;
			return FALSE;
		}
	}
	return TRUE;
}
void db_control_exe::_Exit()
{
	if(m_pUser)
	{
		m_pUser->Exit();
		delete m_pUser;
		m_pUser = NULL;
	}
	if(m_pMember)
	{
		m_pMember->Exit();
		delete m_pMember;
		m_pMember = NULL;
	}
}

BOOL db_control_exe::_user_Clear()
{
	if(m_pUser)
	{
		return m_pUser->Clear();
	}
	return FALSE;
}
BOOL db_control_exe::_user_Insert(USER_Item* pItem)
{
	if(m_pUser)
	{
		return m_pUser->Insert(pItem);
	}
	return FALSE;
}
BOOL db_control_exe::_user_Delete(USER_Item* pItem)
{
	if(m_pUser)
	{
		return m_pUser->Delete(pItem);
	}
	return FALSE;
}
BOOL db_control_exe::_user_Update(USER_Item* pItem)
{
	if(m_pUser)
	{
		return m_pUser->Update(pItem);
	}
	return FALSE;
}
BOOL db_control_exe::_user_Search(USER_Search* sch, USER_Table* tbl)
{
	if(m_pUser)
	{
		return m_pUser->Search(sch, tbl);
	}
	return FALSE;
}
BOOL db_control_exe::_user_Login(BOOL bFlag, USER_Search* sch)
{
	if(m_pUser)
	{
		return m_pUser->Login(bFlag, sch);
	}
	return FALSE;
}
USER_Item* db_control_exe::_user_GetLoginUser()
{
	if(m_pUser)
	{
		return m_pUser->GetLoginUser();
	}
	return NULL;
}

BOOL db_control_exe::_member_Insert(MEMBER_Item* pItem)
{
	if(m_pMember)
	{
		return m_pMember->Insert(pItem);
	}
	return FALSE;
}
BOOL db_control_exe::_member_Search(MEMBER_Search* sch, MEMBER_Table* tbl)
{
	if(m_pMember)
	{
		return m_pMember->Search(sch, tbl);
	}
	return FALSE;
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

db_control_exe g_db_control;

BOOL _db_control_Init()
{
	return g_db_control._Init();
}
void _db_control_Exit()
{
	g_db_control._Exit();
}

BOOL _db_user_Clear()
{
	return g_db_control._user_Clear();
}
BOOL _db_user_Insert(USER_Item* pItem)
{
	return g_db_control._user_Insert(pItem);
}
BOOL _db_user_Delete(USER_Item* pItem)
{
	return g_db_control._user_Delete(pItem);
}
BOOL _db_user_Update(USER_Item* pItem)
{
	return g_db_control._user_Update(pItem);
}
BOOL _db_user_Search(USER_Search* sch, USER_Table* tbl)
{
	return g_db_control._user_Search(sch, tbl);
}
BOOL _db_user_Login(BOOL bFlag, USER_Search* sch)
{
	return g_db_control._user_Login(bFlag, sch);
}
USER_Item* _db_user_GetLoginUser()
{
	return g_db_control._user_GetLoginUser();
}

BOOL _db_member_Insert(MEMBER_Item* pItem)
{
	return g_db_control._member_Insert(pItem);
}
BOOL _db_member_Search(MEMBER_Search* sch, MEMBER_Table* tbl)
{
	return g_db_control._member_Search(sch, tbl);
}
