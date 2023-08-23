// DxSound.h: interface for the DxSound class.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _DXSOUND_H_
#define  _DXSOUND_H_

#include "Clock.h"
#include "Stream.h"

typedef struct tagAUD_ST_HDR{
	UINT	SAMPLE;
	UINT	DATABIT;
	UINT	PCM_SIZE;
} AUD_ST_HDR;
struct AUD_PACKET{
	AUD_ST_HDR	AUD_hdr_t;
	uint8		Payload[MAX_AUDIO_SIZE];
};

class DxSound
{
private:
	LPDIRECTSOUND8 lpDS;
	LPDIRECTSOUNDBUFFER dsoundPrimary;
	LPDIRECTSOUNDBUFFER dsoundBuffer;
	LPDIRECTSOUNDNOTIFY dsoundNotify;
	LPDSBPOSITIONNOTIFY dsbpNotifies;

	HWND m_hWnd;
	WAVEFORMATEX m_wfx;

	DWORD PlayBufSize;
	DWORD m_dwNotifySize;
	HANDLE * m_hNotifies;

	ClockReference * m_pClock;
	MediaStream* m_pDatStream;

	BOOL m_buf_Empty;
	ZCriticalSection m_cslock;

public:
	WCHAR * Init(CWnd * pWnd);
	void Shutdown();

	void SendSoundSrc(AUD_SNDSRC* pAud_snd, BOOL bBufInit);

private:
	BOOL InitDirectSound(BOOL fInitialize = true);
	BOOL InitDirectSoundBuffer(BOOL fInitialize = true);
	BOOL InitDirectSoundNotify(BOOL fInitialize = true);

	BOOL IsFormat(AUD_PACKET * pAUD_Packet);
	BOOL CheckDSFormat(AUD_PACKET * pAUD_Packet);

	void SetBufEmpty();

private:
	friend static void DirectThread(void * pDs);
	void DirectThread();

	BOOL m_bDs_Th;
	T_Event m_evtDs_ThEnd;

public:
	DxSound();
	~DxSound();
};

#endif // _DXSOUND_H_
