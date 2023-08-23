// DxSound.cpp: implementation of the DxSound class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "DxSound.h"

const int DS_NOTIFY_COUNT = 13;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxSound::DxSound()
{
	lpDS = NULL;
	dsoundPrimary = NULL;
	dsoundBuffer = NULL;
	dsoundNotify = NULL;
	dsbpNotifies = NULL;

	m_hWnd = NULL;
	memset(&m_wfx, 0, sizeof(WAVEFORMATEX));

	PlayBufSize = 0;
	m_dwNotifySize = 0;
	m_hNotifies = NULL;

	m_pClock = NULL;
	m_pDatStream = NULL;
	m_buf_Empty = FALSE;

	m_bDs_Th = FALSE;
}

DxSound::~DxSound()
{
	Shutdown();
}

WCHAR * DxSound::Init(CWnd * pWnd)
{
	m_hWnd = pWnd->GetSafeHwnd();

	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nChannels = 1;
	m_wfx.nSamplesPerSec = 8000;
	m_wfx.wBitsPerSample = 16;
	m_wfx.nBlockAlign = (m_wfx.wBitsPerSample / 8) * m_wfx.nChannels;
	m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;

	PlayBufSize = 1000;

	HRESULT hr;
	static WCHAR errorMessage[200];

	hr = DirectSoundCreate8(NULL, &lpDS, NULL);
	if (FAILED(hr) || !lpDS)
	{
		wcscpy_s (errorMessage, L"Error in DirectSoundCreate");
		return (errorMessage);
	}
	
	hr = lpDS->SetCooperativeLevel(m_hWnd, DSSCL_PRIORITY);
	if (FAILED(hr))
	{
		wcscpy_s (errorMessage, L"Error in SetCooperativeLevel");
		return (errorMessage);
	}

	if(!InitDirectSound(TRUE))
	{
		wcscpy_s (errorMessage, L"Error in InitDirectSound()");
		return (errorMessage);
	}
	if(!InitDirectSoundBuffer(TRUE))
	{
		wcscpy_s (errorMessage, L"Error in InitDirectSoundBuffer()");
		return (errorMessage);
	}
	if(!InitDirectSoundNotify(TRUE))
	{
		wcscpy_s (errorMessage, L"Error in InitDirectSoundNotify()");
		return (errorMessage);
	}

	if(!m_bDs_Th)
	{
		m_pClock = new ClockReference;
		m_pDatStream = new MediaStream(m_pClock, 4 * MAX_AUDIO_SIZE);

		m_bDs_Th = TRUE;
		m_evtDs_ThEnd.T_Begin(::DirectThread,this);
	}
	return NULL;
}

void DxSound::Shutdown()
{
	if(m_bDs_Th)
	{
		m_bDs_Th = FALSE;

		m_pDatStream->Close();

		dsoundBuffer->Stop();
		SetEvent(m_hNotifies[DS_NOTIFY_COUNT-1]);

		m_evtDs_ThEnd.T_Terminate();

		SAFE_DELETE(m_pClock);
		SAFE_DELETE(m_pDatStream);
	}

	InitDirectSoundNotify(FALSE);
	InitDirectSoundBuffer(FALSE);
	InitDirectSound(FALSE);

	SAFE_RELEASE(lpDS);
}

