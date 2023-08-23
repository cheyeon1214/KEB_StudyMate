
#include "pch.h"
#include "DB_User_sql.h"

#include "DiskObject.h"

DB_User_sql::DB_User_sql()
{
	m_db  = NULL;
}
DB_User_sql::~DB_User_sql()
{
	DBClose();
}

BOOL DB_User_sql::Init()
{
	CFileFind finder;
	if(!finder.FindFile(_cmn_DbPath()))
	{
		CDiskObject objDir;
		objDir.CreateDirectory(_cmn_DbPath());
	}

	WCHAR szDbFile[MAX_PATH];
	swprintf_s(szDbFile, L"%s\\user.db", _cmn_DbPath());
	if(!finder.FindFile(szDbFile))
	{
		if(!DBCreate()) return FALSE;
	}

	return TRUE;
}
void DB_User_sql::Exit()
{
}

BOOL DB_User_sql::Clear()
{
	if(!DBOpen())
	{
		return FALSE;
	}
	if(!DBClear())
	{
		DBClose();
		return FALSE;
	}
	DBClose();

	return TRUE;
}
BOOL DB_User_sql::Insert(USER_Item* pItem)
{
	if(!DBOpen())
	{
		return FALSE;
	}
	if(!DBInsert(pItem))
	{
		DBClose();
		return FALSE;
	}
	DBClose();

	return TRUE;
}
BOOL DB_User_sql::Delete(USER_Item* pItem)
{
	if(!DBOpen())
	{
		return FALSE;
	}
	if(!DBDelete(pItem))
	{
		DBClose();
		return FALSE;
	}
	DBClose();

	return TRUE;
}
BOOL DB_User_sql::Update(USER_Item* pItem)
{
	if(!DBOpen())
	{
		return FALSE;
	}
	if(!DBUpdate(pItem))
	{
		DBClose();
		return FALSE;
	}
	DBClose();

	return TRUE;
}
BOOL DB_User_sql::Search(USER_Search* sch, USER_Table* tbl)
{
	if(!DBOpen())
	{
		return FALSE;
	}
	if(!DBSearch(sch, tbl))
	{
		DBClose();
		return FALSE;
	}
	DBClose();

	return TRUE;
}

BOOL DB_User_sql::Login(BOOL bFlag, USER_Search* sch)
{
	if(bFlag)
	{
		USER_Table usTbl;
		if(Search(sch, &usTbl))
		{
			if(usTbl.Cnt())
			{
				USER_Item* pItem = usTbl.Obj(0);
				if(pItem)
				{
					m_usLogin = *pItem;
					return TRUE;
				}
			}
		}
	}
	else
	{
		m_usLogin.Default();
		return TRUE;
	}
	return FALSE;
}
USER_Item* DB_User_sql::GetLoginUser()
{
	if(m_usLogin.uiLevel)
		return &m_usLogin;
	return NULL;
}

BOOL DB_User_sql::DBOpen()
{
	WCHAR szDbFile[MAX_PATH];
	swprintf_s(szDbFile, L"%s\\user.db", _cmn_DbPath());

	CFileFind finder;
	if(!finder.FindFile(szDbFile)) return FALSE;

	if(sqlite3_open16(szDbFile, &m_db) != SQLITE_OK)
	{
		DBClose();
		return FALSE;
	}
	return TRUE;
}
BOOL DB_User_sql::DBClose()
{
	if(m_db)
	{
		sqlite3_close(m_db);
		m_db = NULL;
	}
	return TRUE;
}

