// dxm_control_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "dxm_control_exe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*****************************************************************************
 *
 *  dxm_control
 *
 ****************************************************************************/
dxm_control::dxm_control()
{
	m_pWnd = NULL;
	m_pD3d = NULL;
	m_pSnd = NULL;

	m_bActive = FALSE;
	m_UseScrn = 0;

	m_SelCh = -1;
	m_bBufInit = FALSE;

	for(int div=0; div<MAX_SCR; div++)
	{
		for(int mode=0; mode<MAX_SCH; mode++)
			for(int sc=0; sc<MAX_SCH; sc++)
				VideoScreen[div][mode][sc].SetRectEmpty();
	}
}
dxm_control::~dxm_control()
{
}

BOOL dxm_control::Init(CWnd * pWnd)
{
	m_pWnd = pWnd;
	if(!m_pWnd->GetSafeHwnd()) return FALSE;

	if(m_pD3d==NULL)
	{
		m_pD3d = new Dxm3D9;
		WCHAR * errorString = m_pD3d->Init(pWnd);
		if(errorString)
		{
			_wchr_Trace(_T("Dxm3D9 : %s\r\n"), errorString);

			delete m_pD3d;
			m_pD3d = NULL;
		}
	}
	if(m_pSnd==NULL)
	{
		m_pSnd = new DxSound;
		WCHAR * errorString = m_pSnd->Init(pWnd);
		if(errorString)
		{
			_wchr_Trace(_T("DxSound : %s\r\n"), errorString);

			delete m_pSnd;
			m_pSnd = NULL;
		}
	}

	LoadScreen();

	m_bActive = TRUE;

	return TRUE;
}
void dxm_control::Exit()
{
	m_bActive = FALSE;

	if(m_pD3d != NULL)
	{
		delete m_pD3d;
		m_pD3d = NULL;
	}
	if(m_pSnd != NULL)
	{
		delete m_pSnd;
		m_pSnd = NULL;
	}
}
BOOL dxm_control::DxCheck()
{
	if(m_bActive)
	{
		if(m_pD3d){
			return m_pD3d->DxCheck();
		}
	}
	return TRUE;
}
void* dxm_control::GetD3DD()
{
	if(m_bActive)
	{
		if(m_pD3d){
			return m_pD3d->GetD3DD();
		}
	}
	return NULL;
}

