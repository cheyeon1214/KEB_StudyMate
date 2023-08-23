/*-------------------------------------------------------------------------
Project Im1Player
INTERNATIONAL ORGANIZATION FOR STANDARDIZATION
ISO/IEC JTC1/SC29/WG11

SUBSYSTEM:  Buffer management
FILE:       Stream.cpp
AUTHOR:     Zvi Lifshitz (VDOnet Corp.)
OVERVIEW:   Implementation of MediaStream.
			(This version simulation only. Just makes sure Allocate,
			Reallocate, and Dispatch are called properly.
-------------------------------------------------------------------------*/
#include "pch.h"
#include <assert.h>
#include "Stream.h"
#include "Clock.h"


/*-----------------------------------------------------------------
MediaStream constructor. The simulation allocate a buffer large enough 
for all possible AUs
-----------------------------------------------------------------*/
MediaStream :: MediaStream (ClockReference *pClock, int nBufferSize) :
	m_smpWaitForMemory (1, 0),
	m_smpWaitForData (1, 0)
{
	/*-----------------------------------------------------------------
	Allocate the stream buffer.
	-----------------------------------------------------------------*/
	m_nBufferSize = nBufferSize;
	m_pBuffer = (LPBYTE) malloc (nBufferSize);
	m_pClock = pClock;
	Reset ();
}

/*-----------------------------------------------------------------
Reset the stream
-----------------------------------------------------------------*/
void MediaStream :: Reset ()
{
	m_nFreeStart = 0;
	m_nTakenStart = m_nBufferSize;
	m_nAllocatedLength = m_nLockedLength = -1;
	m_bWaitingForMemory = m_bReallocInProgress = m_bEndOfStream = FALSE;
	m_bFirstRetrieved = FALSE;
}

/*-----------------------------------------------------------------
MediaStream destructor
-----------------------------------------------------------------*/
MediaStream :: ~MediaStream ()
{
	m_evtWait.V();
	free (m_pBuffer);
}

/*-----------------------------------------------------------------
Allocate() allocates a block of nSize byte and returns its address
-----------------------------------------------------------------*/
LPVOID MediaStream :: Allocate (int nSize, BOOL bWait, BOOL bShove)
{
	assert (nSize > 0);

	/*-----------------------------------------------------------------
	Return NULL if last allocated block has not been dispatched
	or if required size exceeds buffer size
	-----------------------------------------------------------------*/
	nSize += sizeof (int) + sizeof (DTime);	// for block length and time stamp
	if (m_nAllocatedLength >= 0 && !m_bReallocInProgress ||
		nSize > m_nBufferSize
	)
		return NULL;

	/*-----------------------------------------------------------------
	Check available (contiguous) space.
	-----------------------------------------------------------------*/
	m_mtxLockStream.P ();
	while (
		nSize > m_nTakenStart - m_nFreeStart &&
		(	m_nTakenStart > m_nFreeStart ||
			m_nTakenStart == m_nFreeStart && !m_bReallocInProgress ||
			nSize > m_nBufferSize - m_nFreeStart &&
			nSize > m_nTakenStart
		)
	)
	{
		/*-----------------------------------------------------------------
		Not enough space available. One reason might be that the buffer
		is empty, and this is a reallocation of a block which is
		currently at the middle of the buffer
		-----------------------------------------------------------------*/
		if (m_nTakenStart == m_nFreeStart && m_bReallocInProgress)
		{	m_nFreeStart = 0;
			m_nTakenStart = m_nBufferSize;
		}
		/*-----------------------------------------------------------------
		If bShove it TRUE, shove last block
		(or wait for the currently locked block to be discarded).
		-----------------------------------------------------------------*/
		else if (bShove)
		{	if (m_nLockedLength >= 0)
			{	m_mtxLockStream.V ();
				m_smpLockOutBlock.P ();
				m_smpLockOutBlock.V ();
				m_mtxLockStream.P ();
			}
			else
				Discard ();
		}
		/*-----------------------------------------------------------------
		If the wait flag is true, wait till a space is available
		-----------------------------------------------------------------*/
		else if (bWait)
		{	m_bWaitingForMemory = TRUE;
			m_mtxLockStream.V ();
			m_smpWaitForMemory.P ();
			m_mtxLockStream.P ();
		}
		else
		{	m_mtxLockStream.V ();
			return NULL;
		}
	}

	/*-----------------------------------------------------------------
	Allocate a block of the desired length. Starting either at the next
	available byte, or cycle around the buffer
	-----------------------------------------------------------------*/
	if (m_nBufferSize - m_nFreeStart < nSize)
	{	STOREINT (-1, m_pBuffer + m_nFreeStart);	// Mark as not used
		if (m_nTakenStart == m_nFreeStart)
			m_nTakenStart = m_nBufferSize;
		m_nFreeStart = 0;
	}
	m_nAllocatedLength = nSize - (sizeof (int) + sizeof (DTime));
	m_bWait = bWait;			// save for realloc
	m_bShove = bShove;
	m_mtxLockStream.V ();
	return m_pBuffer + m_nFreeStart + sizeof (int) + sizeof (DTime);
}

