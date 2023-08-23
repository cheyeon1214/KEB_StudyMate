#ifndef _DX_CONTROL_DLL_H_
#define _DX_CONTROL_DLL_H_

#pragma once

#ifdef _API_STATIC_LIB_
	#define API_DX_CONTROL
#else
	#ifdef _DX_CONTROL_DLL_
		#define API_DX_CONTROL __declspec(dllexport)
	#else
		#define API_DX_CONTROL __declspec(dllimport)
		#ifdef _WIN64
			#ifdef _DEBUG
				#pragma comment(lib, "../x64/libd/dx_control.lib")
				#pragma message("Automatically linking with ../x64/libd/dx_control.lib")
			#else
				#pragma comment(lib, "../x64/lib/dx_control.lib")
				#pragma message("Automatically linking with ../x64/lib/dx_control.lib")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "../x86/libd/dx_control.lib")
				#pragma message("Automatically linking with ../x86/libd/dx_control.lib")
			#else
				#pragma comment(lib, "../x86/lib/dx_control.lib")
				#pragma message("Automatically linking with ../x86/lib/dx_control.lib")
			#endif
		#endif
	#endif
#endif
#include "../include/common.h"

#define Cdx_control void

/*****************************************************************************
 *
 *  functions
 *
 ****************************************************************************/

inline CRect ScaleRect(LPRECT lpDat, LPRECT lpSrc, LPRECT lpDst)
{
	CRect re = *lpDat;
	double src_cx = (double)(lpSrc->right - lpSrc->left);
	double src_cy = (double)(lpSrc->bottom - lpSrc->top);
	double dst_cx = (double)(lpDst->right - lpDst->left);
	double dst_cy = (double)(lpDst->bottom - lpDst->top);
	if(src_cx && src_cy)
	{
		re.top = (int)((double)lpDat->top * dst_cy / src_cy + 0.5);
		re.left = (int)((double)lpDat->left * dst_cx / src_cx + 0.5);
		re.right = (int)((double)lpDat->right * dst_cx / src_cx + 0.5);
		re.bottom = (int)((double)lpDat->bottom * dst_cy / src_cy + 0.5);
	}
	return re;
}

/*****************************************************************************
 *
 *  Draw Point Rect
 *
 ****************************************************************************/

struct RtObj
{
	CRect re;
	WCHAR sz[64];
	COLORREF cr;
	bool b_fill;
	bool b_mosaic;
	uint8 alp_fill;

	RtObj(RtObj* pRt)
	{
		this->re = pRt->re;
		wcscpy_s(this->sz, pRt->sz);
		this->cr = pRt->cr;
		this->b_fill = pRt->b_fill;
		this->b_mosaic = pRt->b_mosaic;
		this->alp_fill = pRt->alp_fill;
	}
	RtObj(LPRECT lp)
	{
		this->re = CRect(lp);
		*(this->sz) = L'\0';
		this->cr = RGB(0,255,0);
		this->b_fill = false;
		this->b_mosaic = false;
		this->alp_fill = 127;
	}
	RtObj(int x1, int y1, int x2, int y2)
	{
		this->re = CRect(x1, y1, x2, y2);
		*(this->sz) = L'\0';
		this->cr = RGB(0,255,0);
		this->b_fill = false;
		this->b_mosaic = false;
		this->alp_fill = 127;
	}
	inline RtObj& operator = (RtObj& other)
	{
		this->re = other.re;
		wcscpy_s(this->sz, other.sz);
		this->cr = other.cr;
		this->b_fill = other.b_fill;
		this->b_mosaic = other.b_mosaic;
		this->alp_fill = other.alp_fill;
		return *this;
	}
	inline BOOL operator == (RtObj& other)
	{
		if(this->re != other.re) return FALSE;
		if(wcscmp(this->sz, other.sz) != 0) return FALSE;
		if(this->cr != other.cr) return FALSE;
		if(this->b_fill != other.b_fill) return FALSE;
		if(this->b_mosaic != other.b_mosaic) return FALSE;
		if(this->alp_fill != other.alp_fill) return FALSE;
		return TRUE;
	}
	inline BOOL operator != (RtObj& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}