void dxm_control::LoadScreen()
{
	m_UseScrn = _UseScr();

	CRect xyPos[MAX_SCH];
	for(int div = 0; div < m_UseScrn; div++)
	{
		switch(div)
		{
		case SCR_NXN_01:
			xyPos[ 0] = CRect(  0,   0,1280, 960);
			break;
		case SCR_1X2_02:
			xyPos[ 0] = CRect(   0,   0, 640, 960);
			xyPos[ 1] = CRect( 640,   0,1280, 960);
			break;
		case SCR_NXN_04:
			xyPos[ 0] = CRect(  0,   0, 640, 480);
			xyPos[ 1] = CRect(640,   0,1280, 480);
			xyPos[ 2] = CRect(  0, 480, 640, 960);
			xyPos[ 3] = CRect(640, 480,1280, 960);
			break;
		case SCR_1XN_06:
			xyPos[ 0] = CRect(   0,   0, 853, 640);
			xyPos[ 1] = CRect( 853,   0,1280, 320);
			xyPos[ 2] = CRect( 853, 320,1280, 640);
			xyPos[ 3] = CRect(   0, 640, 427, 960);
			xyPos[ 4] = CRect( 427, 640, 853, 960);
			xyPos[ 5] = CRect( 853, 640,1280, 960);
			break;
		case SCR_NXN_09:
			xyPos[ 0] = CRect(   0,   0, 427, 320);
			xyPos[ 1] = CRect( 427,   0, 853, 320);
			xyPos[ 2] = CRect( 853,   0,1280, 320);
			xyPos[ 3] = CRect(   0, 320, 427, 640);
			xyPos[ 4] = CRect( 427, 320, 853, 640);
			xyPos[ 5] = CRect( 853, 320,1280, 640);
			xyPos[ 6] = CRect(   0, 640, 427, 960);
			xyPos[ 7] = CRect( 427, 640, 853, 960);
			xyPos[ 8] = CRect( 853, 640,1280, 960);
			break;
		case SCR_2XN_10:
			xyPos[ 0] = CRect(   0,   0, 640, 480);
			xyPos[ 1] = CRect( 640,   0,1280, 480);
			xyPos[ 2] = CRect(   0, 480, 320, 720);
			xyPos[ 3] = CRect( 320, 480, 640, 720);
			xyPos[ 4] = CRect( 640, 480, 960, 720);
			xyPos[ 5] = CRect( 960, 480,1280, 720);
			xyPos[ 6] = CRect(   0, 720, 320, 960);
			xyPos[ 7] = CRect( 320, 720, 640, 960);
			xyPos[ 8] = CRect( 640, 720, 960, 960);
			xyPos[ 9] = CRect( 960, 720,1280, 960);
			break;
		case SCR_1XN_13:
			xyPos[ 0] = CRect( 320, 240, 960, 720);
			xyPos[ 1] = CRect(   0,   0, 320, 240);
			xyPos[ 2] = CRect( 320,   0, 640, 240);
			xyPos[ 3] = CRect( 640,   0, 960, 240);
			xyPos[ 4] = CRect( 960,   0,1280, 240);
			xyPos[ 5] = CRect(   0, 240, 320, 480);
			xyPos[ 6] = CRect( 960, 240,1280, 480);
			xyPos[ 7] = CRect(   0, 480, 320, 720);
			xyPos[ 8] = CRect( 960, 480,1280, 720);
			xyPos[ 9] = CRect(   0, 720, 320, 960);
			xyPos[10] = CRect( 320, 720, 640, 960);
			xyPos[11] = CRect( 640, 720, 960, 960);
			xyPos[12] = CRect( 960, 720,1280, 960);
			break;
		case SCR_NXN_16:
			xyPos[ 0] = CRect(   0,   0, 320, 240);
			xyPos[ 1] = CRect( 320,   0, 640, 240);
			xyPos[ 2] = CRect( 640,   0, 960, 240);
			xyPos[ 3] = CRect( 960,   0,1280, 240);
			xyPos[ 4] = CRect(   0, 240, 320, 480);
			xyPos[ 5] = CRect( 320, 240, 640, 480);
			xyPos[ 6] = CRect( 640, 240, 960, 480);
			xyPos[ 7] = CRect( 960, 240,1280, 480);
			xyPos[ 8] = CRect(   0, 480, 320, 720);
			xyPos[ 9] = CRect( 320, 480, 640, 720);
			xyPos[10] = CRect( 640, 480, 960, 720);
			xyPos[11] = CRect( 960, 480,1280, 720);
			xyPos[12] = CRect(   0, 720, 320, 960);
			xyPos[13] = CRect( 320, 720, 640, 960);
			xyPos[14] = CRect( 640, 720, 960, 960);
			xyPos[15] = CRect( 960, 720,1280, 960);
			break;
		}

		int nMinCh = 0;
		int nMaxCh = _UseSch() - 1;
		int nMaxRot = (_UseSch() / DivCamNum[div]) + (_UseSch() % DivCamNum[div] ? 1 : 0);
		for(int mode1=0, mode2=(nMaxRot-1); mode1<(nMaxRot/2); mode1++, mode2--)
		{
			for(int sc1=0, sc2=(DivCamNum[div]-1); sc1<DivCamNum[div]; sc1++, sc2--)
			{
				VideoScreen[div][mode1][nMinCh++] = xyPos[sc1];
				VideoScreen[div][mode2][nMaxCh--] = xyPos[sc2];
			}
		}
		if((nMaxRot%2) == 1)
		{
			int mode = (nMaxRot/2);
			int nPosCh = nMaxRot == 1 ? 0 : DivCamNum[div]*mode - (DivCamNum[div] - (nMaxCh - nMinCh + 1))/2;
			for(int sc=0; sc<DivCamNum[div]; sc++)
			{
				VideoScreen[div][mode][nPosCh++] = xyPos[sc];
			}
		}
	}
}
DX_SCREEN* dxm_control::GetDxScrn(int nCh)
{
	DX_SCREEN * pScrn = NULL;
	if(m_pD3d){
		pScrn = m_pD3d->GetDxScrn(nCh);
	}
	return pScrn;
}
CRect dxm_control::GetScreen(int nCh, BOOL bScale)
{
	CRect reDat(0,0,0,0);
	if(m_dxCfg.S_Mode < MAX_SCR)
		reDat = VideoScreen[m_dxCfg.S_Mode][m_dxCfg.S_Rotate][m_dxCfg.S_Ch[nCh]];
	if(bScale)
	{
		CRect reDst;
		m_pWnd->GetClientRect(&reDst);

		CRect reSrc = VideoScreen[0][0][0];
		CRect reRet = ScaleRect(&reDat, &reSrc, &reDst);
		return reRet;
	}
	return reDat;
}

