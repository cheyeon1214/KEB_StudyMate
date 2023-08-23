// AinputControl.cpp : implementation file
//

#include "pch.h"
#include "AinputControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AinputControl

AinputControl::AinputControl()
: m_smpActSave(1,0), m_smpActDone(1,0), m_smpThreadEnd(1,0) // thr end
{
	m_bActive = FALSE;
	m_bLocked = FALSE;
	m_nCurCount = 0;
	m_nBufOrder = 0;
	m_bEnd = FALSE;

	PlayBufSize = 2000;

	m_hThreadEnd = NULL;

	m_ptr = NULL;
	m_cbAIn = NULL;

	m_bWaveInOpen = FALSE;
	m_bRecording = FALSE;
	m_hWaveIn = NULL;
	memset(&m_pcmIn, 0, sizeof(WAVEFORMATEX));

	for(int i = 0; i < NUMOFWAVEBUFFER; i++)
	{
		m_hInBuffer[i] = NULL;
		m_hWaveInHdr[i] = NULL;
		m_lpInBuffer[i] = NULL;
		m_lpWaveInHdr[i] = NULL;
	}

	CString szWcName = AfxRegisterWndClass(CS_SAVEBITS);
	CreateEx(WS_EX_OVERLAPPEDWINDOW, szWcName, L"WaveInWindow",WS_POPUP,0,0,0,0, NULL, NULL, NULL);
}

AinputControl::~AinputControl()
{
	SAFE_TERMINATE(m_hThreadEnd);
}