/*-----------------------------------------------------------------
Reallocate() expands the reserved block, if necessary
-----------------------------------------------------------------*/
LPVOID MediaStream :: Reallocate (int nSize)
{
	assert (nSize >= 0);

	/*-----------------------------------------------------------------
	Reallocation is done by zeroing m_nAllocatedLength and calling
	Allocate with the new size.
	-----------------------------------------------------------------*/

	m_mtxLockStream.P ();

	int nPreviousStart = m_nFreeStart;
	int nPreviousLength = m_nAllocatedLength;
	LPVOID pResult;
	
	m_bReallocInProgress = TRUE;
	m_mtxLockStream.V ();
	pResult = Allocate (nSize, m_bWait, m_bShove);

	/*-----------------------------------------------------------------
	If realloc successful, but block was shifted in memory, copy the
	contents of the old block.
	-----------------------------------------------------------------*/
	if (pResult != NULL && m_nFreeStart != nPreviousStart)
		memcpy (
			pResult,
			m_pBuffer + nPreviousStart + sizeof (int) + sizeof (DTime),
			nPreviousLength
		);
	m_bReallocInProgress = FALSE;
	return pResult;
}

/*-----------------------------------------------------------------
Dispatch() dispatches the allocated block into the stream
-----------------------------------------------------------------*/
BOOL MediaStream :: Dispatch (int nSize, DTime dtTime)
{
	assert (nSize >= 0);

	if (m_nAllocatedLength < 0)
		return FALSE;

	/*-----------------------------------------------------------------
	store the size and time information. Signal to waiting threads.
	-----------------------------------------------------------------*/
	m_mtxLockStream.P ();
	m_bEndOfStream = FALSE;
	nSize += sizeof (int) + sizeof (DTime);
	STOREINT (nSize, m_pBuffer + m_nFreeStart);
	STOREDTIME (dtTime, m_pBuffer + m_nFreeStart + sizeof (int));

	m_nFreeStart += nSize;
	if (m_nFreeStart == m_nBufferSize)
		m_nFreeStart = 0;
	if (m_nTakenStart == m_nBufferSize)
		m_nTakenStart = 0;
	m_nAllocatedLength = -1;
	if (m_bWaitingForData)
	{	m_bWaitingForData = FALSE;
		m_smpWaitForData.V ();
	}
	m_mtxLockStream.V ();
	return TRUE;
}

/*-----------------------------------------------------------------
FetchNext fetches the next available block, ignoring its time stamp (which
is only returned in rdwTime.  If bWait is true, the function waits till
a block becomes available.
If successful, the function locks the fetched block.
If end of stream reached, the function returns END_OF_STREAM.
-----------------------------------------------------------------*/
LPVOID MediaStream :: FetchNext (
	int &rnSize,
	DTime &rdtTime,
	BOOL bWait
)
{
	/*-----------------------------------------------------------------
	Returns NULL if a block is already locked
	-----------------------------------------------------------------*/
	if (m_nLockedLength >= 0)
		return NULL;

	/*-----------------------------------------------------------------
	Check if a loaded block is available
	-----------------------------------------------------------------*/
	m_mtxLockStream.P ();
	while (
		m_nTakenStart >= m_nBufferSize ||
		m_nTakenStart == m_nFreeStart && m_nAllocatedLength >= 0
	)
	{
		/*-----------------------------------------------------------------
		If end of stream, return END_OF_STREAM
		-----------------------------------------------------------------*/
		if (m_bEndOfStream)
		{	m_mtxLockStream.V ();
			return END_OF_STREAM;
		}
		/*-----------------------------------------------------------------
		If necessary, wait for a block, otherwise return NULL
		-----------------------------------------------------------------*/
		if (bWait)
		{	m_bWaitingForData = TRUE;
			m_mtxLockStream.V ();
			m_smpWaitForData.P ();
			if (m_bEndOfStream)
				return END_OF_STREAM;
			m_mtxLockStream.P ();
		}
		else
		{	m_mtxLockStream.V ();
			return NULL;
		}
	}
	
	/*-----------------------------------------------------------------
	Lock a block and return its address, size and time stamp
	-----------------------------------------------------------------*/
	m_smpLockOutBlock.P ();
	m_nLockedLength = GetBlockLength (m_nTakenStart);
	rnSize = m_nLockedLength - sizeof (int) - sizeof (DTime);
	rdtTime = GetTimeStamp (m_nTakenStart);
	m_mtxLockStream.V ();
	return m_pBuffer + sizeof (int) + sizeof (DTime) + m_nTakenStart;
}

