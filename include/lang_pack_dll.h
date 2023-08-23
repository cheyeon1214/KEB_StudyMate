#ifndef _LANG_PACK_DLL_H_
#define _LANG_PACK_DLL_H_

#pragma once

#ifdef _API_STATIC_LIB_
	#define API_LANG_PACK
#else
	#ifdef _LANG_PACK_DLL_
		#define API_LANG_PACK __declspec(dllexport)
	#else
		#define API_LANG_PACK __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/lang_pack.lib")
				#pragma message("Automatically linking with ../x64/libd/lang_pack.lib")
			#else
				#pragma comment(lib, "../x64/lib/lang_pack.lib")
				#pragma message("Automatically linking with ../x64/lib/lang_pack.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/lang_pack.lib")
				#pragma message("Automatically linking with ../x86/libd/lang_pack.lib")
			#else
				#pragma comment(lib, "../x86/lib/lang_pack.lib")
				#pragma message("Automatically linking with ../x86/lib/lang_pack.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

enum Lang_ID
{
	ID_English = 0,
	ID_Korean,

	ID_LangMax,
};
enum Lang_IDL
{
	IDL_VIDEO_PROJECT               = 0,
	IDL_SETTING                     ,
	IDL_CONNECTION                  ,
	IDL_DISCONNECT                  ,
	IDL_FMT_USE                     ,
	IDL_CAMERA                      ,
	IDL_SERVER                      ,
	IDL_INDEX                       ,
	IDL_WIDTH                       ,
	IDL_HEIGHT                      ,
	IDL_IP_ADDRESS                  ,
	IDL_PORT                        ,
	IDL_OK                          ,
	IDL_CANCEL                      ,
	IDL_SEND                        ,
	IDL_USER                        ,
	IDL_PASSWORD                    ,
	IDL_MICROPHONE                  ,
	IDL_LOG_VIEW                    ,
	IDL_ALL                         ,
	IDL_NONE                        ,
	IDL_SYSTEM                      ,
	IDL_NETWORK                     ,
	IDL_LOG_SEARCH                  ,
	IDL_NUMBER                      ,
	IDL_DATE                        ,
	IDL_TIME                        ,
	IDL_TYPE                        ,
	IDL_LOG_QUERY                   ,
	IDL_LOG_COUNT                   ,
	IDL_DESCRIPTION                 ,
	IDL_NAME                        ,
	IDL_MEMBERSHIP                  ,
	IDL_JOIN_MEMBERSHIP             ,
	IDL_USER_ID                     ,
	IDL_LOGIN                       ,
	IDL_ID_DUPLICATE                ,
	IDL_INFORMATION                 ,
	IDL_PICTURE                     ,
	IDL_ADD                         ,
	IDL_EDIT                        ,
	IDL_DELETE                      ,
	IDL_SAVE                        ,
	IDL_APPLY                       ,
	IDL_SETUP_MODIFY                ,
	IDL_STUDY_MATE                  ,
	IDL_STUDY_MANAGER               ,
	IDL_OPEN_FILE                   ,
	IDL_MATE                        ,
	IDL_MANAGER                     ,
	IDL_START_TIME                  ,
	IDL_END_TIME                    ,
	IDL_DURATION                    ,
	IDL_SEARCH                      ,
	IDL_MEMBER_SEARCH               ,
	IDL_MEMBER_MANAGER              ,
	IDL_MEMBER_MANAGEMENT           ,

	MAX_Lang_IDL,
};
API_LANG_PACK UINT _lang_Get();
API_LANG_PACK void _lang_Set(UINT uiLang);
API_LANG_PACK BOOL _lang_(UINT nIDL, WCHAR* szLang, int nLen = 64);

#endif
