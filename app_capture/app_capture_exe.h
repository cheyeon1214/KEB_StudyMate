// app_capture_exe.h
//

#pragma once

#include "AinputControl.h"
#include "VinputControl.h"

class app_capture_exe
{
private:
	AinputControl* m_pMicDrv;
	VinputControl* m_pCamDrv;

public:
	BOOL _mic_Init(uint samples, uint databit);
	void _mic_Exit();
	BOOL _mic_IsActive();
	void _mic_SetCallback(void* Ptr, AInCallback cb);

	BOOL _cam_Init(int index, int hsize, int vsize);
	void _cam_Exit();
	BOOL _cam_IsActive();
	void _cam_SetCallback(void* Ptr, VInCallback cb);

public:
	app_capture_exe();
	~app_capture_exe();
};
