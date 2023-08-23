#ifndef _APP_FACECV_DLL_H_
#define _APP_FACECV_DLL_H_

#pragma once

#ifdef _API_STATIC_LIB_
	#define API_APP_FACECV
#else
	#ifdef _APP_FACECV_DLL_
		#define API_APP_FACECV __declspec(dllexport)
	#else
		#define API_APP_FACECV __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/app_facecv.lib")
				#pragma message("Automatically linking with ../x64/libd/app_facecv.lib")
			#else
				#pragma comment(lib, "../x64/lib/app_facecv.lib")
				#pragma message("Automatically linking with ../x64/lib/app_facecv.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/app_facecv.lib")
				#pragma message("Automatically linking with ../x86/libd/app_facecv.lib")
			#else
				#pragma comment(lib, "../x86/lib/app_facecv.lib")
				#pragma message("Automatically linking with ../x86/lib/app_facecv.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

#define Ccpu_FaceDetect void

/*****************************************************************************
 *  CVFaceDetect
 ****************************************************************************/
API_APP_FACECV Ccpu_FaceDetect* _face_detect_Init(WCHAR* szDir, int thresh);
API_APP_FACECV void _face_detect_Exit(void* Ptr);
API_APP_FACECV void _face_detect_Set(void* Ptr, int dsp_dist, int dsp_size, int thres_nms);
API_APP_FACECV BOOL _face_detect_Run(void* Ptr, VID_IMGSRC * pVid_img, vector<CRect>& re_ary);

#endif
