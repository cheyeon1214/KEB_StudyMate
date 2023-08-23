#ifndef _DB_CONTROL_DLL_H_
#define _DB_CONTROL_DLL_H_

#pragma once

#ifdef _API_STATIC_LIB_
	#define API_DB_CONTROL
#else
	#ifdef _DB_CONTROL_DLL_
		#define API_DB_CONTROL __declspec(dllexport)
	#else
		#define API_DB_CONTROL __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/db_control.lib")
				#pragma message("Automatically linking with ../x64/libd/db_control.lib")
			#else
				#pragma comment(lib, "../x64/lib/db_control.lib")
				#pragma message("Automatically linking with ../x64/lib/db_control.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/db_control.lib")
				#pragma message("Automatically linking with ../x86/libd/db_control.lib")
			#else
				#pragma comment(lib, "../x86/lib/db_control.lib")
				#pragma message("Automatically linking with ../x86/lib/db_control.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

/*****************************************************************************
 *
 *  Login User
 *	uiLevel (0:logoff 1:login)
 *
 ****************************************************************************/

struct USER_Item
{
	int		No;
	WCHAR	szName[64];
	WCHAR	szUser[64];
	WCHAR	szPass[256];
	WCHAR	szNote[256];
	uint32	uiLevel;

	USER_Item()
	{
		Default();
	}
	inline USER_Item& operator = (USER_Item& other)
	{
		this->No = other.No;
		wcscpy_s(this->szName, other.szName);
		wcscpy_s(this->szUser, other.szUser);
		wcscpy_s(this->szPass, other.szPass);
		wcscpy_s(this->szNote, other.szNote);
		this->uiLevel = other.uiLevel;
		return *this;
	}
	inline BOOL operator == (USER_Item& other)
	{
		if(this->No != other.No)
			return FALSE;
		if(wcscmp(this->szName, other.szName) != 0)
			return FALSE;
		if(wcscmp(this->szUser, other.szUser) != 0)
			return FALSE;
		if(wcscmp(this->szPass, other.szPass) != 0)
			return FALSE;
		if(wcscmp(this->szNote, other.szNote) != 0)
			return FALSE;
		if(this->uiLevel != other.uiLevel)
			return FALSE;
		return TRUE;
	}
	inline BOOL operator != (USER_Item& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
	inline void Default()
	{
		this->No = 0;
		*this->szName = L'\0';
		*this->szUser = L'\0';
		*this->szPass = L'\0';
		*this->szNote = L'\0';
		this->uiLevel = 0;
	}
};
struct USER_Table
{
	vector<USER_Item*> vec_User;

	USER_Table()
	{
		Default();
	}
	~USER_Table()
	{
		Clear();
	}
	inline int Cnt()
	{
		return (int)vec_User.size();
	}
	inline USER_Item* Obj(int itm)
	{
		if(itm < (int)vec_User.size())
		{
			USER_Item* pItem = vec_User[itm];
			return pItem;
		}
		return NULL;
	}
	inline USER_Table& operator = (USER_Table& other)
	{
		Clear();

		for(int itm=0; itm<other.Cnt(); itm++)
		{
			Insert(other.Obj(itm));
		}
		return *this;
	}
	inline BOOL operator == (USER_Table& other)
	{
		if((int)vec_User.size() != other.Cnt())
			return FALSE;
		for(int itm=0; itm<(int)vec_User.size(); itm++)
		{
			USER_Item* pItem = vec_User[itm];
			if(pItem && other.Obj(itm))
			{
				if(*pItem != *(other.Obj(itm)))
					return FALSE;
			}
			else return FALSE;
		}
		return TRUE;
	}
	inline BOOL operator != (USER_Table& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
	inline void Default()
	{
		Clear();
	}
	inline void Clear()
	{
		for(auto pItem : vec_User)
		{
			SAFE_DELETE(pItem);
		}
		vec_User.clear();
	}
	inline void Insert(USER_Item* pItem)
	{
		USER_Item* pNew = new USER_Item;
		*pNew = *pItem;
		vec_User.push_back(pNew);
	}
	inline void Delete(int i)
	{
		if(i < (int)vec_User.size())
		{
			USER_Item* pItem = vec_User[i];
			SAFE_DELETE(pItem);
			vec_User.erase(vec_User.begin() + i);
		}
	}
};
struct USER_Search
{
	WCHAR szUser[64];
	WCHAR szPass[256];

	USER_Search()
	{
		Default();
	}
	inline USER_Search& operator = (USER_Search& other)
	{
		wcscpy_s(this->szUser, other.szUser);
		wcscpy_s(this->szPass, other.szPass);
		return *this;
	}
	inline BOOL operator == (USER_Search& other)
	{
		if(wcscmp(this->szUser, other.szUser) != 0)
			return FALSE;
		if(wcscmp(this->szPass, other.szPass) != 0)
			return FALSE;
		return TRUE;
	}
	inline BOOL operator != (USER_Search& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
	inline void Default()
	{
		*this->szUser = L'\0';
		*this->szPass = L'\0';
	}
};


/*****************************************************************************
 *
 *  MEMBER
 *
 ****************************************************************************/

struct MEMBER_Item
{
	int		No;
	WCHAR	szName[64];
	WCHAR	szUser[64];
	DTime	dtStart;
	DTime	dtEnd;