	inline BOOL SetScale(LPRECT lpSrc, LPRECT lpDst)
	{
		this->re = ScaleRect(&this->re, lpSrc, lpDst);
		this->re.OffsetRect(lpDst->left, lpDst->top);
		this->re &= CRect(lpDst);

		if(!this->re.IsRectEmpty())
			return TRUE;
		return FALSE;
	}
};
struct DX_PT_RECT
{
	CRect Ref;

	vector<RtObj*> vec_Rt;

	DX_PT_RECT()
	{
		this->Ref.top = 0;
		this->Ref.left = 0;
		this->Ref.right = 0;
		this->Ref.bottom = 0;
	}
	DX_PT_RECT(int cx, int cy)
	{
		this->Ref.top = 0;
		this->Ref.left = 0;
		this->Ref.right = cx;
		this->Ref.bottom = cy;
	}
	~DX_PT_RECT()
	{
		Clear();
	}
	inline int Cnt()
	{
		return (int)vec_Rt.size();
	}
	inline RtObj* Obj(int i)
	{
		if(i < (int)vec_Rt.size())
		{
			RtObj* pRt = vec_Rt[i];
			return pRt;
		}
		return NULL;
	}
	inline DX_PT_RECT& operator = (DX_PT_RECT& other)
	{
		Clear();

		this->Ref = other.Ref;

		for(int i=0; i<other.Cnt(); i++)
		{
			Insert(other.Obj(i));
		}
		return *this;
	}
	inline BOOL operator == (DX_PT_RECT& other)
	{
		if(this->Ref != other.Ref)
			return FALSE;

		if((int)vec_Rt.size() != other.Cnt())
			return FALSE;
		for(int i=0; i<(int)vec_Rt.size(); i++)
		{
			RtObj* pRt = vec_Rt[i];
			if(pRt && other.Obj(i))
			{
				if(*pRt != *(other.Obj(i)))
					return FALSE;
			}
			else return FALSE;
		}
		return TRUE;
	}
	inline BOOL operator != (DX_PT_RECT& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}

	inline void Clear()
	{
		for(auto pRt : vec_Rt)
		{
			SAFE_DELETE(pRt);
		}
		vec_Rt.clear();
	}
	inline void Insert(RtObj* pRt)
	{
		RtObj* pNew = new RtObj(pRt);
		vec_Rt.push_back(pNew);
	}
	inline void Delete(int i)
	{
		if(i < (int)vec_Rt.size())
		{
			RtObj* pRt = vec_Rt[i];
			SAFE_DELETE(pRt);

			vec_Rt.erase(vec_Rt.begin() + i);
		}
	}
	inline BOOL IsRtObj()
	{
		if(this->Ref.IsRectEmpty())
			return FALSE;
		if(!vec_Rt.size()) return FALSE;

		return TRUE;
	}
};

/*****************************************************************************
 *
 *  DX_SCREEN
 *
 ****************************************************************************/

struct DX_SCREEN
{
	void* DxSurf;
	int H_size;
	int V_size;
	int Status;
	int Is_Sel;
	int Is_Aud;
	int Is_Mic;
	WCHAR szText[64];
	DX_PT_RECT dx_Rect;

	DX_SCREEN()
	{
		this->Is_Sel = 0;
		Default();
	}
	inline void Default()
	{
		this->DxSurf = NULL;
		this->H_size = 0;
		this->V_size = 0;
		this->Status = 0;
		this->Is_Aud = 0;
		this->Is_Mic = 0;
		*this->szText = L'\0';
		this->dx_Rect.Clear();
	}

	inline CRect GetRect()
	{
		CRect reRet(0, 0, this->H_size, this->V_size);
		return reRet;
	}
	inline void SetPt_Rect(DX_PT_RECT* pDxRect)
	{
		if(pDxRect)
			this->dx_Rect = *pDxRect;
		else this->dx_Rect.Clear();
	}
};

/*****************************************************************************
 *
 *  dx_single
 *
 ****************************************************************************/