int dxm_control::GetStatus(int nCh)
{
	DX_SCREEN * pScrn = GetDxScrn(nCh);
	if(pScrn) return pScrn->Status;
	return -1;
}

void dxm_control::SetSwapCh(int nCh1, int nCh2)
{
	if(m_bActive)
	{
		if(nCh1 == nCh2) return;
		if(nCh1 >= _UseSch()) return;
		if(nCh2 >= _UseSch()) return;

		int temp = m_dxCfg.S_Ch[nCh1];
		m_dxCfg.S_Ch[nCh1] = m_dxCfg.S_Ch[nCh2];
		m_dxCfg.S_Ch[nCh2] = temp;

		Send_Refresh(nCh1);
		Send_Refresh(nCh2);
	}
}

void dxm_control::ScreenSplit()
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->ScreenSplit();
		}
	}
}
void dxm_control::ScreenCover()
{
	if(m_bActive)
	{
		m_bActive = FALSE;
		if(m_pD3d){
			m_pD3d->ClearSurface();
		}
	}
}
void dxm_control::ChangeSplit()
{
	if(m_bActive)
	{
		for(int ch=0; ch<_UseSch(); ch++)
		{
			Send_Refresh(ch);
		}
	}
}

BOOL dxm_control::SetDxCfg(DX_Config * pDxCfg)
{
	m_dxCfg = *pDxCfg;
	return SetDivMode(MSG_SET_REFRESH, 0, 0);
}
DX_Config* dxm_control::GetDxCfg()
{
	return &m_dxCfg;
}

BOOL dxm_control::SetDivMode(uint uiMsg, int nValue1, int nValue2)
{
	if(m_bActive)
	{
		switch(uiMsg)
		{
		case MSG_MULTISCREEN:
			if(nValue1 < MAX_SCR)
			{
				if(nValue1 >= m_UseScrn)
					return FALSE;
			}
			else return FALSE;
			m_dxCfg.S_Mode = nValue1;
			m_dxCfg.S_Rotate = nValue2;
			break;
		case MSG_SELVCHANNEL:
			m_dxCfg.S_Mode = SCR_NXN_01;
			m_dxCfg.S_Rotate = m_dxCfg.S_Ch[nValue1];
			break;
		case MSG_SELROTATION:
			if(nValue1 < 0)
				m_dxCfg.S_Rotate++;
			else m_dxCfg.S_Rotate = nValue1;
			break;
		case MSG_SET_REFRESH:
			break;
		}
		int nDivCamNum = DivCamNum[m_dxCfg.S_Mode];
		int nMaxRotation = (_UseSch() / nDivCamNum) + (_UseSch() % nDivCamNum ? 1 : 0);
		if(m_dxCfg.S_Rotate >= nMaxRotation) m_dxCfg.S_Rotate = 0;
		else if(uiMsg == MSG_SELROTATION && nValue1 < 0)
		{
			bool b_next = true;
			while(b_next)
			{
				int Cnt = 0;
				for(int ch=0; ch<_UseSch(); ch++)
				{
					if(!GetScreen(ch, FALSE).IsRectEmpty())
					{
						if(GetStatus(ch) != SCR_IMG_LOGO)
						{
							b_next = false;
							break;
						}
						Cnt++;
						if(Cnt >= nDivCamNum) break;
					}
				}
				if(b_next)
				{
					m_dxCfg.S_Rotate++;
					if(m_dxCfg.S_Rotate >= nMaxRotation)
					{
						m_dxCfg.S_Rotate = 0;
						b_next = false;
					}
				}
			}
		}

		if(m_pD3d){
			m_pD3d->ClearSurface();
		}

		m_SelCh = -1;
		for(int ch=0; ch<_UseSch(); ch++)
		{
			DX_SCREEN * pScrn = GetDxScrn(ch);
			if(!pScrn) return FALSE;

			pScrn->Is_Sel = 0;
			if(m_SelCh < 0)
			{
				if(!GetScreen(ch, FALSE).IsRectEmpty())
				{
					m_SelCh = ch;
					pScrn->Is_Sel = 1;
				}
			}

			Send_Refresh(ch);
		}
		return TRUE;
	}
	return FALSE;
}
int dxm_control::GetSelDiv()
{
	return m_dxCfg.S_Mode;
}
int dxm_control::GetSelRot()
{
	return m_dxCfg.S_Rotate;
}