/*-----------------------------------------------------------------
Fetch() fetches a block whose time stamp matured.
It will return TRUE rbRecurrent if it's the same block returned on
previous call.
If bSkip is TRUE it will skip all blocks till the newest one whose time
matches.
If bWait is TRUE it will wait till one block matures.
-----------------------------------------------------------------*/
LPVOID MediaStream :: Fetch (
	int &rnSize,
	DTime &rdtTime,
	BOOL &rbRecurrent,
	BOOL bSkip,
	BOOL bWait,
	DTime dtDelta
)
{
	/*-----------------------------------------------------------------
	Returns NULL if a block is already locked
	-----------------------------------------------------------------*/
	if (m_nLockedLength >= 0)
		return NULL;

	/*-----------------------------------------------------------------
	If first block has been already retrieved and bWait is true, discard
	that block.
	-----------------------------------------------------------------*/
	m_mtxLockStream.P ();
	if (m_bFirstRetrieved && bWait)
		Discard ();

	/*-----------------------------------------------------------------
	Check if a loaded block is available
	-----------------------------------------------------------------*/
	while (
		m_nTakenStart >= m_nBufferSize ||
		m_nTakenStart == m_nFreeStart && m_nAllocatedLength >= 0
	)
	{
		/*-----------------------------------------------------------------
		If end of stream, return END_OF_STREAM
		-----------------------------------------------------------------*/
		if (m_bEndOfStream)
		{	m_mtxLockStream.V ();
			return END_OF_STREAM;
		}
		/*-----------------------------------------------------------------
		If necessary, wait for a block, otherwise return NULL
		-----------------------------------------------------------------*/
		if (bWait)
		{	m_bWaitingForData = TRUE;
			m_mtxLockStream.V ();
			m_smpWaitForData.P ();
			if (m_bEndOfStream)
				return END_OF_STREAM;
			m_mtxLockStream.P ();
		}
		else
		{	m_mtxLockStream.V ();
			return NULL;
		}
	}

	/*-----------------------------------------------------------------
	Find the next block which matured. If bSkip is TRUE, find the newest
	matured block.
	-----------------------------------------------------------------*/
	int nNext = m_nTakenStart;
	int i = nNext;
	DTime dtTime;
	DTime dtCurrentTime = m_pClock -> GetTime () + dtDelta;

	if (m_bFirstRetrieved)
		i = GetNext (nNext);
	while (
		i >= 0 &&
		(dtTime = GetTimeStamp (i)) != 0 &&
		m_pClock -> CompareTimes (dtCurrentTime, dtTime) >= 0
	)
	{	nNext = i;
		if (!bSkip)
			break;
		i = GetNext (nNext);
	}

	/*-----------------------------------------------------------------
	Now nNext points to the most suitable block.
	If it is a matured block, lock it and return its address.
	Otherwise wait or return NULL
	-----------------------------------------------------------------*/
	dtTime = GetTimeStamp (nNext);
	if (m_pClock -> CompareTimes (dtTime, dtCurrentTime) > 0)
	{	m_mtxLockStream.V ();
		if (bWait)
		{	m_pClock -> WaitOn (dtTime, &m_evtWait);
			m_mtxLockStream.P ();
		}
		else
			return NULL;
	}

	/*-----------------------------------------------------------------
	Now we can lock the block and return all the required information
	-----------------------------------------------------------------*/
	m_smpLockOutBlock.P ();
	m_nLockedLength = GetBlockLength (nNext);
	rnSize = m_nLockedLength - sizeof (int) - sizeof (DTime);
	rdtTime = GetTimeStamp (m_nTakenStart);
	rbRecurrent = nNext == m_nTakenStart && m_bFirstRetrieved;
	m_bFirstRetrieved = TRUE;

	/*-----------------------------------------------------------------
	Discard old blocks
	-----------------------------------------------------------------*/
	if (nNext != m_nTakenStart)
	{	m_nTakenStart = nNext;
		if (m_bWaitingForMemory)
		{	m_bWaitingForMemory = FALSE;
			m_smpWaitForMemory.V ();
		}
	}			
	m_mtxLockStream.V ();
	return m_pBuffer + sizeof (int) + sizeof (DTime) + m_nTakenStart;
}

