#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
using namespace std;

/*****************************************************************************
 *
 *  Variable type
 *
 ****************************************************************************/

typedef unsigned int	 uint	;
typedef char			 int8	;
typedef unsigned char	 uint8	;
typedef short			 int16	;
typedef unsigned short	 uint16	;
typedef int				 int32	;
typedef unsigned int	 uint32	;
typedef __int64			 int64	;
typedef unsigned __int64 uint64	;
typedef signed __int64	 DTime	;

/*****************************************************************************
 *
 *  Time typedefs
 *
 ****************************************************************************/

typedef TIME_ZONE_INFORMATION TZone;
struct LTime
{
	SYSTEMTIME stm;
	BOOL bApplyDst;

	LTime()
	{
		Default();
	}
	inline LTime& operator = (LTime& other)
	{
		memcpy(this, &other, sizeof(LTime));
		return *this;
	}
	inline BOOL operator == (LTime& other)
	{
		if(this->stm.wYear			!= other.stm.wYear)			return FALSE;
		if(this->stm.wMonth			!= other.stm.wMonth)		return FALSE;
		if(this->stm.wDay			!= other.stm.wDay)			return FALSE;
		if(this->stm.wHour			!= other.stm.wHour)			return FALSE;
		if(this->stm.wMinute		!= other.stm.wMinute)		return FALSE;
		if(this->stm.wSecond		!= other.stm.wSecond)		return FALSE;
		if(this->stm.wMilliseconds	!= other.stm.wMilliseconds)	return FALSE;
		if(this->bApplyDst			!= other.bApplyDst)			return FALSE;
		return TRUE;
	}
	inline BOOL operator != (LTime& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}

	inline void Default()
	{
		memset(&(this->stm), 0, sizeof(SYSTEMTIME));
		this->bApplyDst = FALSE;
	}
	inline void SetDateFormat()
	{
		this->stm.wDayOfWeek	= 0;
		this->stm.wHour			= 0;
		this->stm.wMinute		= 0;
		this->stm.wSecond		= 0;
		this->stm.wMilliseconds	= 0;
		this->bApplyDst			= 0;
	}
	inline BOOL IsDate(LTime* plt = NULL)
	{
		if(!plt)
		{
			if(this->stm.wYear	== 0) return FALSE;
			if(this->stm.wMonth	== 0) return FALSE;
			if(this->stm.wDay	== 0) return FALSE;
		}
		else
		{
			if(this->stm.wYear	!= plt->stm.wYear)	return FALSE;
			if(this->stm.wMonth	!= plt->stm.wMonth)	return FALSE;
			if(this->stm.wDay	!= plt->stm.wDay)	return FALSE;
		}
		return TRUE;
	}
};

const DTime _DTIME_SECOND	= 1000;
const DTime _DTIME_MINUTE	= 60 * 1000;
const DTime _DTIME_HOUR		= 60 * 60 * 1000;
const DTime _DTIME_DAY		= (DTime)24 * 60 * 60 * 1000;

/*****************************************************************************
 *
 *  Sound & Image Source
 *
 ****************************************************************************/

// Audio Source
struct AUD_SNDSRC
{
	uint32	channel;
	uint32	samples;
	uint32	databit;
	uint8*	aud_ptr;
	uint32	audbytes;

	AUD_SNDSRC()
	{
		this->channel = 1;
		this->samples = 0;
		this->databit = 0;
		this->aud_ptr = NULL;
		this->audbytes = 0;
	}
};

// Video Source
struct VID_IMGSRC
{
	uint32	f_col;
	int		hsize;
	int		vsize;
	uint8*	x_ptr;
	int		x_stride;
	uint8*	y_src;
	uint8*	u_src;
	uint8*	v_src;
	int		y_stride;
	int		uv_stride;

	VID_IMGSRC()
	{
		Default();
	}
	inline void Default()
	{
		this->f_col = 0;
		this->hsize = 0;
		this->vsize = 0;
		this->x_ptr = NULL;
		this->x_stride = 0;
		this->y_src = NULL;
		this->u_src = NULL;
		this->v_src = NULL;
		this->y_stride = 0;
		this->uv_stride = 0;
	}
	inline BOOL IsImage()
	{
		if(this->f_col)
		{
			if(this->y_src && this->u_src && this->v_src)
				return TRUE;
			if(this->x_ptr && this->x_stride)
				return TRUE;
		}
		return FALSE;
	}
};

/*****************************************************************************
 *
 *  Audio/Video Data Packet
 *
 ****************************************************************************/

 // Max Stream Buffer Size
#define MAX_BUF_SIZE 4194304

// Max Audio/Video Size
#define MAX_AUDIO_SIZE 32000
#define MAX_VIDEO_SIZE MAX_BUF_SIZE

// Max Screen
#define MAX_SCH 64

struct AVS_HDR
{
	int	iType;
	int	iSize;
	int	iInfo[4];

	AVS_HDR()
	{
		memset(this, 0, sizeof(AVS_HDR));
	}
	inline AVS_HDR& operator = (AVS_HDR& other)
	{
		memcpy(this, &other, sizeof(AVS_HDR));
		return *this;
	}
};
struct AVS_PACKET
{
	AVS_HDR	AVS_hdr_t;
	uint8	Payload[MAX_BUF_SIZE];

	inline AVS_PACKET& operator = (AVS_PACKET& other)
	{
		memcpy(this->Payload, other.Payload, other.AVS_hdr_t.iSize);
		this->AVS_hdr_t = other.AVS_hdr_t;
		return *this;
	}
	inline void Default()
	{
		memset(&this->AVS_hdr_t, 0, sizeof(AVS_HDR));
	}

	inline void SetType(int iType)
	{
		this->AVS_hdr_t.iType = iType;
	}
	inline void SetSize(int iSize)
	{
		this->AVS_hdr_t.iSize = iSize;
	}
	inline void SetInfo(int i, int iInfo)
	{
		this->AVS_hdr_t.iInfo[i] = iInfo;
	}

	inline int GetType()
	{
		return this->AVS_hdr_t.iType;
	}
	inline int GetSize()
	{
		return this->AVS_hdr_t.iSize;
	}
	inline int GetInfo(int i)
	{
		return this->AVS_hdr_t.iInfo[i];
	}
	inline int GetPacketLen()
	{
		return (sizeof(AVS_HDR) + this->AVS_hdr_t.iSize);
	}
};


#define CD_NONE		0
#define CD_AUDIO	1
#define CD_VIDEO	2
#define CD_METAD	3
#define CD_OBJROI	4
#define I_TYPE		1
#define P_TYPE		2
#define B_TYPE		4

static const uint32 FOURCC_YV12 = MAKEFOURCC('Y','V','1','2');
static const uint32 FOURCC_BGRA = MAKEFOURCC('B','G','R','A');
static const uint32 FOURCC_BGR24 = MAKEFOURCC('B','G','R',24);

#define SAFE_RELEASE(c)	if(c!=NULL) {c->Release();c=NULL;}
#define SAFE_DELETEA(c)	if(c!=NULL) {delete [] c;c=NULL;}
#define SAFE_DELETE(c)	if(c!=NULL) {delete c;c=NULL;}
#define SAFE_CLOSE(c)	if(c!=NULL) {c->Close();delete c;c=NULL;}
#define SAFE_TERMINATE(h) \
if(h!=NULL) \
{ \
	DWORD ExitCode = 0; \
	if(::GetExitCodeThread(h,&ExitCode)) \
	{ \
		TerminateThread(h,ExitCode); \
		CloseHandle(h); \
	} \
	h=NULL; \
}

#endif // _COMMON_H_