	MEMBER_Item()
	{
		Default();
	}
	inline MEMBER_Item& operator = (MEMBER_Item& other)
	{
		this->No = other.No;
		wcscpy_s(this->szName, other.szName);
		wcscpy_s(this->szUser, other.szUser);
		this->dtStart = other.dtStart;
		this->dtEnd = other.dtEnd;
		return *this;
	}
	inline MEMBER_Item& operator = (USER_Item& other)
	{
		wcscpy_s(this->szName, other.szName);
		wcscpy_s(this->szUser, other.szUser);
		return *this;
	}
	inline BOOL operator == (MEMBER_Item& other)
	{
		if(this->No != other.No)
			return FALSE;
		if(wcscmp(this->szName, other.szName) != 0)
			return FALSE;
		if(wcscmp(this->szUser, other.szUser) != 0)
			return FALSE;
		if(this->dtStart != other.dtStart)
			return FALSE;
		if(this->dtEnd != other.dtEnd)
			return FALSE;
		return TRUE;
	}
	inline BOOL operator != (MEMBER_Item& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
	inline void Default()
	{
		this->No = 0;
		*this->szName = L'\0';
		*this->szUser = L'\0';
		this->dtStart = 0;
		this->dtEnd = 0;
	}
	inline DTime GetDuration()
	{
		if(!this->dtStart) return 0;
		if(!this->dtEnd) return 0;
		DTime dtDuration = 0;
		if(this->dtStart < this->dtEnd)
		{
			dtDuration = this->dtEnd - this->dtStart + 1;
		}
		return dtDuration;
	}
};
struct MEMBER_Table
{
	vector<MEMBER_Item*> vec_Member;

	MEMBER_Table()
	{
		Default();
	}
	~MEMBER_Table()
	{
		Clear();
	}
	inline int Cnt()
	{
		return (int)vec_Member.size();
	}
	inline MEMBER_Item* Obj(int itm)
	{
		if(itm < (int)vec_Member.size())
		{
			MEMBER_Item* pItem = vec_Member[itm];
			return pItem;
		}
		return NULL;
	}
	inline MEMBER_Table& operator = (MEMBER_Table& other)
	{
		Clear();

		for(int itm=0; itm<other.Cnt(); itm++)
		{
			Insert(other.Obj(itm));
		}
		return *this;
	}
	inline BOOL operator == (MEMBER_Table& other)
	{
		if((int)vec_Member.size() != other.Cnt())
			return FALSE;
		for(int itm=0; itm<(int)vec_Member.size(); itm++)
		{
			MEMBER_Item* pItem = vec_Member[itm];
			if(pItem && other.Obj(itm))
			{
				if(*pItem != *(other.Obj(itm)))
					return FALSE;
			}
			else return FALSE;
		}
		return TRUE;
	}
	inline BOOL operator != (MEMBER_Table& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
	inline void Default()
	{
		Clear();
	}
	inline void Clear()
	{
		for(auto pItem : vec_Member)
		{
			SAFE_DELETE(pItem);
		}
		vec_Member.clear();
	}
	inline void Insert(MEMBER_Item* pItem)
	{
		MEMBER_Item* pNew = new MEMBER_Item;
		*pNew = *pItem;
		vec_Member.push_back(pNew);
	}
	inline DTime GetDuration()
	{
		DTime dtDuration = 0;
		for(auto pItem : vec_Member)
		{
			dtDuration += pItem->GetDuration();
		}
		return dtDuration;
	}
};
struct MEMBER_Search
{
	LTime ltDate;
	WCHAR szUser[64];

	MEMBER_Search()
	{
		Default();
	}
	inline MEMBER_Search& operator = (MEMBER_Search& other)
	{
		this->ltDate = other.ltDate;
		wcscpy_s(this->szUser, other.szUser);
		return *this;
	}
	inline BOOL operator == (MEMBER_Search& other)
	{
		if(this->ltDate != other.ltDate)
			return FALSE;
		if(wcscmp(this->szUser, other.szUser) != 0)
			return FALSE;
		return TRUE;
	}
	inline BOOL operator != (MEMBER_Search& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
	inline void Default()
	{
		this->ltDate.Default();
		*this->szUser = L'\0';
	}
};

/*****************************************************************************
 *  _db_control_
 ****************************************************************************/
API_DB_CONTROL BOOL _db_control_Init();
API_DB_CONTROL void _db_control_Exit();

API_DB_CONTROL BOOL _db_user_Clear();
API_DB_CONTROL BOOL _db_user_Insert(USER_Item* pItem);
API_DB_CONTROL BOOL _db_user_Delete(USER_Item* pItem);
API_DB_CONTROL BOOL _db_user_Update(USER_Item* pItem);
API_DB_CONTROL BOOL _db_user_Search(USER_Search* sch, USER_Table* tbl);
API_DB_CONTROL BOOL _db_user_Login(BOOL bFlag, USER_Search* sch = NULL);
API_DB_CONTROL USER_Item* _db_user_GetLoginUser();

API_DB_CONTROL BOOL _db_member_Insert(MEMBER_Item* pItem);
API_DB_CONTROL BOOL _db_member_Search(MEMBER_Search* sch, MEMBER_Table* tbl);

#endif
