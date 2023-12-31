// Dxs3D9.h: interface for the Dxs3D9 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _DXS_3D9_H_
#define  _DXS_3D9_H_

#include "Dx3D9.h"

class Dxs3D9 : Dx3D9
{
private:
	DX_SCREEN m_dxScrn;

	ZCriticalSection m_csDraw;

private:
	OffSurfCtrl m_Offscrn;

	BOOL DrawSCHSurface(LPRECT lpDst, LPRECT lpSrc);
	BOOL DrawINFSurface(LPRECT lpDst);

	void RenderSurface(LPRECT lpDst, LPRECT lpSrc);

public:
	WCHAR* Init(CWnd* pWnd);
	void Shutdown();
	BOOL DxCheck();
	void* GetD3DD();

	void SendImageSrc(VID_IMGSRC* pVid_img);
	void SendCleanMsg(int STATUS);
	void Send_Refresh();

	DX_SCREEN* GetDxScrn();
	void SetPt_Rect(DX_PT_RECT* pDxRect);

	void ScreenSplit();
	void ClearSurface();

	Dxs3D9::Dxs3D9();
	Dxs3D9::~Dxs3D9();
};

#endif // _DXS3D9_H_
