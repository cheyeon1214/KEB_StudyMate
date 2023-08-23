#ifndef _APP_CAPTURE_DLL_H_
#define _APP_CAPTURE_DLL_H_

#pragma once

#ifdef _API_STATIC_LIB_
	#define API_APP_CAPTURE
#else
	#ifdef _APP_CAPTURE_DLL_
		#define API_APP_CAPTURE __declspec(dllexport)
	#else
		#define API_APP_CAPTURE __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/app_capture.lib")
				#pragma message("Automatically linking with ../x64/libd/app_capture.lib")
			#else
				#pragma comment(lib, "../x64/lib/app_capture.lib")
				#pragma message("Automatically linking with ../x64/lib/app_capture.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/app_capture.lib")
				#pragma message("Automatically linking with ../x86/libd/app_capture.lib")
			#else
				#pragma comment(lib, "../x86/lib/app_capture.lib")
				#pragma message("Automatically linking with ../x86/lib/app_capture.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

typedef void(*AInCallback)(void* Ptr, AUD_SNDSRC* pAud_snd);
typedef void(*VInCallback)(void* Ptr, VID_IMGSRC* pVid_img);

API_APP_CAPTURE BOOL _mic_drv_Init(uint samples, uint databit);
API_APP_CAPTURE void _mic_drv_Exit();
API_APP_CAPTURE BOOL _mic_drv_IsActive();
API_APP_CAPTURE void _mic_drv_SetCallback(void* Ptr, AInCallback cb);

API_APP_CAPTURE BOOL _cam_drv_Init(int index, int hsize, int vsize);
API_APP_CAPTURE void _cam_drv_Exit();
API_APP_CAPTURE BOOL _cam_drv_IsActive();
API_APP_CAPTURE void _cam_drv_SetCallback(void* Ptr, VInCallback cb);
API_APP_CAPTURE BOOL _cam_drv_GetPicture(vector<uint8>& buf);

#endif
