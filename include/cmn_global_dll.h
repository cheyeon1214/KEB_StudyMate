#ifndef _CMN_GLOBAL_DLL_H_
#define _CMN_GLOBAL_DLL_H_

#pragma once

#ifdef _CMN_GLOBAL_DLL_
	#define API_CMN_GLOBAL __declspec(dllexport)
#else
	#define API_CMN_GLOBAL __declspec(dllimport)
	#ifdef _WIN64
		#ifdef _DEBUG
			#pragma comment(lib, "../x64/libd/cmn_global.lib")
			#pragma message("Automatically linking with ../x64/libd/cmn_global.lib")
		#else
			#pragma comment(lib, "../x64/lib/cmn_global.lib")
			#pragma message("Automatically linking with ../x64/lib/cmn_global.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib, "../x86/libd/cmn_global.lib")
			#pragma message("Automatically linking with ../x86/libd/cmn_global.lib")
		#else
			#pragma comment(lib, "../x86/lib/cmn_global.lib")
			#pragma message("Automatically linking with ../x86/lib/cmn_global.lib")
		#endif
	#endif
#endif
#include "../include/common.h"

/*****************************************************************************
 *  mypath
 ****************************************************************************/
API_CMN_GLOBAL void SetServerApp();
API_CMN_GLOBAL WCHAR* _cmn_MyPath();
API_CMN_GLOBAL WCHAR* _cmn_ResPath();
API_CMN_GLOBAL WCHAR* _cmn_ImgPath();
API_CMN_GLOBAL WCHAR* _cmn_IcoPath();
API_CMN_GLOBAL WCHAR* _cmn_CurPath();
API_CMN_GLOBAL WCHAR* _cmn_WavPath();
API_CMN_GLOBAL WCHAR* _cmn_DocPath();
API_CMN_GLOBAL WCHAR* _cmn_SetPath();
API_CMN_GLOBAL WCHAR* _cmn_AppPath();
API_CMN_GLOBAL WCHAR* _cmn_DbPath();
API_CMN_GLOBAL WCHAR* _cmn_MemberPath();

/*****************************************************************************
 *  gui
 ****************************************************************************/
API_CMN_GLOBAL void _cmn_gui_background(CDC* pDC, CDC* pMemDC, CBitmap* pBitmap, CRect *reDst);
API_CMN_GLOBAL void _cmn_gui_stretchframe(CDC* pDC, CDC* pMemDC, CBitmap* pBitmap, CRect *reDst);

/*****************************************************************************
 *  crypt
 ****************************************************************************/
API_CMN_GLOBAL BOOL _cmn_encript_base64(WCHAR* szIn, WCHAR* szOut);
API_CMN_GLOBAL BOOL _cmn_decript_base64(WCHAR* szIn, WCHAR* szOut);

/*****************************************************************************
 *  dtime
 ****************************************************************************/
API_CMN_GLOBAL void _cmn_GetTZone(TZone * ptzi);
API_CMN_GLOBAL BOOL _cmn_IsMyTZone(TZone * ptzi);
API_CMN_GLOBAL void _cmn_CopyTZone(TZone * pdst, TZone * psrc);
API_CMN_GLOBAL BOOL _cmn_CompTZone(TZone * ptz1, TZone * ptz2);// if same return TRUE

API_CMN_GLOBAL DTime _Get_DTime();
API_CMN_GLOBAL LTime _Get_LTime(BOOL bGetDst = FALSE);
API_CMN_GLOBAL void _cmn_dtime_GetCurDTime(DTime * pdt);
API_CMN_GLOBAL void _cmn_dtime_GetCurLTime(LTime * plt, BOOL bGetDst);
API_CMN_GLOBAL void _cmn_dtime_GetCurSTime(WCHAR * pst, BOOL bMillisec = FALSE, BOOL bDst = FALSE);
API_CMN_GLOBAL void _cmn_dtime_DTimetoLTime(const DTime * pdt, TZone * ptzi, LTime * plt);
API_CMN_GLOBAL void _cmn_dtime_LTimetoDTime(const LTime * plt, TZone * ptzi, DTime * pdt);
API_CMN_GLOBAL void _cmn_dtime_DTimetoSDate(const DTime * pdt, TZone * ptzi, WCHAR * psd);
API_CMN_GLOBAL void _cmn_dtime_DTimetoSTime(const DTime * pdt, TZone * ptzi, WCHAR * pst, BOOL bMillisec = FALSE, BOOL bDst = FALSE);
API_CMN_GLOBAL void _cmn_dtime_LTimetoSDate(const LTime * plt, WCHAR * psd);
API_CMN_GLOBAL void _cmn_dtime_LTimetoSTime(const LTime * plt, WCHAR * pst, BOOL bMillisec = FALSE, BOOL bDst = FALSE);
API_CMN_GLOBAL void _cmn_dtime_GetDayZone(const LTime * plt, TZone * ptzi, DTime * pdtDayStart, DTime * pdtDayEnd);

#endif
