
#include "pch.h"
#include "DB_Member_sql.h"

#include "DiskObject.h"

DB_Member_sql::DB_Member_sql()
{
	for(int idx=0; idx<2; idx++)
	{
		m_db[idx]  = NULL;
	}

	m_pClock = NULL;
	m_pDatStream = NULL;

	m_bInsert = FALSE;
}
DB_Member_sql::~DB_Member_sql()
{
	for(int idx=0; idx<2; idx++)
	{
		DBClose(idx);
	}
}

void DB_Member_sql::GetDB_Path(LTime* plt, WCHAR* szPath)
{
	swprintf_s(
		szPath, MAX_PATH, L"%s\\%04d-%02d-%02d.db", _cmn_MemberPath(),
		(int)plt->stm.wYear, (int)plt->stm.wMonth, (int)plt->stm.wDay
		);
}

BOOL DB_Member_sql::Init()
{
	CFileFind finder;
	if(!finder.FindFile(_cmn_MemberPath()))
	{
		CDiskObject objDir;
		objDir.CreateDirectory(_cmn_MemberPath());
	}

	bool b_open = false;
	m_ltOpen = _Get_LTime();
	if(DBCreate(&m_ltOpen))
	{
		if(DBOpen(IDX_INS, &m_ltOpen))
			b_open = true;
	}
	if(!b_open)
	{
		WCHAR szDbFile[MAX_PATH];
		GetDB_Path(&m_ltOpen, szDbFile);
		if(finder.FindFile(szDbFile)) DeleteFile(szDbFile);

		return FALSE;
	}

	WCHAR szfind[MAX_PATH];
	swprintf_s(szfind, L"%s\\*.db", _cmn_MemberPath());

	BOOL bWorking = finder.FindFile(szfind);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		WCHAR szDate[64];
		swprintf_s(szDate, L"%s", (LPCTSTR)finder.GetFileTitle());

		int nYear = 0, nMonth = 0, nDay = 0;
		swscanf_s(szDate, L"%04d-%02d-%02d", &nYear, &nMonth, &nDay);

		if(nYear && nMonth && nDay)
		{
			if(nYear < (int)m_ltOpen.stm.wYear)
			{
				if(nYear == (int)(m_ltOpen.stm.wYear - 1))
				{
					if(nMonth <= (int)m_ltOpen.stm.wMonth)
						DeleteFile(finder.GetFilePath());
				}else	DeleteFile(finder.GetFilePath());
			}
		}
	}

	m_pClock = new ClockReference;
	m_pDatStream = new MediaStream(m_pClock, MAX_BUF_SIZE);

	m_bInsert = TRUE;
	m_evtInsertEnd.T_Begin(::InsertThread,this);

	return TRUE;
}
void DB_Member_sql::Exit()
{
	if(m_bInsert)
	{
		m_bInsert = FALSE;
		m_pDatStream->Close();
		m_evtInsertEnd.T_Terminate();

		SAFE_DELETE(m_pClock);
		SAFE_DELETE(m_pDatStream);
	}
}

static void InsertThread(void * pIns)
{
	((DB_Member_sql *) pIns)->InsertThread();
}
void DB_Member_sql::InsertThread()
{
	uint8*	pData;
	int		nSize;
	DTime	dtTime = 0;

	while((pData = (uint8*)m_pDatStream->FetchNext(nSize, dtTime, TRUE)) != END_OF_STREAM)
	{ 
		if(m_bInsert)
		{
			MEMBER_Item* pItem = (MEMBER_Item*)pData;

			LTime lt;
			lt = _Get_LTime();
			if(!m_ltOpen.IsDate(&lt))
			{
				DBClose(IDX_INS);

				m_ltOpen = lt;
				if(!DBCreate(&m_ltOpen))
				{
					m_evtInsertEnd.T_End(TRUE);
					return;
				}
				if(!DBOpen(IDX_INS, &m_ltOpen))
				{
					m_evtInsertEnd.T_End(TRUE);
					return;
				}
			}
			if(!DBInsert(pItem))
			{
			}
		}
		m_pDatStream->Release(TRUE);
	}

	DBClose(IDX_INS);

	m_evtInsertEnd.T_End();
}

BOOL DB_Member_sql::Insert(MEMBER_Item* pItem)
{
	if(m_bInsert)
	{
		ZCsLock csl(&m_cslock);

		MEMBER_Item* pBuf = (MEMBER_Item*)m_pDatStream->Allocate(sizeof(MEMBER_Item), FALSE, FALSE);
		if(pBuf)
		{
			*pBuf = *pItem;
			m_pDatStream->Dispatch(sizeof(MEMBER_Item), 0);
			
			return TRUE;
		}
	}
	return FALSE;
}
BOOL DB_Member_sql::Search(MEMBER_Search* sch, MEMBER_Table* tbl)
{
	if(m_ltOpen.IsDate(&(sch->ltDate)))
	{
		if(!DBSearch(IDX_INS, sch, tbl))
			return FALSE;
	}
	else
	{
		if(!DBOpen(IDX_SCH, &(sch->ltDate)))
			return FALSE;
		if(!DBSearch(IDX_SCH, sch, tbl))
		{
			DBClose(IDX_SCH);
			return FALSE;
		}
		DBClose(IDX_SCH);
	}
	return TRUE;
}