API_DX_CONTROL Cdx_control* _dxs_Init(CWnd* pWnd);
API_DX_CONTROL void _dxs_Exit(void* Ptr);
API_DX_CONTROL CWnd* _dxs_GetWnd(void* Ptr);
API_DX_CONTROL BOOL _dxs_DxCheck(void* Ptr);
API_DX_CONTROL void* _dxs_GetD3DD(void* Ptr);

API_DX_CONTROL void* _dxs_GetDxScrn(void* Ptr);
API_DX_CONTROL CRect _dxs_GetScreen(void* Ptr);

API_DX_CONTROL int _dxs_GetStatus(void* Ptr);

API_DX_CONTROL void _dxs_ScreenSplit(void* Ptr);
API_DX_CONTROL void _dxs_ScreenCover(void* Ptr);
API_DX_CONTROL void _dxs_ChangeSplit(void* Ptr);

API_DX_CONTROL void _dxs_SetSelCh(void* Ptr, BOOL bSelCh);
API_DX_CONTROL BOOL _dxs_GetSelCh(void* Ptr);
API_DX_CONTROL void _dxs_SetAudCh(void* Ptr, BOOL bAudCh);
API_DX_CONTROL BOOL _dxs_GetAudCh(void* Ptr);
API_DX_CONTROL void _dxs_SetMicCh(void* Ptr, BOOL bMicCh);
API_DX_CONTROL BOOL _dxs_GetMicCh(void* Ptr);
API_DX_CONTROL void _dxs_SetText(void* Ptr, WCHAR* szText);
API_DX_CONTROL WCHAR* _dxs_GetText(void* Ptr);

API_DX_CONTROL DX_PT_RECT* _dxs_GetPt_Rect(void* Ptr);
API_DX_CONTROL BOOL _dxs_GetPt_Rect(void* Ptr, DX_PT_RECT* pDxRect);
API_DX_CONTROL void _dxs_SetPt_Rect(void* Ptr, DX_PT_RECT* pDxRect);

API_DX_CONTROL void _dxs_SendImageSrc(void* Ptr, VID_IMGSRC* pVid_img);
API_DX_CONTROL void _dxs_SendCleanMsg(void* Ptr, int STATUS);
API_DX_CONTROL void _dxs_Send_Refresh(void* Ptr);

/*****************************************************************************
 *
 *  dx_multi
 *
 ****************************************************************************/

#define MAX_IMG			4
#define SCR_IMG_BLK		0
#define SCR_IMG_LOGO	1
#define SCR_IMG_RECON	2
#define SCR_IMG_DISCON	3
#define SCR_CAM_CONNECT	4

enum DivModeMsg
{
	MSG_MULTISCREEN = 0,
	MSG_SELVCHANNEL,
	MSG_SELROTATION,
	MSG_SET_REFRESH,
};

enum DivMode
{
	SCR_NXN_01 = 0,
	SCR_1X2_02,
	SCR_NXN_04,
	SCR_1XN_06,
	SCR_NXN_09,
	SCR_2XN_10,
	SCR_1XN_13,
	SCR_NXN_16,
	MAX_SCR,
};
const int DivCamNum[MAX_SCR] = {1,2,4,6,9,10,13,16};

inline int _UseSch()
{
	return 16;
}
inline int _UseScr()
{
	int USE_SCR = 0;
	for(int div=0; div<MAX_SCR; div++)
	{
		USE_SCR++;
		if((DivCamNum[div]/_UseSch()) > 0)
		{
			if(div == (MAX_SCR-1))
				break;
			else if(DivCamNum[div] < DivCamNum[div+1])
				break;
		}
	}
	return USE_SCR;
}

struct DX_Config
{
	int S_Mode;
	int S_Rotate;
	int S_Ch[MAX_SCH];

