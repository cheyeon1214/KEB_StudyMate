// app_capture_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "app_capture_exe.h"

app_capture_exe::app_capture_exe()
{
	m_pMicDrv = NULL;
	m_pCamDrv = NULL;
}
app_capture_exe::~app_capture_exe()
{
}

BOOL app_capture_exe::_mic_Init(uint samples, uint databit)
{
	if(!m_pMicDrv)
	{
		m_pMicDrv = new AinputControl;
		if(m_pMicDrv)
		{
			if(!m_pMicDrv->Init(samples, databit))
			{
				m_pMicDrv->Exit();
				delete m_pMicDrv;
				m_pMicDrv = NULL;
				return FALSE;
			}
		}
	}
	return TRUE;
}
void app_capture_exe::_mic_Exit()
{
	if(m_pMicDrv)
	{
		m_pMicDrv->Exit();
		delete m_pMicDrv;
		m_pMicDrv = NULL;
	}
}
BOOL app_capture_exe::_mic_IsActive()
{
	if(m_pMicDrv)
	{
		return TRUE;
	}
	return FALSE;
}
void app_capture_exe::_mic_SetCallback(void* Ptr, AInCallback cb)
{
	if(m_pMicDrv)
	{
		m_pMicDrv->SetCallback(Ptr, cb);
	}
}

BOOL app_capture_exe::_cam_Init(int index, int hsize, int vsize)
{
	if(!m_pCamDrv)
	{
		m_pCamDrv = new VinputControl;
		if(m_pCamDrv)
		{
			if(!m_pCamDrv->Init(index, hsize, vsize))
			{
				m_pCamDrv->Exit();
				delete m_pCamDrv;
				m_pCamDrv = NULL;
				return FALSE;
			}
		}
	}
	return TRUE;
}
void app_capture_exe::_cam_Exit()
{
	if(m_pCamDrv)
	{
		m_pCamDrv->Exit();
		delete m_pCamDrv;
		m_pCamDrv = NULL;
	}
}
BOOL app_capture_exe::_cam_IsActive()
{
	if(m_pCamDrv)
	{
		return TRUE;
	}
	return FALSE;
}
void app_capture_exe::_cam_SetCallback(void* Ptr, VInCallback cb)
{
	if(m_pCamDrv)
	{
		m_pCamDrv->SetCallback(Ptr, cb);
	}
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

app_capture_exe g_capture;

BOOL _mic_drv_Init(uint samples, uint databit)
{
	return g_capture._mic_Init(samples, databit);
}
void _mic_drv_Exit()
{
	g_capture._mic_Exit();
}
BOOL _mic_drv_IsActive()
{
	return g_capture._mic_IsActive();
}
void _mic_drv_SetCallback(void* Ptr, AInCallback cb)
{
	g_capture._mic_SetCallback(Ptr, cb);
}

BOOL _cam_drv_Init(int index, int hsize, int vsize)
{
	return g_capture._cam_Init(index, hsize, vsize);
}
void _cam_drv_Exit()
{
	g_capture._cam_Exit();
}
BOOL _cam_drv_IsActive()
{
	return g_capture._cam_IsActive();
}
void _cam_drv_SetCallback(void* Ptr, VInCallback cb)
{
	g_capture._cam_SetCallback(Ptr, cb);
}

BOOL _cam_drv_GetPicture(vector<uint8>& buf)
{
	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	if(!cap.isOpened())
		return FALSE;

	cv::Mat bgrImg;
	if(cap.read(bgrImg))
	{
		if(cv::imencode(".bmp", bgrImg, buf)) return TRUE;
	}
	return FALSE;
}