BEGIN_MESSAGE_MAP(AinputControl, CWnd)
	//{{AFX_MSG_MAP(AinputControl)
	ON_MESSAGE(MM_WIM_DATA, OnDataIn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// AinputControl message handlers

BOOL AinputControl::Init(uint samples, uint databit)
{
	m_pcmIn.nChannels = 1; // mono
	m_pcmIn.wBitsPerSample = databit;
	m_pcmIn.nSamplesPerSec = samples;
	if(!WaveInOpen(WAVE_MAPPER)) return FALSE;

	m_hThreadEnd = (HANDLE)_beginthread(::CaptureThread,0,this);
	
	m_smpThreadEnd.V();

	WaveInPrepare();
	WaveInStart();

	m_bActive = TRUE;

	return TRUE;
}
void AinputControl::Exit()
{
	m_bActive = FALSE;
	m_bEnd = TRUE;
	
	if(!CheckLock())
		m_smpActSave.V();

	WaveTerminate();
}

static void CaptureThread(void *pCap)
{
	((AinputControl *)pCap)->CaptureThread();
}
void AinputControl::CaptureThread()
{
	// thr end
	m_smpThreadEnd.P();

	while(!m_bEnd)
	{
		m_smpActSave.P();
		if(m_bEnd)
			break;
		SetLock(TRUE);
		int nTemCount = GetCount();
		m_smpActDone.V();
		m_smpActSave.V();
		for(int i=0; i<nTemCount; i++)
		{
			if(!SaveToStream(m_lpWaveInHdr[m_nBufOrder], 0) || m_bEnd)
			{
				m_smpActDone.P();
				WaveInStop();
				m_smpThreadEnd.V();
				m_hThreadEnd = NULL;
				return;
			}
			m_nBufOrder++;

			if(m_nBufOrder == NUMOFWAVEBUFFER)
				m_nBufOrder = 0;
		}
		m_smpActDone.P();
		SetLock(FALSE);
		Sleep(0);
	}

	WaveInStop();
	m_smpThreadEnd.V();

	m_hThreadEnd = NULL;
}

BOOL AinputControl::CheckLock()
{
	BOOL a;
	m_mtxThreadLocked.P();
	a = m_bLocked;
	m_mtxThreadLocked.V();
	return a;
}

void AinputControl::SetLock(BOOL mode)
{
	m_mtxThreadLocked.P();
	m_bLocked = mode;
	m_mtxThreadLocked.V();
	return;
}

void AinputControl::AddCount(int value)
{
	m_mtxCurCount.P();
	m_nCurCount += value;
	m_mtxCurCount.V();
}

int AinputControl::GetCount()
{
	int count;
	m_mtxCurCount.P();
	count = m_nCurCount;
	m_nCurCount = 0;
	m_mtxCurCount.V();
	return count;
}

BOOL AinputControl::SaveToStream(LPWAVEHDR lpCurrent, DWORD dwTime)
{
	if(m_bActive)
	{
		AUD_SNDSRC stAud_snd;
		stAud_snd.channel = m_pcmIn.nChannels;
		stAud_snd.samples = m_pcmIn.nSamplesPerSec;
		stAud_snd.databit = m_pcmIn.wBitsPerSample;
		stAud_snd.aud_ptr = (uint8*)(lpCurrent->lpData);
		stAud_snd.audbytes = PlayBufSize;

		if(m_cbAIn)
		{
			m_cbAIn(m_ptr, &stAud_snd);
		}
	}

	waveInAddBuffer(m_hWaveIn, lpCurrent, sizeof(WAVEHDR));

	return TRUE;
}

// Process Wave In Buffer on MM_WIM_DATA message
LRESULT AinputControl::OnDataIn(WPARAM wParam, LPARAM lParam)
{
	if(!m_bEnd)
	{
		if(CheckLock())
			AddCount(1);
		else
		{
			AddCount(1);
			m_smpActSave.V();
			m_smpActDone.P();
			m_smpActSave.P();
			m_smpActDone.V();
		}
	}
	else
	{		
		if(!CheckLock())
			m_smpActSave.V();
	}

	return 0;
}

BOOL AinputControl::WaveInOpen(MCIDEVICEID wDeviceID)
{
	if(m_bWaveInOpen)
		return FALSE;

	for(int i = 0; i < NUMOFWAVEBUFFER; i++)
	{
		m_hWaveInHdr[i] = GlobalAlloc(GHND | GMEM_SHARE, sizeof(WAVEHDR));
		m_lpWaveInHdr[i] = (LPWAVEHDR)GlobalLock(m_hWaveInHdr[i]);
		m_hInBuffer[i] = GlobalAlloc(GHND | GMEM_SHARE, PlayBufSize);
		m_lpInBuffer[i] = (LPBYTE)GlobalLock(m_hInBuffer[i]);
		m_lpWaveInHdr[i]->lpData = (char*)m_lpInBuffer[i];
		m_lpWaveInHdr[i]->dwBufferLength = PlayBufSize;
		m_lpWaveInHdr[i]->dwBytesRecorded = 0L;
		m_lpWaveInHdr[i]->dwUser = 0L;
		m_lpWaveInHdr[i]->dwFlags = 0L;
		m_lpWaveInHdr[i]->dwLoops = 1L;
		m_lpWaveInHdr[i]->lpNext = NULL;
		m_lpWaveInHdr[i]->reserved = 0L;
	}

	// Set Wave format 
	m_pcmIn.wFormatTag = WAVE_FORMAT_PCM;
	m_pcmIn.nBlockAlign = m_pcmIn.nChannels * m_pcmIn.wBitsPerSample/8;
	m_pcmIn.nAvgBytesPerSec = m_pcmIn.nBlockAlign * m_pcmIn.nSamplesPerSec;

	// Open Wave In Device
	UINT n_numDevice  = waveInGetNumDevs(); 

	for(int i = 0; i < 100; i++)
	{
		for(int j = 0; j < (int)n_numDevice; j++)
		{
			if(!waveInOpen(&m_hWaveIn, j, &m_pcmIn,(DWORD_PTR)this->m_hWnd, 0L, CALLBACK_WINDOW))
			{		
				m_bWaveInOpen = TRUE;
				return TRUE;
			}
			Sleep(10);
		}
	}

	return FALSE;
}

BOOL AinputControl::WaveInStart()
{
    waveInStart(m_hWaveIn);
    m_bRecording = TRUE ;
  
	return TRUE;
}

BOOL AinputControl::WaveInPrepare()
{
	int i;

	if(!m_bWaveInOpen)
		return FALSE;

	for(i=0; i<NUMOFWAVEBUFFER; i++)
    {
        if(waveInPrepareHeader(m_hWaveIn, m_lpWaveInHdr[i], sizeof(WAVEHDR))) 
        {
            AfxMessageBox(L"WaveInPrepareHeader Error"); 
            return FALSE;
        }
        
        if(waveInAddBuffer(m_hWaveIn, m_lpWaveInHdr[i], sizeof(WAVEHDR))) 
        {
            AfxMessageBox(L"WaveInAddBuffer Error");
            return FALSE;
        }
    }
	return TRUE;
}

BOOL AinputControl::WaveInStop()
{
	if(!m_bWaveInOpen || !m_bRecording)
		return FALSE;

	if(waveInReset(m_hWaveIn))
	{
		AfxMessageBox(L"Cannot Reset Wave Device");
		return FALSE;
	}
	else
		m_bRecording = FALSE;
		
	return TRUE;
}

BOOL AinputControl::WaveInClose()
{
	int i;

	if(m_bRecording)
		WaveInStop();

	if(m_bWaveInOpen)
		waveInClose(m_hWaveIn);
	
	for(i=0; i<NUMOFWAVEBUFFER; i++)
    {
        GlobalUnlock(m_hWaveInHdr[i]);
        GlobalUnlock(m_hInBuffer[i]);
        GlobalFree(m_hWaveInHdr[i]);
        GlobalFree(m_hInBuffer[i]);
    }
    m_bWaveInOpen=FALSE;
	
	return TRUE;
}

BOOL AinputControl::WaveTerminate()
{
	if(!m_bWaveInOpen)
		return FALSE;

	if(m_bRecording)
		WaveInStop();

	if(m_bWaveInOpen)
		WaveInClose();

	return TRUE;
}

void AinputControl::SetCallback(void* Ptr, AInCallback cb)
{
	m_ptr = Ptr;
	m_cbAIn = cb;
}