void dxm_control::SetSelCh(int nCh, BOOL bSelCh)
{
	if(m_bActive)
	{
		if(bSelCh)
		{
			if(m_SelCh > -1 && m_SelCh != nCh)
			{
				DX_SCREEN * pScrn = GetDxScrn(m_SelCh);
				if(!pScrn) return;

				if(pScrn->Is_Sel)
				{
					pScrn->Is_Sel = 0;
					Send_Refresh(m_SelCh);
				}
				m_SelCh = -1;

				m_bBufInit = TRUE;
			}

			if(nCh < 0) return;

			DX_SCREEN * pScrn = GetDxScrn(nCh);
			if(!pScrn) return;

			if(!pScrn->Is_Sel)
			{
				pScrn->Is_Sel = 1;
				Send_Refresh(nCh);
			}
			m_SelCh = nCh;
		}
		else
		{
			if(nCh < 0)
			{
				if(m_SelCh > -1) nCh = m_SelCh;
				else return;
			}

			DX_SCREEN * pScrn = GetDxScrn(nCh);
			if(!pScrn) return;

			if(pScrn->Is_Sel)
			{
				pScrn->Is_Sel = 0;
				Send_Refresh(nCh);
			}
			if(m_SelCh == nCh) m_SelCh = -1;
		}
	}
}
int dxm_control::GetSelCh()
{
	if(m_bActive)
	{
		return m_SelCh;
	}
	return -1;
}
void dxm_control::SetAudCh(int nCh, BOOL bAudCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(!pScrn) return;

		if(bAudCh)
			 pScrn->Is_Aud = 1;
		else pScrn->Is_Aud = 0;
		Send_Refresh(nCh);
	}
}
BOOL dxm_control::GetAudCh(int nCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(pScrn) if(pScrn->Is_Aud) return TRUE;
	}
	return FALSE;
}
void dxm_control::SetMicCh(int nCh, BOOL bMicCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(!pScrn) return;

		if(bMicCh)
			 pScrn->Is_Mic = 1;
		else pScrn->Is_Mic = 0;
		Send_Refresh(nCh);
	}
}
BOOL dxm_control::GetMicCh(int nCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(pScrn) if(pScrn->Is_Mic) return TRUE;
	}
	return FALSE;
}
void dxm_control::SetText(int nCh, WCHAR* szText)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(!pScrn) return;

		wcscpy_s(pScrn->szText, szText);
		Send_Refresh(nCh);
	}
}
WCHAR* dxm_control::GetText(int nCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(pScrn) return pScrn->szText;
	}
	return NULL;
}

DX_PT_RECT* dxm_control::GetPt_Rect(int nCh)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(pScrn) return &(pScrn->dx_Rect);
	}
	return NULL;
}
BOOL dxm_control::GetPt_Rect(int nCh, DX_PT_RECT* pDxRect)
{
	if(m_bActive)
	{
		DX_SCREEN * pScrn = GetDxScrn(nCh);
		if(pScrn)
		{
			*pDxRect = pScrn->dx_Rect;
			return TRUE;
		}
	}
	return FALSE;
}
void dxm_control::SetPt_Rect(int nCh, DX_PT_RECT* pDxRect)
{
	if(m_bActive)
	{
		if(m_pD3d){
			m_pD3d->SetPt_Rect(nCh, pDxRect);
		}
	}
}

void dxm_control::SendImageSrc(int nCh, VID_IMGSRC* pVid_img)
{
	if(m_bActive)
	{
		CRect reDst; reDst = GetScreen(nCh, TRUE);
		if(m_pD3d){
			m_pD3d->SendImageSrc(nCh, &reDst, pVid_img);
		}
	}
}
void dxm_control::SendCleanMsg(int nCh, int STATUS)
{
	if(m_bActive)
	{
		CRect reDst; reDst = GetScreen(nCh, TRUE);
		if(m_pD3d){
			m_pD3d->SendCleanMsg(nCh, &reDst, STATUS);
		}
	}
}
void dxm_control::Send_Refresh(int nCh)
{
	if(m_bActive)
	{
		CRect reDst; reDst = GetScreen(nCh, TRUE);
		if(m_pD3d){
			m_pD3d->Send_Refresh(nCh, &reDst);
		}
	}
}
void dxm_control::SendSoundSrc(int nCh, AUD_SNDSRC* pAud_snd)
{
	if(m_bActive)
	{
		if(nCh == m_SelCh)
		{
			if(m_pSnd){
				m_pSnd->SendSoundSrc(pAud_snd, m_bBufInit);
				m_bBufInit = FALSE;
			}
		}
	}
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

Cdx_control* _dxm_Init(CWnd * pWnd)
{
	dxm_control* pdx_M = new dxm_control;
	if(pdx_M)
	{
		if(pdx_M->Init(pWnd))
		{
			return (Cdx_control*)pdx_M;
		}
		else
		{
			pdx_M->Exit();
			delete pdx_M;
			pdx_M = NULL;
		}
	}
	return NULL;
}
void _dxm_Exit(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->Exit();
		delete pdx_M;
		pdx_M = NULL;
	}
}
CWnd* _dxm_GetWnd(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetWnd();
	}
	return NULL;
}
BOOL _dxm_DxCheck(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->DxCheck();
	}
	return TRUE;
}
void* _dxm_GetD3DD(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetD3DD();
	}
	return NULL;
}

