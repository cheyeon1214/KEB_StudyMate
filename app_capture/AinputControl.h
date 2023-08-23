#ifndef _AINPUT_CONTROL_H_
#define _AINPUT_CONTROL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AinputControl.h : header file
//

typedef void(*AInCallback)(void* Ptr, AUD_SNDSRC* pAud_snd);

/////////////////////////////////////////////////////////////////////////////
// AinputControl window

#define NUMOFWAVEBUFFER	50

class AinputControl : public CWnd
{
private:
	BOOL m_bActive;
	BOOL m_bLocked;
	int m_nCurCount;
	int m_nBufOrder;
	BOOL m_bEnd;

	DWORD PlayBufSize;

	ZSemaphore m_smpActSave;
	ZSemaphore m_smpActDone;

	// thr end
	ZSemaphore	m_smpThreadEnd;
	HANDLE		m_hThreadEnd;

	ZMutex m_mtxThreadLocked;
	ZMutex m_mtxCurCount;

	void* m_ptr;
	AInCallback m_cbAIn;

public:
	BOOL Init(uint samples, uint databit);
	void Exit();

private:
	friend static void CaptureThread(void* pCap);
	void CaptureThread();

	BOOL CheckLock();
	void SetLock(BOOL mode);
	void AddCount(int value);
	int GetCount();


	////////////////////////////////////////////////////////////////
	// Wave Process function
	////////////////////////////////////////////////////////////////
	BOOL SaveToStream(LPWAVEHDR lpCurrent, DWORD dwTime);
	BOOL WaveInOpen(MCIDEVICEID);
	BOOL WaveInStart();
	BOOL WaveInStop();
	BOOL WaveInClose();
	BOOL WaveInPrepare();
	BOOL WaveTerminate();

	////////////////////////////////////////////////////////////////
	// Wave Process Attribute
	////////////////////////////////////////////////////////////////
private:
	BOOL m_bWaveInOpen;
	BOOL m_bRecording;
	WAVEFORMATEX m_pcmIn;

	HWAVEIN m_hWaveIn;

	GLOBALHANDLE    m_hInBuffer[NUMOFWAVEBUFFER];
	GLOBALHANDLE    m_hWaveInHdr[NUMOFWAVEBUFFER];
	LPBYTE          m_lpInBuffer[NUMOFWAVEBUFFER];
	LPWAVEHDR       m_lpWaveInHdr[NUMOFWAVEBUFFER];

public:	// CALLBACK
	void SetCallback(void* Ptr, AInCallback cb);

public:
	AinputControl();
	virtual ~AinputControl();

	// Attributes
public:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(AinputControl)
		//}}AFX_VIRTUAL

		// Generated message map functions
protected:
	//{{AFX_MSG(AinputControl)
	afx_msg LRESULT OnDataIn(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
