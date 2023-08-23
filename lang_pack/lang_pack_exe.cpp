// lang_pack_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "lang_pack_exe.h"

lang_pack_exe::lang_pack_exe()
{
	usLangID = 0x409;

	m_Lang = ID_English;

	m_LangID[IDL_VIDEO_PROJECT               ] = IDS_VIDEO_PROJECT               ;
	m_LangID[IDL_SETTING                     ] = IDS_SETTING                     ;
	m_LangID[IDL_CONNECTION                  ] = IDS_CONNECTION                  ;
	m_LangID[IDL_DISCONNECT                  ] = IDS_DISCONNECT                  ;
	m_LangID[IDL_FMT_USE                     ] = IDS_FMT_USE                     ;
	m_LangID[IDL_CAMERA                      ] = IDS_CAMERA                      ;
	m_LangID[IDL_SERVER                      ] = IDS_SERVER                      ;
	m_LangID[IDL_INDEX                       ] = IDS_INDEX                       ;
	m_LangID[IDL_WIDTH                       ] = IDS_WIDTH                       ;
	m_LangID[IDL_HEIGHT                      ] = IDS_HEIGHT                      ;
	m_LangID[IDL_IP_ADDRESS                  ] = IDS_IP_ADDRESS                  ;
	m_LangID[IDL_PORT                        ] = IDS_PORT                        ;
	m_LangID[IDL_OK                          ] = IDS_OK                          ;
	m_LangID[IDL_CANCEL                      ] = IDS_CANCEL                      ;
	m_LangID[IDL_SEND                        ] = IDS_SEND                        ;
	m_LangID[IDL_USER                        ] = IDS_USER                        ;
	m_LangID[IDL_PASSWORD                    ] = IDS_PASSWORD                    ;
	m_LangID[IDL_MICROPHONE                  ] = IDS_MICROPHONE                  ;
	m_LangID[IDL_LOG_VIEW                    ] = IDS_LOG_VIEW                    ;
	m_LangID[IDL_ALL                         ] = IDS_ALL                         ;
	m_LangID[IDL_NONE                        ] = IDS_NONE                        ;
	m_LangID[IDL_SYSTEM                      ] = IDS_SYSTEM                      ;
	m_LangID[IDL_NETWORK                     ] = IDS_NETWORK                     ;
	m_LangID[IDL_LOG_SEARCH                  ] = IDS_LOG_SEARCH                  ;
	m_LangID[IDL_NUMBER                      ] = IDS_NUMBER                      ;
	m_LangID[IDL_DATE                        ] = IDS_DATE                        ;
	m_LangID[IDL_TIME                        ] = IDS_TIME                        ;
	m_LangID[IDL_TYPE                        ] = IDS_TYPE                        ;
	m_LangID[IDL_LOG_QUERY                   ] = IDS_LOG_QUERY                   ;
	m_LangID[IDL_LOG_COUNT                   ] = IDS_LOG_COUNT                   ;
	m_LangID[IDL_DESCRIPTION                 ] = IDS_DESCRIPTION                 ;
	m_LangID[IDL_NAME                        ] = IDS_NAME                        ;
	m_LangID[IDL_MEMBERSHIP                  ] = IDS_MEMBERSHIP                  ;
	m_LangID[IDL_JOIN_MEMBERSHIP             ] = IDS_JOIN_MEMBERSHIP             ;
	m_LangID[IDL_USER_ID                     ] = IDS_USER_ID                     ;
	m_LangID[IDL_LOGIN                       ] = IDS_LOGIN                       ;
	m_LangID[IDL_ID_DUPLICATE                ] = IDS_ID_DUPLICATE                ;
	m_LangID[IDL_INFORMATION                 ] = IDS_INFORMATION                 ;
	m_LangID[IDL_PICTURE                     ] = IDS_PICTURE                     ;
	m_LangID[IDL_ADD                         ] = IDS_ADD                         ;
	m_LangID[IDL_EDIT                        ] = IDS_EDIT                        ;
	m_LangID[IDL_DELETE                      ] = IDS_DELETE                      ;
	m_LangID[IDL_SAVE                        ] = IDS_SAVE                        ;
	m_LangID[IDL_APPLY                       ] = IDS_APPLY                       ;
	m_LangID[IDL_SETUP_MODIFY                ] = IDS_SETUP_MODIFY                ;
	m_LangID[IDL_STUDY_MATE                  ] = IDS_STUDY_MATE                  ;
	m_LangID[IDL_STUDY_MANAGER               ] = IDS_STUDY_MANAGER               ;
	m_LangID[IDL_OPEN_FILE                   ] = IDS_OPEN_FILE                   ;
	m_LangID[IDL_MATE                        ] = IDS_MATE                        ;
	m_LangID[IDL_MANAGER                     ] = IDS_MANAGER                     ;
	m_LangID[IDL_START_TIME                  ] = IDS_START_TIME                  ;
	m_LangID[IDL_END_TIME                    ] = IDS_END_TIME                    ;
	m_LangID[IDL_DURATION                    ] = IDS_DURATION                    ;
	m_LangID[IDL_SEARCH                      ] = IDS_SEARCH                      ;
	m_LangID[IDL_MEMBER_SEARCH               ] = IDS_MEMBER_SEARCH               ;
	m_LangID[IDL_MEMBER_MANAGER              ] = IDS_MEMBER_MANAGER              ;
	m_LangID[IDL_MEMBER_MANAGEMENT           ] = IDS_MEMBER_MANAGEMENT           ;
}
lang_pack_exe::~lang_pack_exe()
{
}

UINT lang_pack_exe::GetIDS(int nIDL)
{
	return m_LangID[nIDL];
}

UINT lang_pack_exe::_Get()
{
	return m_Lang;
}
void lang_pack_exe::_Set(UINT uiLang)
{
	m_Lang = uiLang;
	switch(uiLang)
	{
	case ID_English:
		usLangID = 0x409;
		break;
	case ID_Korean:
		usLangID = 0x412;
		break;
	default:
		usLangID = 0x409;
		break;
	}
}
BOOL lang_pack_exe::_Get(UINT nIDL, WCHAR* szLang, int nLen)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString str;
	if(!str.LoadString(AfxGetInstanceHandle(), GetIDS(nIDL), usLangID))
		if(!str.LoadString(AfxGetInstanceHandle(), GetIDS(nIDL), 0x409))
			return FALSE;
	swprintf_s(szLang, nLen, L"%s", (LPCTSTR)str);
	return TRUE;
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

lang_pack_exe g_lang;

UINT _lang_Get()
{
	return g_lang._Get();
}
void _lang_Set(UINT uiLang)
{
	g_lang._Set(uiLang);
}
BOOL _lang_(UINT nIDL, WCHAR* szLang, int nLen)
{
	return g_lang._Get(nIDL, szLang, nLen);
}
