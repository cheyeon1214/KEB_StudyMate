#ifndef _APP_DB_DLG_DLL_H_
#define _APP_DB_DLG_DLL_H_

#pragma once

#ifdef _API_STATIC_LIB_
	#define API_APP_DB_DLG
#else
	#ifdef _APP_DB_DLG_DLL_
		#define API_APP_DB_DLG __declspec(dllexport)
	#else
		#define API_APP_DB_DLG __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/app_db_dlg.lib")
				#pragma message("Automatically linking with ../x64/libd/app_db_dlg.lib")
			#else
				#pragma comment(lib, "../x64/lib/app_db_dlg.lib")
				#pragma message("Automatically linking with ../x64/lib/app_db_dlg.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/app_db_dlg.lib")
				#pragma message("Automatically linking with ../x86/libd/app_db_dlg.lib")
			#else
				#pragma comment(lib, "../x86/lib/app_db_dlg.lib")
				#pragma message("Automatically linking with ../x86/lib/app_db_dlg.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

/*****************************************************************************
 *  Login / Membership
 ****************************************************************************/
API_APP_DB_DLG BOOL _db_Login(CWnd* pWnd);
API_APP_DB_DLG BOOL _db_Member(CWnd* pWnd);
API_APP_DB_DLG BOOL _db_Membership(CWnd* pWnd);

/*****************************************************************************
 *  Manager
 ****************************************************************************/
API_APP_DB_DLG BOOL _db_Manager(CWnd* pWnd);

#endif
