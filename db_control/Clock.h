#if !defined(_clock_h)	// Sentry, use file only if it's not already included.
#define _clock_h

/*-------------------------------------------------------------------------
Project Im1Player
INTERNATIONAL ORGANIZATION FOR STANDARDIZATION
ISO/IEC JTC1/SC29/WG11

SUBSYSTEM:  Buffer management
FILE:       Clock.h
AUTHOR:     Zvi Lifshitz (VDOnet Corp.)
OVERVIEW:   Definition of ClockReference.
-------------------------------------------------------------------------*/

class ClockReference
{
	DTime m_dtInitTime;
	DTime m_dtOffset;
	DTime m_dtGranularity;
	BOOL m_bStarted;
	BOOL m_bOffset;

public:
	ClockReference (DTime dtGranularity = 10);
	void Reset ();
	BOOL IsStart ();
	BOOL IsOffset ();
	DTime GetInitTime ();
	void Start (DTime dtInit, DTime dtBuff = 1000);
	DTime GetTime ();
	void WaitOn (DTime dtTime, ZEvent* pWait);
	DTime CompareTimes (DTime dtTime, DTime dtCompareTo = 0);
};

#endif                                      // _clock_h sentry.