int callback_user(void* arg, int columns, wchar_t** value, wchar_t** name)  
{
	if(columns)
	{
		_ArgList* pArg = (_ArgList*)arg;
		switch(pArg->n_val)
		{
		case lst_count:
			*((int*)pArg->p_val[0]) = _wtoi(value[0]);
			break;
		case lst_search:
			if(columns >= 6)
			{
				USER_Search* sch = (USER_Search*)pArg->p_val[0];
				USER_Table* tbl = (USER_Table*)pArg->p_val[1];

				bool b_ins = false;

				USER_Item Item;
				Item.No = _wtoi(value[0]);
				wcscpy_s(Item.szName, value[1]);
				wcscpy_s(Item.szUser, value[2]);

				WCHAR szCrypt[256];
				wcscpy_s(szCrypt, value[3]);
				if(*szCrypt != L'\0')
				{
					if(_cmn_decript_base64(szCrypt, Item.szPass))
					{
						if(*sch->szPass != L'\0')
						{
							if(wcscmp(Item.szPass, sch->szPass) == 0)
								b_ins = true;
						}
						else b_ins = true;
					}
				}
				wcscpy_s(Item.szNote, value[4]);
				Item.uiLevel = (uint32)_wtoi(value[5]);

				if(b_ins) tbl->Insert(&Item);
			}
			break;
		}
	}
	return 0;
}
int DB_User_sql::DBCount()
{
	if(m_db)
	{
		const WCHAR * query = L"SELECT COUNT(*) FROM User_Table;";

		int count = 0;
		_ArgList arg(this, lst_count, (void*)&count);
		if(sqlite3_exec16(m_db, query, callback_user, (void*)&arg, NULL) != SQLITE_OK)
			count = 0;
		return count;
	}
	return 0;
}
BOOL DB_User_sql::DBClear()
{
	if(!m_db) return FALSE;

	WCHAR query[1024];
	swprintf_s(query, 1024,
		L"DELETE FROM User_Table WHERE userNo > -1"
	);

	if(sqlite3_exec16(m_db, query, NULL, NULL, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
BOOL DB_User_sql::DBCreate()
{
	WCHAR szDbFile[MAX_PATH];
	swprintf_s(szDbFile, L"%s\\user.db", _cmn_DbPath());

	CFileFind finder;
	if(!finder.FindFile(szDbFile))
	{
		sqlite3 *db;
		if(sqlite3_open16(szDbFile, &db) != SQLITE_OK) return FALSE;

		const WCHAR * query =
			L"CREATE TABLE User_Table (userNo INTEGER PRIMARY KEY, szName TEXT, szUser TEXT, szCrypt TEXT, szNote TEXT, uiLevel NUM);";

		bool result = true;
		if(sqlite3_exec16(db, query, NULL, NULL, NULL) != SQLITE_OK)
			result = false;

		sqlite3_close(db);

		if(!result) return FALSE;
	}
	return TRUE;
}
BOOL DB_User_sql::DBInsert(USER_Item* pItem)
{
	if(!m_db) return FALSE;
	
	WCHAR szCrypt[256];
	_cmn_encript_base64(pItem->szPass, szCrypt);

	WCHAR query[1024];
	swprintf_s(query, 1024,
		L"INSERT INTO User_Table (userNo, szName, szUser, szCrypt, szNote, uiLevel) VALUES(NULL, '%s', '%s', '%s', '%s', %d);",
		pItem->szName, pItem->szUser, szCrypt, pItem->szNote, pItem->uiLevel
	);

	if(sqlite3_exec16(m_db, query, NULL, NULL, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
BOOL DB_User_sql::DBDelete(USER_Item* pItem)
{
	if(!m_db) return FALSE;

	WCHAR query[1024];
	swprintf_s(query, 1024,
		L"DELETE FROM User_Table WHERE userNo = %d", pItem->No
	);

	if(sqlite3_exec16(m_db, query, NULL, NULL, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
BOOL DB_User_sql::DBUpdate(USER_Item* pItem)
{
	if(!m_db) return FALSE;

	WCHAR szCrypt[256];
	_cmn_encript_base64(pItem->szPass, szCrypt);

	WCHAR query[1024];
	swprintf_s(query, 1024,
		L"UPDATE User_Table SET szName = '%s', szUser = '%s', szCrypt = '%s', szNote = '%s', uiLevel = %d WHERE userNo = %d",
		pItem->szName, pItem->szUser, szCrypt, pItem->szNote, pItem->uiLevel, pItem->No
	);

	if(sqlite3_exec16(m_db, query, NULL, NULL, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
BOOL DB_User_sql::DBSearch(USER_Search* sch, USER_Table* tbl)
{
	if(!m_db) return FALSE;

	BOOL bWh = FALSE;
	WCHAR szParam[3][64];
	for(int i=0; i<3; i++) *(szParam[i]) = L'\0';
	swprintf_s(szParam[0], L"SELECT * FROM User_Table");

	if(*sch->szUser != L'\0')
	{
		swprintf_s(szParam[1], L" %s szUser = '%s'", bWh ? L"AND" : L"WHERE", sch->szUser);
		bWh = TRUE;
	}
	wcscpy_s(szParam[2], L" ORDER BY User_Table.userNo asc;");

	WCHAR query[1024];
	swprintf_s(query, 1024, L"%s%s%s", szParam[0], szParam[1], szParam[2]);

	tbl->Clear();

	_ArgList arg(this, lst_search, (void*)sch, (void*)tbl);
	if(sqlite3_exec16(m_db, query, callback_user, (void*)&arg, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