/*-----------------------------------------------------------------
Releases a previously locked output block. If bDiscard is TRUE, or its
time stamp is zero, discard the block from the stream
-----------------------------------------------------------------*/
void MediaStream :: Release (BOOL bDiscard)
{
	/*-----------------------------------------------------------------
	Do nothing if no block is locked
	-----------------------------------------------------------------*/
	if (m_nLockedLength < 0)
		return;

	m_mtxLockStream.P ();
	m_nLockedLength = -1;
	m_smpLockOutBlock.V ();
	if (bDiscard)
		Discard ();
	m_mtxLockStream.V ();
}

/*-----------------------------------------------------------------
Mark that input side of the stream is closes.
-----------------------------------------------------------------*/
void MediaStream :: Close ()
{
	m_mtxLockStream.P ();
	m_bEndOfStream = TRUE;
	if (m_bWaitingForData)
	{	m_bWaitingForData = FALSE;
		m_smpWaitForData.V ();
	}
	m_mtxLockStream.V ();
}
	
/*-----------------------------------------------------------------
Discard next block
-----------------------------------------------------------------*/
void MediaStream :: Discard ()
{
	m_nTakenStart += GetBlockLength (m_nTakenStart);
	if (m_nTakenStart >= m_nBufferSize ||
		m_nTakenStart != m_nFreeStart &&
		GetBlockLength (m_nTakenStart) == -1
	)
		m_nTakenStart = 0;

	/*-----------------------------------------------------------------
	If the buffer is now empty and no block is currently
	allocated, we can reset the TakenStart and FreeStart values
	-----------------------------------------------------------------*/
	if (m_nTakenStart == m_nFreeStart && m_nAllocatedLength < 0)
	{	m_nFreeStart = 0;
		m_nTakenStart = m_nBufferSize;
	}
	m_bFirstRetrieved = FALSE;
	if (m_bWaitingForMemory)
	{	m_bWaitingForMemory = FALSE;
		m_smpWaitForMemory.V ();
	}
}

/*-----------------------------------------------------------------
Find the offset of the block following the one whose offset is given
Returns -1 if there are no more available blocks.
-----------------------------------------------------------------*/
int MediaStream :: GetNext (int nOffset)
{
	int i = GetBlockLength (nOffset);

	if (i == -1)
		nOffset = 0;
	else
		nOffset += i;
	if (nOffset >= m_nBufferSize)
		nOffset = 0;
	if (nOffset == m_nFreeStart)
		return -1;
	if (GetBlockLength (nOffset) == -1)
		nOffset = 0;
	if (nOffset == m_nFreeStart)
		return -1;
	return nOffset;
}

BOOL MediaStream :: SetBufEmpty()
{
	if((m_nAllocatedLength < 0) && (m_nLockedLength < 0))
	{
		Reset ();
		return TRUE;
	}
	return FALSE;
}
void MediaStream :: UnlockWait(BOOL bReset)
{
	if(!bReset)
		m_evtWait.V();
	else m_evtWait.Reset();
}
BOOL MediaStream :: IsBufferEmpty()
{
	if((m_nFreeStart - m_nTakenStart) == 0)
		return TRUE;
	if(((m_nBufferSize - m_nTakenStart) + m_nFreeStart) == 0)
		return TRUE;
	return FALSE;
}
int MediaStream :: GetBufferRate()
{
	int nRet = 0;
	if(m_nTakenStart >= m_nFreeStart)
		 nRet = ((m_nBufferSize - m_nTakenStart) + m_nFreeStart) * 100 / m_nBufferSize;
	else nRet = (m_nFreeStart - m_nTakenStart) * 100 / m_nBufferSize;
	return nRet;
}
int MediaStream :: GetBufferSize()
{
	return m_nBufferSize;
}
