#ifndef _DXS_CONTROL_H_
#define _DXS_CONTROL_H_

#include "Dxs3D9.h"

class dxs_control
{
private:
	CWnd * m_pWnd;
	Dxs3D9 * m_pD3d;

	BOOL m_bActive;

public:
	BOOL Init(CWnd * pWnd);
	void Exit();
	CWnd* GetWnd(){return m_pWnd;}
	BOOL DxCheck();
	void* GetD3DD();

	DX_SCREEN* GetDxScrn();
	CRect GetScreen();

	int GetStatus();

	void ScreenSplit();
	void ScreenCover();
	void ChangeSplit();

	void SetSelCh(BOOL bSelCh); BOOL GetSelCh();
	void SetAudCh(BOOL bAudCh); BOOL GetAudCh();
	void SetMicCh(BOOL bMicCh); BOOL GetMicCh();
	void SetText(WCHAR* szText); WCHAR* GetText();

	DX_PT_RECT* GetPt_Rect();
	BOOL GetPt_Rect(DX_PT_RECT* pDxRect);
	void SetPt_Rect(DX_PT_RECT* pDxRect);

	void SendImageSrc(VID_IMGSRC* pVid_img);
	void SendCleanMsg(int STATUS);
	void Send_Refresh();

public:
	dxs_control();
	~dxs_control();
};

#endif
