#pragma once

#include "sqlite3.h"

class DB_User_sql
{
private:
	sqlite3* m_db;

	USER_Item m_usLogin;

private:
	BOOL DBOpen();
	BOOL DBClose();

	int DBCount();
	BOOL DBClear();
	BOOL DBCreate();
	BOOL DBInsert(USER_Item* pItem);
	BOOL DBDelete(USER_Item* pItem);
	BOOL DBUpdate(USER_Item* pItem);
	BOOL DBSearch(USER_Search* sch, USER_Table* tbl);

public:
	BOOL Init();
	void Exit();

	BOOL Clear();
	BOOL Insert(USER_Item* pItem);
	BOOL Delete(USER_Item* pItem);
	BOOL Update(USER_Item* pItem);
	BOOL Search(USER_Search* sch, USER_Table* tbl);

	BOOL Login(BOOL bFlag, USER_Search* sch);
	USER_Item* GetLoginUser();

public:
	DB_User_sql();
	virtual ~DB_User_sql();
};
