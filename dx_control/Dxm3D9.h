// Dxm3D9.h: interface for the Dxm3D9 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _DXM_3D9_H_
#define  _DXM_3D9_H_

#include "Dx3D9.h"

class Dxm3D9 : Dx3D9
{
private:
	DX_SCREEN m_dxScrn[MAX_SCH];

	ZCriticalSection m_csDraw;

private:
	OffSurfCtrl m_Offscrn[MAX_SCH];

	BOOL DrawSCHSurface(int nCh, LPRECT lpDst, LPRECT lpSrc);
	BOOL DrawINFSurface(int nCh, LPRECT lpDst);
	
	void RenderSurface(LPRECT lpDst, LPRECT lpSrc);

public:
	WCHAR* Init(CWnd* pWnd, WCHAR* szIniPath = NULL);
	void Shutdown();
	BOOL DxCheck();
	void* GetD3DD();

	void SendImageSrc(int nCh, LPRECT lpDst, VID_IMGSRC* pVid_img);
	void SendCleanMsg(int nCh, LPRECT lpDst, int STATUS);
	void Send_Refresh(int nCh, LPRECT lpDst);

	DX_SCREEN* GetDxScrn(int nCh);
	void SetPt_Rect(int nCh, DX_PT_RECT* pDxRect);

	void ScreenSplit();
	void ClearSurface();

	Dxm3D9::Dxm3D9();
	Dxm3D9::~Dxm3D9();
};

#endif // _DXM3D9_H_
