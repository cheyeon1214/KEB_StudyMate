/*-------------------------------------------------------------------------
Project Im1Player
INTERNATIONAL ORGANIZATION FOR STANDARDIZATION
ISO/IEC JTC1/SC29/WG11

SUBSYSTEM:  Buffer management
FILE:       Clock.cpp
AUTHOR:     Zvi Lifshitz (VDOnet Corp.)
OVERVIEW:   Implementation of ClockReference.
-------------------------------------------------------------------------*/
#include "pch.h"
#include "Clock.h"

/*-----------------------------------------------------------------
ClockReference constructor. Save current time and granularity
-----------------------------------------------------------------*/
ClockReference :: ClockReference (DTime dtGranularity)
{
	m_dtGranularity = dtGranularity;
	Reset ();
}

/*-----------------------------------------------------------------
Reset the clock to zero (but don't start it running)
-----------------------------------------------------------------*/
void ClockReference :: Reset ()
{
	m_bStarted = FALSE;
	m_bOffset = FALSE;
	m_dtInitTime = 0;
	m_dtOffset = 0;
}

/*-----------------------------------------------------------------
Start the clock running
-----------------------------------------------------------------*/
BOOL ClockReference :: IsStart ()
{
	return m_bStarted;
}
BOOL ClockReference :: IsOffset ()
{
	return m_bOffset;
}
DTime ClockReference :: GetInitTime ()
{
	return m_dtInitTime;
}
void ClockReference :: Start (DTime dtInit, DTime dtBuff)
{
	m_dtInitTime = dtInit;
	m_dtOffset = (DTime)(timeGetTime ()) + dtBuff;
	m_bStarted = TRUE;
	m_bOffset = dtBuff ? TRUE : FALSE;
}

/*-----------------------------------------------------------------
Return current value of the object
-----------------------------------------------------------------*/
DTime ClockReference :: GetTime ()
{
	return (m_dtInitTime + ((DTime)(timeGetTime ()) - m_dtOffset));
}

/*-----------------------------------------------------------------
Wait till the object reaches the given time
-----------------------------------------------------------------*/
void ClockReference ::WaitOn (DTime dtTime, ZEvent* pWait)
{
	DTime lDelta = dtTime - GetTime ();
	if(lDelta > 0)
	{
		if(lDelta > 5000)
			pWait->P(5000);
		else pWait->P((UINT)lDelta);
	}
}

/*-----------------------------------------------------------------
Compare two time stamps
-----------------------------------------------------------------*/
DTime ClockReference :: CompareTimes (DTime dtTime, DTime dtCompareTo)
{
	DTime dtResult;

	if (dtCompareTo == 0)
		dtCompareTo = GetTime ();
	dtResult = dtTime - dtCompareTo;
	if(_abs64(dtResult) <= m_dtGranularity)
		dtResult = 0;
	return dtResult;
}
