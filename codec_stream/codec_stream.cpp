// codec_stream.cpp : DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "framework.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static int LockMgr(void** mutex, enum AVLockOp op)
{
	switch(op)
	{
	case AV_LOCK_CREATE:
		{
			HANDLE hHandle = NULL;
			hHandle = CreateMutex(NULL, FALSE, NULL);
			if(hHandle)
			{
				*mutex = hHandle;
			}
			return (hHandle == NULL);
		}
		break;
	case AV_LOCK_DESTROY:
		{
			HANDLE hHandle = *mutex;
			if(hHandle) CloseHandle(hHandle);
			return 0;
		}
		break;
	case AV_LOCK_OBTAIN:
		{
			HANDLE hHandle = *mutex;
			if(hHandle) WaitForSingleObject(hHandle, INFINITE);
		}
		break;
	case AV_LOCK_RELEASE:
		{
			HANDLE hHandle = *mutex;
			if(hHandle) ReleaseMutex(hHandle);
		}
		break;
	default:
		return -1; // invalid option
	}

	return 0;
}
BOOL _codec_ffmpeg_Init()
{
	avcodec_register_all();

	if(av_lockmgr_register(&LockMgr))
		return FALSE;
	return TRUE;
}
void _codec_ffmpeg_Exit()
{
	av_lockmgr_register(NULL);
}

#include "H264ENC.h"

codec_video_enc::codec_video_enc()
{
	m_penc = new H264ENC;
}
codec_video_enc::~codec_video_enc()
{
	if(m_penc)
	{
		delete m_penc;
		m_penc = NULL;
	}
}
bool codec_video_enc::Init(int hsize, int vsize, int fps, int quality)
{
	if(m_penc)
	{
		if(m_penc->Init(hsize, vsize, fps, quality))
			return true;
	}
	return false;
}
void codec_video_enc::Exit()
{
	if(m_penc)
	{
		m_penc->Exit();
	}
}
bool codec_video_enc::Encode(VID_IMGSRC* pVid_img, uint8* pOutput, int& nOutsize, BOOL& bRecvI)
{
	if(m_penc)
	{
		if(m_penc->Encode(pVid_img, pOutput, nOutsize, bRecvI))
			return true;
	}
	return false;
}


#include "H264DEC.h"

codec_video_dec::codec_video_dec()
{
	m_pdec = new H264DEC;
}
codec_video_dec::~codec_video_dec()
{
	if(m_pdec)
	{
		delete m_pdec;
		m_pdec = NULL;
	}
}
bool codec_video_dec::Init()
{
	if(m_pdec)
	{
		if(m_pdec->Init())
			return true;
	}
	return false;
}
void codec_video_dec::Exit()
{
	if(m_pdec)
	{
		m_pdec->Exit();
	}
}
bool codec_video_dec::Decode(uint8* pInput, int nInsize, VID_IMGSRC* pVid_img)
{
	if(m_pdec)
	{
		if(m_pdec->Decode(pInput, nInsize, pVid_img))
			return true;
	}
	return false;
}


#include "G726ENC.h"

codec_audio_enc::codec_audio_enc()
{
	m_penc = new G726ENC;
}
codec_audio_enc::~codec_audio_enc()
{
	if(m_penc)
	{
		delete m_penc;
		m_penc = NULL;
	}
}
bool codec_audio_enc::Init(uint32 aud_bit, uint32 channel, uint32 samples)
{
	if(m_penc)
	{
		if(m_penc->Init(aud_bit, channel, samples))
			return true;
	}
	return false;
}
void codec_audio_enc::Exit()
{
	if(m_penc)
	{
		m_penc->Exit();
	}
}
bool codec_audio_enc::Encode(AUD_SNDSRC* pAud_snd, uint8* pOutput, int& nOutsize)
{
	if(m_penc)
	{
		if(m_penc->Encode(pAud_snd, pOutput, nOutsize))
			return true;
	}
	return false;
}


#include "G726DEC.h"

codec_audio_dec::codec_audio_dec()
{
	m_pdec = new G726DEC;
}
codec_audio_dec::~codec_audio_dec()
{
	if(m_pdec)
	{
		delete m_pdec;
		m_pdec = NULL;
	}
}
bool codec_audio_dec::Init(uint32 aud_bit, uint32 channel, uint32 samples)
{
	if(m_pdec)
	{
		if(m_pdec->Init(aud_bit, channel, samples))
			return true;
	}
	return false;
}
void codec_audio_dec::Exit()
{
	if(m_pdec)
	{
		m_pdec->Exit();
	}
}
bool codec_audio_dec::Decode(uint8* pInput, int nInsize, AUD_SNDSRC* pAud_snd)
{
	if(m_pdec)
	{
		if(m_pdec->Decode(pInput, nInsize, pAud_snd))
			return true;
	}
	return false;
}

/*****************************************************************************
 *
 *  ScaleImage
 *
 ****************************************************************************/

#include "ScaleImage.h"

codec_scale::codec_scale()
{
	m_pscl = new ScaleImage;
	if(!m_pscl->Init())
	{
		delete m_pscl;
		m_pscl = NULL;
	}
}
codec_scale::~codec_scale()
{
	if(m_pscl)
	{
		m_pscl->Exit();
		delete m_pscl;
		m_pscl = NULL;
	}
}
void codec_scale::Clear()
{
	if(m_pscl)
	{
		m_pscl->Clear();
	}
}
BOOL codec_scale::ScaleVideo(VID_IMGSRC* pVid_src, VID_IMGSRC* pVid_dst)
{
	if(m_pscl)
	{
		if(m_pscl->Scaling(pVid_src, pVid_dst))
			return TRUE;
	}
	return FALSE;
}
