// db_device_exe.h
//

#pragma once

#include "DB_User_sql.h"
#include "DB_Member_sql.h"

class db_control_exe
{
private:
	DB_User_sql* m_pUser;
	DB_Member_sql* m_pMember;

public:
	BOOL _Init();
	void _Exit();

	BOOL _user_Clear();
	BOOL _user_Insert(USER_Item* pItem);
	BOOL _user_Delete(USER_Item* pItem);
	BOOL _user_Update(USER_Item* pItem);
	BOOL _user_Search(USER_Search* sch, USER_Table* tbl);
	BOOL _user_Login(BOOL bFlag, USER_Search* sch);
	USER_Item* _user_GetLoginUser();

	BOOL _member_Insert(MEMBER_Item* pItem);
	BOOL _member_Search(MEMBER_Search* sch, MEMBER_Table* tbl);

public:
	db_control_exe();
	~db_control_exe();
};
