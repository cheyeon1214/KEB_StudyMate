#pragma once

#include "sqlite3.h"

#include "Clock.h"
#include "Stream.h"

class DB_Member_sql
{
private:
	ClockReference * m_pClock;
	MediaStream* m_pDatStream;
	
	LTime m_ltOpen;

	ZCriticalSection m_cslock;

private:
	sqlite3* m_db[2];

private:
	void GetDB_Path(LTime* plt, WCHAR* szPath);

	BOOL DBOpen(int idx, LTime * plt);
	BOOL DBClose(int idx);

	int DBCount(int idx);
	BOOL DBCreate(LTime* plt);
	BOOL DBInsert(MEMBER_Item* pItem);
	BOOL DBSearch(int idx, MEMBER_Search* sch, MEMBER_Table* tbl);

public:
	BOOL Init();
	void Exit();

	BOOL Insert(MEMBER_Item* pItem);
	BOOL Search(MEMBER_Search* sch, MEMBER_Table* tbl);

private:
	friend static void InsertThread(void * pIns);
	void InsertThread();

	// thr end
	BOOL m_bInsert;
	T_Event m_evtInsertEnd;

public:
	DB_Member_sql();
	virtual ~DB_Member_sql();
};