void* _dxm_GetDxScrn(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetDxScrn(nCh);
	}
	return NULL;
}
CRect _dxm_GetScreen(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetScreen(nCh, TRUE);
	}
	return CRect(0,0,0,0);
}

int _dxm_GetStatus(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetStatus(nCh);
	}
	return -1;
}
void _dxm_SetSwapCh(void* Ptr, int nCh1, int nCh2)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SetSwapCh(nCh1, nCh2);
	}
}

void _dxm_ScreenSplit(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->ScreenSplit();
	}
}
void _dxm_ScreenCover(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->ScreenCover();
	}
}
void _dxm_ChangeSplit(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->ChangeSplit();
	}
}

BOOL _dxm_SetDxCfg(void* Ptr, void* pDxCfg)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->SetDxCfg((DX_Config*)pDxCfg);
	}
	return FALSE;
}
void* _dxm_GetDxCfg(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return (void*)pdx_M->GetDxCfg();
	}
	return NULL;
}

BOOL _dxm_SetDivMode(void* Ptr, uint uiMsg, int nValue1, int nValue2)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->SetDivMode(uiMsg, nValue1, nValue2);
	}
	return FALSE;
}
int _dxm_GetSelDiv(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetSelDiv();
	}
	return -1;
}
int _dxm_GetSelRot(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetSelRot();
	}
	return -1;
}

void _dxm_SetSelCh(void* Ptr, int nCh, BOOL bSelCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SetSelCh(nCh, bSelCh);
	}
}
int _dxm_GetSelCh(void* Ptr)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetSelCh();
	}
	return -1;
}
void _dxm_SetAudCh(void* Ptr, int nCh, BOOL bAudCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SetAudCh(nCh, bAudCh);
	}
}
BOOL _dxm_GetAudCh(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetAudCh(nCh);
	}
	return -1;
}
void _dxm_SetMicCh(void* Ptr, int nCh, BOOL bMicCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SetMicCh(nCh, bMicCh);
	}
}
BOOL _dxm_GetMicCh(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetMicCh(nCh);
	}
	return -1;
}
void _dxm_SetText(void* Ptr, int nCh, WCHAR* szText)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SetText(nCh, szText);
	}
}
WCHAR* _dxm_GetText(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetText(nCh);
	}
	return NULL;
}

DX_PT_RECT* _dxm_GetPt_Rect(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetPt_Rect(nCh);
	}
	return NULL;
}
BOOL _dxm_GetPt_Rect(void* Ptr, int nCh, DX_PT_RECT* pDxRect)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		return pdx_M->GetPt_Rect(nCh, pDxRect);
	}
	return FALSE;
}
void _dxm_SetPt_Rect(void* Ptr, int nCh, DX_PT_RECT* pDxRect)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SetPt_Rect(nCh, pDxRect);
	}
}

void _dxm_SendImageSrc(void* Ptr, int nCh, VID_IMGSRC* pVid_img)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SendImageSrc(nCh, pVid_img);
	}
}
void _dxm_SendCleanMsg(void* Ptr, int nCh, int STATUS)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SendCleanMsg(nCh, STATUS);
	}
}
void _dxm_Send_Refresh(void* Ptr, int nCh)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->Send_Refresh(nCh);
	}
}
void _dxm_SendSoundSrc(void* Ptr, int nCh, AUD_SNDSRC* pAud_snd)
{
	if(Ptr)
	{
		dxm_control* pdx_M = (dxm_control*)Ptr;
		pdx_M->SendSoundSrc(nCh, pAud_snd);
	}
}
