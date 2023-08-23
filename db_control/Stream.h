#if !defined(_stream_h)	// Sentry, use file only if it's not already included.
#define _stream_h

/*-------------------------------------------------------------------------
Project Im1Player
INTERNATIONAL ORGANIZATION FOR STANDARDIZATION
ISO/IEC JTC1/SC29/WG11

SUBSYSTEM:  Buffer management
FILE:       Stream.h
AUTHOR:     Zvi Lifshitz (VDOnet Corp.)
OVERVIEW:   Definition of MediaStream.
-------------------------------------------------------------------------*/
#include <stdio.h>

#define STOREINT(n, pBuf) (* (int *) (pBuf) = n)
#define LOADINT(pBuf) (* (int *) (pBuf))
#define STOREWORD(n, pBuf) (* (WORD *) (pBuf) = n)
#define LOADWORD(pBuf) (* (WORD *) (pBuf))
#define STOREDWORD(n, pBuf) (* (DWORD *) (pBuf) = n)
#define LOADDWORD(pBuf) (* (DWORD *) (pBuf))
#define STOREDTIME(n, pBuf) (* (DTime *) (pBuf) = n)
#define LOADDTIME(pBuf) (* (DTime *) (pBuf))
#define END_OF_STREAM  ((void*)-1)

class ClockReference;

class MediaStream
{
	int m_nBufferSize;
	LPBYTE m_pBuffer;
	int m_nAllocatedLength, m_nLockedLength;
	int m_nFreeStart, m_nTakenStart;
	ZMutex m_mtxLockStream;
	ZSemaphore m_smpLockOutBlock, m_smpWaitForMemory, m_smpWaitForData;
	BOOL m_bWaitingForMemory, m_bReallocInProgress, m_bWaitingForData;
	BOOL m_bWait, m_bShove, m_bEndOfStream, m_bFirstRetrieved;
	ClockReference *m_pClock;

	ZEvent m_evtWait;

private:
	void Discard ();
	int GetBlockLength (int nOffset)
	{
		return LOADINT (m_pBuffer + nOffset);
	}
	DTime GetTimeStamp (int nOffset)
	{
		return LOADDTIME (m_pBuffer + nOffset + sizeof (int));
	}
	int GetNext (int nOffset);


public:
	MediaStream (ClockReference *pClock, int nBufferSize);
	~MediaStream ();
	void Reset ();
	LPVOID Allocate (int nSize, BOOL bWait = TRUE, BOOL bShove = FALSE);
	LPVOID Reallocate (int nSize);
	BOOL Dispatch (int nSize, DTime dtTime);
	LPVOID FetchNext (int &rnSize, DTime &rdtTime, BOOL bWait = TRUE);
	LPVOID Fetch (
		int &rnSize,
		DTime &rdtTime,
		BOOL &rbRecurrent,
		BOOL bSkip = FALSE,
		BOOL bWait = TRUE,
		DTime dtDelta = 0
	);
	void Release (BOOL bDiscard = TRUE);
	void Close ();

	void UnlockWait(BOOL bReset = FALSE);
	BOOL SetBufEmpty();
	BOOL IsBufferEmpty();
	int  GetBufferRate();
	int  GetBufferSize();
};

#endif                                      // _stream_h sentry.
