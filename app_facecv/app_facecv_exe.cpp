// app_opencv_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "app_facecv_exe.h"

app_facecv_exe::app_facecv_exe()
{
}
app_facecv_exe::~app_facecv_exe()
{
}

/*****************************************************************************
 *  CVFaceDetect
 ****************************************************************************/

#include "CVFaceDetect.h"

Ccpu_FaceDetect* _face_detect_Init(WCHAR* szDir, int thresh)
{
	CVFaceDetect* pface = new CVFaceDetect;
	if(pface)
	{
		if(pface->Init(szDir, thresh))
		{
			return (Ccpu_FaceDetect*)pface;
		}
		else
		{
			pface->Exit();
			delete pface;
			pface = NULL;
		}
	}
	return NULL;
}
void _face_detect_Exit(void* Ptr)
{
	if(Ptr)
	{
		CVFaceDetect* pface = (CVFaceDetect*)Ptr;
		pface->Exit();
		delete pface;
		pface = NULL;
	}
}
void _face_detect_Set(void* Ptr, int dsp_dist, int dsp_size, int thres_nms)
{
	if(Ptr)
	{
		CVFaceDetect* pface = (CVFaceDetect*)Ptr;
		pface->Set(dsp_dist, dsp_size, thres_nms);
	}
}
BOOL _face_detect_Run(void* Ptr, VID_IMGSRC * pVid_img, vector<CRect>& re_ary)
{
	if(Ptr)
	{
		CVFaceDetect* pface = (CVFaceDetect*)Ptr;
		return pface->Run(pVid_img, re_ary);
	}
	return FALSE;
}
