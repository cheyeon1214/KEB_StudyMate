#ifndef _DXM_CONTROL_H_
#define _DXM_CONTROL_H_

#include "Dxm3D9.h"
#include "DxSound.h"

class dxm_control
{
private:
	CWnd * m_pWnd;
	Dxm3D9 * m_pD3d;
	DxSound * m_pSnd;

	BOOL m_bActive;
	int  m_UseScrn;

private:
	DX_Config m_dxCfg;
	CRect VideoScreen[MAX_SCR][MAX_SCH][MAX_SCH];

	int m_SelCh;
	BOOL m_bBufInit;

public:
	void LoadScreen();
	DX_SCREEN* GetDxScrn(int nCh);
	CRect GetScreen(int nCh, BOOL bScale);

public:
	BOOL Init(CWnd * pWnd);
	void Exit();
	CWnd* GetWnd(){return m_pWnd;}
	BOOL DxCheck();
	void* GetD3DD();

	int GetStatus(int nCh);

	void SetSwapCh(int nCh1, int nCh2);

	void ScreenSplit();
	void ScreenCover();
	void ChangeSplit();

	BOOL SetDxCfg(DX_Config * pDxCfg);
	DX_Config* GetDxCfg();

	BOOL SetDivMode(uint uiMsg, int nValue1 = 0, int nValue2 = 0);
	int GetSelDiv();
	int GetSelRot();

	void SetSelCh(int nCh, BOOL bSelCh); int GetSelCh();
	void SetAudCh(int nCh, BOOL bAudCh); BOOL GetAudCh(int nCh);
	void SetMicCh(int nCh, BOOL bMicCh); BOOL GetMicCh(int nCh);
	void SetText(int nCh, WCHAR* szText); WCHAR* GetText(int nCh);

	DX_PT_RECT* GetPt_Rect(int nCh);
	BOOL GetPt_Rect(int nCh, DX_PT_RECT* pDxRect);
	void SetPt_Rect(int nCh, DX_PT_RECT* pDxRect);

	void SendImageSrc(int nCh, VID_IMGSRC* pVid_img);
	void SendCleanMsg(int nCh, int STATUS);
	void Send_Refresh(int nCh);
	void SendSoundSrc(int nCh, AUD_SNDSRC* pAud_snd);

public:
	dxm_control();
	~dxm_control();
};

#endif