BOOL DxSound::InitDirectSound(BOOL fInitialize)
{
	if(fInitialize)
	{
		HRESULT hr;

		DSBUFFERDESC dsbd;
		memset(&dsbd, 0, sizeof(DSBUFFERDESC));
		dsbd.dwSize			= sizeof(DSBUFFERDESC);
		dsbd.dwFlags		= DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
		dsbd.dwBufferBytes	= 0;
		dsbd.lpwfxFormat	= NULL;

		hr = lpDS->CreateSoundBuffer(&dsbd, &dsoundPrimary, NULL);
		if (FAILED(hr) || !dsoundPrimary)
			return FALSE;

		hr = dsoundPrimary->SetFormat(&m_wfx);
		if (FAILED(hr))
			return FALSE;
		//dsoundPrimary->SetVolume(DSBVOLUME_MAX);
	}
	else
	{
		SAFE_RELEASE(dsoundPrimary);
	}

	return TRUE;
}
BOOL DxSound::InitDirectSoundBuffer(BOOL fInitialize)
{
	if(fInitialize)
	{
		HRESULT hr;

		DSBUFFERDESC dsbd;
		memset(&dsbd, 0, sizeof(DSBUFFERDESC));
		dsbd.dwSize        = sizeof(DSBUFFERDESC);
		dsbd.dwFlags       = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN;
		dsbd.dwBufferBytes = PlayBufSize * (DS_NOTIFY_COUNT-1);
		dsbd.lpwfxFormat   = &m_wfx;

		hr = lpDS->CreateSoundBuffer(&dsbd, &dsoundBuffer, NULL);
		if (FAILED(hr) || !dsoundBuffer)
			return FALSE;
		//dsoundBuffer->SetPan(0);
	}
	else
	{
		SAFE_RELEASE(dsoundBuffer);
	}

	return TRUE;
}
BOOL DxSound::InitDirectSoundNotify(BOOL fInitialize)
{
	if(fInitialize)
	{
		HRESULT hr;

		m_dwNotifySize = PlayBufSize;

		hr = dsoundBuffer->QueryInterface(IID_IDirectSoundNotify, (VOID **)&dsoundNotify);
		if (FAILED(hr) || !dsoundNotify)
			return FALSE;

		m_hNotifies = new HANDLE[DS_NOTIFY_COUNT];
		dsbpNotifies = new DSBPOSITIONNOTIFY[DS_NOTIFY_COUNT];

		for (int i=0; i<DS_NOTIFY_COUNT-1; i++)
		{
			m_hNotifies[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
			dsbpNotifies[i].dwOffset = (m_dwNotifySize * i) + m_dwNotifySize - 1;
			dsbpNotifies[i].hEventNotify = m_hNotifies[i];
		}
		m_hNotifies[DS_NOTIFY_COUNT-1] = CreateEvent(NULL, FALSE, FALSE, NULL);
		dsbpNotifies[DS_NOTIFY_COUNT-1].dwOffset = DSBPN_OFFSETSTOP;
		dsbpNotifies[DS_NOTIFY_COUNT-1].hEventNotify = m_hNotifies[DS_NOTIFY_COUNT-1];
		
		hr = dsoundNotify->SetNotificationPositions(DS_NOTIFY_COUNT, dsbpNotifies);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		if(m_hNotifies != NULL)
		{
			for(int i=0; i<DS_NOTIFY_COUNT; i++)
				CloseHandle(m_hNotifies[i]);
		}

		SAFE_DELETEA(m_hNotifies);
		SAFE_DELETEA(dsbpNotifies);
		SAFE_RELEASE(dsoundNotify);
	}

	return TRUE;
}

BOOL DxSound::IsFormat(AUD_PACKET * pAUD_Packet)
{
	if(pAUD_Packet == NULL)
		return TRUE;

	BOOL bFormatOK = TRUE;

	if(m_wfx.nSamplesPerSec != (DWORD)pAUD_Packet->AUD_hdr_t.SAMPLE)
		bFormatOK = FALSE;
	if(m_wfx.wBitsPerSample != (WORD)pAUD_Packet->AUD_hdr_t.DATABIT)
		bFormatOK = FALSE;

	if((pAUD_Packet->AUD_hdr_t.PCM_SIZE % PlayBufSize) != 0)
	{
		if(pAUD_Packet->AUD_hdr_t.PCM_SIZE < 1000)
		{
			PlayBufSize = pAUD_Packet->AUD_hdr_t.PCM_SIZE;
		}
		else
		{
			if((pAUD_Packet->AUD_hdr_t.PCM_SIZE % 1000) == 0)
				 PlayBufSize = 1000;
			else if((pAUD_Packet->AUD_hdr_t.PCM_SIZE % 1024) == 0)
				 PlayBufSize = 1024;
			else PlayBufSize = pAUD_Packet->AUD_hdr_t.PCM_SIZE;
		}
		bFormatOK = FALSE;
	}
	if(!bFormatOK)
	{
		m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_wfx.nChannels = 1;
		m_wfx.nSamplesPerSec = (DWORD)pAUD_Packet->AUD_hdr_t.SAMPLE;
		m_wfx.wBitsPerSample = (WORD)pAUD_Packet->AUD_hdr_t.DATABIT;
		m_wfx.nBlockAlign = (m_wfx.wBitsPerSample / 8) * m_wfx.nChannels;
		m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
	}
	return bFormatOK;
}
BOOL DxSound::CheckDSFormat(AUD_PACKET * pAUD_Packet)
{
	if(pAUD_Packet == NULL)
		return TRUE;

	if(!IsFormat(pAUD_Packet))
	{
		dsoundBuffer->Stop();

		InitDirectSoundNotify(FALSE);
		InitDirectSoundBuffer(FALSE);
		InitDirectSound(FALSE);

		if(!InitDirectSound(TRUE))
			return FALSE;
		if(!InitDirectSoundBuffer(TRUE))
			return FALSE;
		if(!InitDirectSoundNotify(TRUE))
			return FALSE;

		char *pBuffer;
		DWORD dwBufferLen;

		HRESULT hr = dsoundBuffer->Lock(0, 0, (LPVOID *)&pBuffer, &dwBufferLen, NULL, NULL, DSBLOCK_ENTIREBUFFER);
		if(SUCCEEDED(hr))
		{
			memset(pBuffer, (m_wfx.wBitsPerSample == 8) ? 128 : 0, dwBufferLen);
			dsoundBuffer->Unlock(pBuffer, dwBufferLen, NULL, 0);
		}
		for (int i=0; i<DS_NOTIFY_COUNT; i++)
			ResetEvent(m_hNotifies[i]);

		hr = dsoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
		if(FAILED(hr)) return FALSE;
	}

	return TRUE;
}

void DxSound::SetBufEmpty()
{
	m_cslock.P();
	m_buf_Empty = TRUE;

	if(m_pDatStream)
	{
		m_pDatStream->UnlockWait();
		while(!m_pDatStream->IsBufferEmpty())
			Sleep(1);
		m_pDatStream->UnlockWait(TRUE);
	}

	m_buf_Empty = FALSE;
	m_cslock.V();
}

void DxSound::SendSoundSrc(AUD_SNDSRC* pAud_snd, BOOL bBufInit)
{
	if(m_bDs_Th)
	{
		if(bBufInit)
		{
			SetBufEmpty();

			char *pBuffer;
			DWORD dwBufferLen;

			HRESULT hr = dsoundBuffer->Lock(0, 0, (LPVOID *)&pBuffer, &dwBufferLen, NULL, NULL, DSBLOCK_ENTIREBUFFER);
			if (SUCCEEDED(hr))
			{
				memset(pBuffer, (m_wfx.wBitsPerSample == 8) ? 128 : 0, dwBufferLen);
				dsoundBuffer->Unlock(pBuffer, dwBufferLen, NULL, 0);
			}
		}
		if(!pAud_snd) return;

		m_cslock.P();

		int nOutSize = sizeof(AUD_ST_HDR) + pAud_snd->audbytes;
		AUD_PACKET * pAUD_Packet = (AUD_PACKET* )m_pDatStream->Allocate(nOutSize, FALSE, FALSE);
		if(pAUD_Packet != NULL)
		{
			pAUD_Packet->AUD_hdr_t.SAMPLE = pAud_snd->samples;
			pAUD_Packet->AUD_hdr_t.DATABIT = pAud_snd->databit;
			pAUD_Packet->AUD_hdr_t.PCM_SIZE = pAud_snd->audbytes;
			memcpy(pAUD_Packet->Payload, pAud_snd->aud_ptr, pAud_snd->audbytes);

			m_pDatStream->Dispatch(nOutSize, 0);
		}

		m_cslock.V();
	}
}

static void DirectThread(void * pDs)
{
	((DxSound *) pDs)->DirectThread();
}
void DxSound::DirectThread()
{
	uint8*	pData;
	int		nSize;
	DTime	dtTime = 0;

	int nIndex = 0;
	DWORD dwResult;

	int PlayCount = 0;

	while((pData = (uint8*)m_pDatStream->FetchNext(nSize, dtTime, FALSE)) != END_OF_STREAM)
	{
		if(m_bDs_Th)
		{
			if(m_buf_Empty)
			{
				m_pDatStream->Release(TRUE);
				m_pDatStream->SetBufEmpty();
				continue;
			}
			if(pData)
			{
				if(!PlayCount)
				{
					char *pBuffer;
					DWORD dwBufferLen;

					HRESULT hr = dsoundBuffer->Lock(0, 0, (LPVOID *)&pBuffer, &dwBufferLen, NULL, NULL, DSBLOCK_ENTIREBUFFER);
					if (SUCCEEDED(hr))
					{
						memset(pBuffer, (m_wfx.wBitsPerSample == 8) ? 128 : 0, dwBufferLen);
						dsoundBuffer->Unlock(pBuffer, dwBufferLen, NULL, 0);
					}
					for (int i=0; i<DS_NOTIFY_COUNT; i++)
						ResetEvent(m_hNotifies[i]);

					hr = dsoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
					if(FAILED(hr)) break;
				}
				PlayCount = 200;
			}
			else
			{
				if(PlayCount)
				{
					PlayCount--;
					if(!PlayCount) dsoundBuffer->Stop();
				}
				Sleep(10);
			}

			if(PlayCount)
			{
				AUD_PACKET * pAUD_Packet = (AUD_PACKET *)pData;

				int bufIDX = 0;
				BOOL bLOOP = TRUE;
				if(pData == NULL)
					bLOOP = FALSE;

				if(!CheckDSFormat(pAUD_Packet))
					break;
				do
				{
					dwResult = ::WaitForMultipleObjects(DS_NOTIFY_COUNT, m_hNotifies, FALSE, INFINITE);
					if(WAIT_OBJECT_0 <= dwResult && dwResult <= (WAIT_OBJECT_0+DS_NOTIFY_COUNT-2))
					{
						nIndex = dwResult - WAIT_OBJECT_0;

						uint8* pBuffer = NULL;
						DWORD dwBufferLen;

						DWORD dwWriteCursor = PlayBufSize * nIndex;
						HRESULT hr = dsoundBuffer->Lock(dwWriteCursor, PlayBufSize, (LPVOID *)&pBuffer, &dwBufferLen, NULL, NULL, 0L);
						if(SUCCEEDED(hr))
						{
							if(pData != NULL)
							{
								if(pAUD_Packet->AUD_hdr_t.PCM_SIZE > (bufIDX + dwBufferLen))
								{
									memcpy(pBuffer, (uint8*)(pAUD_Packet->Payload + bufIDX), dwBufferLen);
									bufIDX += dwBufferLen;
								}
								else
								{
									DWORD dwCopyLen = pAUD_Packet->AUD_hdr_t.PCM_SIZE - bufIDX;
									memcpy(pBuffer, (uint8*)(pAUD_Packet->Payload + bufIDX), dwCopyLen);
									bLOOP = FALSE;
								}
							}
							else
							{
								memset(pBuffer, (m_wfx.wBitsPerSample == 8) ? 128 : 0, dwBufferLen);
							}
							dsoundBuffer->Unlock(pBuffer, dwBufferLen, NULL, 0);
						}
					}
					else if(dwResult == WAIT_OBJECT_0+DS_NOTIFY_COUNT-1)
					{
						bLOOP = FALSE;
					}
				}while(m_bDs_Th && bLOOP);
			}
		}

		m_pDatStream->Release(TRUE);
	}

	m_evtDs_ThEnd.T_End();
}