BOOL DB_Member_sql::DBOpen(int idx, LTime * plt)
{
	WCHAR szDbFile[MAX_PATH];
	GetDB_Path(plt, szDbFile);

	CFileFind finder;
	if(!finder.FindFile(szDbFile)) return FALSE;

	if(sqlite3_open16(szDbFile, &m_db[idx]) != SQLITE_OK)
	{
		DBClose(idx);
		return FALSE;
	}
	return TRUE;
}
BOOL DB_Member_sql::DBClose(int idx)
{
	if(m_db[idx])
	{
		sqlite3_close(m_db[idx]);
		m_db[idx] = NULL;
	}
	return TRUE;
}

int callback_member(void* arg, int columns, wchar_t** value, wchar_t** name)  
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
			if(columns >= 5)
			{
				MEMBER_Search* sch = (MEMBER_Search*)pArg->p_val[0];
				MEMBER_Table* tbl = (MEMBER_Table*)pArg->p_val[1];

				MEMBER_Item mbItem;
				mbItem.No = _wtoi(value[0]);
				wcscpy_s(mbItem.szName, value[1]);
				wcscpy_s(mbItem.szUser, value[2]);
				mbItem.dtStart = _wtoi64(value[3]);
				mbItem.dtEnd = _wtoi64(value[4]);

				tbl->Insert(&mbItem);
			}
			break;
		}
	}
	return 0;
}
int DB_Member_sql::DBCount(int idx)
{
	if(m_db[idx])
	{
		const WCHAR * query = L"SELECT COUNT(*) FROM Member_Table;";

		int count = 0;
		_ArgList arg(this, lst_count, (void*)&count);
		if(sqlite3_exec16(m_db[idx], query, callback_member, (void*)&arg, NULL) != SQLITE_OK)
			count = 0;
		return count;
	}
	return 0;
}
BOOL DB_Member_sql::DBCreate(LTime * plt)
{
	WCHAR szDbFile[MAX_PATH];
	GetDB_Path(plt, szDbFile);

	CFileFind finder;
	if(!finder.FindFile(szDbFile))
	{
		sqlite3 *db;
		if(sqlite3_open16(szDbFile, &db) != SQLITE_OK) return FALSE;

		const WCHAR * query =
			L"CREATE TABLE Member_Table (memberNo INTEGER PRIMARY KEY, szName TEXT, szUser TEXT, dtStart NUM, dtEnd NUM);";

		bool result = true;
		if(sqlite3_exec16(db, query, NULL, NULL, NULL) != SQLITE_OK)
			result = false;

		sqlite3_close(db);

		if(!result) return FALSE;
	}

	LTime ltDel;
	ltDel = *plt;
	ltDel.stm.wYear -= 1;
	GetDB_Path(&ltDel, szDbFile);
	if(finder.FindFile(szDbFile)) DeleteFile(szDbFile);

	return TRUE;
}
BOOL DB_Member_sql::DBInsert(MEMBER_Item* pItem)
{
	if(m_db[IDX_INS] == NULL) 
		return FALSE;

	WCHAR query[1024];
	swprintf_s(query, 1024,
		L"INSERT INTO Member_Table (memberNo, szName, szUser, dtStart, dtEnd) VALUES(NULL, '%s', '%s', %I64d, %I64d);",
		pItem->szName, pItem->szUser, pItem->dtStart, pItem->dtEnd
	);

	if(sqlite3_exec16(m_db[IDX_INS], query, NULL, NULL, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
BOOL DB_Member_sql::DBSearch(int idx, MEMBER_Search* sch, MEMBER_Table* tbl)
{
	if(m_db[idx] == NULL)
		return FALSE;

	BOOL bWh = FALSE;
	WCHAR szParam[3][64];
	for(int i=0; i<3; i++) *(szParam[i]) = L'\0';
	swprintf_s(szParam[0], L"SELECT * FROM Member_Table");

	if(*sch->szUser != L'\0')
	{
		swprintf_s(szParam[1], L" %s szUser = '%s'", bWh ? L"AND" : L"WHERE", sch->szUser);
		bWh = TRUE;
	}
	wcscpy_s(szParam[2], L" ORDER BY Member_Table.memberNo asc;");

	WCHAR query[1024];
	swprintf_s(query, 1024, L"%s%s%s", szParam[0], szParam[1], szParam[2]);

	tbl->Clear();

	_ArgList arg(this, lst_search, (void*)sch, (void*)tbl);
	if(sqlite3_exec16(m_db[idx], query, callback_member, (void*)&arg, NULL) != SQLITE_OK)
		return FALSE;
	return TRUE;
}