	DX_Config()
	{
		Default();
	}
	~DX_Config()
	{
	}
	inline DX_Config& operator = (DX_Config& other)
	{
		this->S_Mode = other.S_Mode;
		this->S_Rotate = other.S_Rotate;
		for(int ch=0; ch<_UseSch(); ch++)
			this->S_Ch[ch] = other.S_Ch[ch];
		return *this;
	}
	inline BOOL operator == (DX_Config& other)
	{
		if(this->S_Mode != other.S_Mode)
			return FALSE;
		if(this->S_Rotate != other.S_Rotate)
			return FALSE;
		for(int ch=0; ch<_UseSch(); ch++)
			if(this->S_Ch[ch] != other.S_Ch[ch])
				return FALSE;
		return TRUE;
	}
	inline BOOL operator != (DX_Config& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}

	inline void Default()
	{
		this->S_Mode = SCR_NXN_16;
		this->S_Rotate = 0;
		for(int ch=0; ch<MAX_SCH; ch++)
			this->S_Ch[ch] = ch;
	}
};

API_DX_CONTROL Cdx_control* _dxm_Init(CWnd * pWnd);
API_DX_CONTROL void _dxm_Exit(void* Ptr);
API_DX_CONTROL CWnd* _dxm_GetWnd(void* Ptr);
API_DX_CONTROL BOOL _dxm_DxCheck(void* Ptr);
API_DX_CONTROL void* _dxm_GetD3DD(void* Ptr);

API_DX_CONTROL void* _dxm_GetDxScrn(void* Ptr, int nCh);
API_DX_CONTROL CRect _dxm_GetScreen(void* Ptr, int nCh);

API_DX_CONTROL int _dxm_GetStatus(void* Ptr, int nCh);
API_DX_CONTROL void _dxm_SetSwapCh(void* Ptr, int nCh1, int nCh2);

API_DX_CONTROL void _dxm_ScreenSplit(void* Ptr);
API_DX_CONTROL void _dxm_ScreenCover(void* Ptr);
API_DX_CONTROL void _dxm_ChangeSplit(void* Ptr);

API_DX_CONTROL BOOL _dxm_SetDxCfg(void* Ptr, void* pDxCfg);
API_DX_CONTROL void* _dxm_GetDxCfg(void* Ptr);

API_DX_CONTROL BOOL _dxm_SetDivMode(void* Ptr, uint uiMsg, int nValue1 = 0, int nValue2 = 0);
API_DX_CONTROL int _dxm_GetSelDiv(void* Ptr);
API_DX_CONTROL int _dxm_GetSelRot(void* Ptr);

API_DX_CONTROL void _dxm_SetSelCh(void* Ptr, int nCh, BOOL bSelCh);
API_DX_CONTROL int _dxm_GetSelCh(void* Ptr);
API_DX_CONTROL void _dxm_SetAudCh(void* Ptr, int nCh, BOOL bAudCh);
API_DX_CONTROL BOOL _dxm_GetAudCh(void* Ptr, int nCh);
API_DX_CONTROL void _dxm_SetMicCh(void* Ptr, int nCh, BOOL bMicCh);
API_DX_CONTROL BOOL _dxm_GetMicCh(void* Ptr, int nCh);
API_DX_CONTROL void _dxm_SetText(void* Ptr, int nCh, WCHAR* szText);
API_DX_CONTROL WCHAR* _dxm_GetText(void* Ptr, int nCh);

API_DX_CONTROL DX_PT_RECT* _dxm_GetPt_Rect(void* Ptr, int nCh);
API_DX_CONTROL BOOL _dxm_GetPt_Rect(void* Ptr, int nCh, DX_PT_RECT* pDxRect);
API_DX_CONTROL void _dxm_SetPt_Rect(void* Ptr, int nCh, DX_PT_RECT* pDxRect);

API_DX_CONTROL void _dxm_SendImageSrc(void* Ptr, int nCh, VID_IMGSRC* pVid_img);
API_DX_CONTROL void _dxm_SendCleanMsg(void* Ptr, int nCh, int STATUS);
API_DX_CONTROL void _dxm_Send_Refresh(void* Ptr, int nCh);
API_DX_CONTROL void _dxm_SendSoundSrc(void* Ptr, int nCh, AUD_SNDSRC* pAud_snd);

#endif
