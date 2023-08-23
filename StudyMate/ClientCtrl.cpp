// ClientCtrl.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ClientCtrl.h"

#include "StudyMateDlg.h"

CClientCtrl::CClientCtrl()
{
	m_pdx = NULL;
	m_pDlg = NULL;

	m_bufVid = NULL;
	m_encVid = NULL;

	m_Face = NULL;
	m_bDetect = FALSE;

	m_bActive = FALSE;
	m_bIStart = FALSE;

	m_buf = NULL;
	m_sockCnt = INVALID_SOCKET;

	m_bClient = FALSE;
	m_hClientThread = NULL;
	m_hClientThreadEnd = NULL;
}

CClientCtrl::~CClientCtrl()
{
	Exit();
}

BOOL CClientCtrl::Init(CStudyMateDlg* pDlg, Cdx_control* pdx)
{
	m_pDlg = pDlg;
	m_pdx = pdx;

	m_hClientThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}
void CClientCtrl::Exit()
{
	_cam_drv_Exit();

	if(m_encVid)
	{
		m_encVid->Exit();
		delete m_encVid;
		m_encVid = NULL;
	}
	SAFE_DELETEA(m_bufVid);

	if(m_Face)
	{
		_face_detect_Exit(m_Face);
		m_Face = NULL;
	}

	CLOSE();

	if(m_usItem.uiLevel)
	{
		if(m_mbItem.GetDuration())
		{
			_db_member_Insert(&m_mbItem);
		}
	}

	if(m_hClientThreadEnd)
	{
		CloseHandle(m_hClientThreadEnd);
		m_hClientThreadEnd = NULL;
	}
}

BOOL CClientCtrl::SetCfg(ConfigMate* pCfg)
{
	if(m_cfg != *pCfg)
	{
		_cam_drv_Exit();

		if(m_encVid)
		{
			m_encVid->Exit();
			delete m_encVid;
			m_encVid = NULL;
		}
		SAFE_DELETEA(m_bufVid);

		if(m_Face)
		{
			_face_detect_Exit(m_Face);
			m_Face = NULL;
		}

		if(m_pdx)
		{
			_dxs_SendCleanMsg(m_pdx, SCR_IMG_LOGO);
		}

		if(pCfg->bUseCam)
		{
			WCHAR szDir[MAX_PATH];
			swprintf_s(szDir, L"%s\\Models", _cmn_MyPath());
			m_Face = _face_detect_Init(szDir, 5);

			m_encVid = new codec_video_enc;
			if(!m_encVid->Init(pCfg->hsize, pCfg->vsize, 30, 4000000))
			{
				m_encVid->Exit();
				delete m_encVid;
				m_encVid = NULL;
			}
			m_bufVid = new uint8[sizeof(AVS_HDR) + MAX_VIDEO_SIZE];

			if(_cam_drv_Init(pCfg->index, pCfg->hsize, pCfg->vsize))
			{
				_cam_drv_SetCallback(this, _CB_VInput_);
			}
		}

		m_cfg = *pCfg;
	}
	return TRUE;
}
void CClientCtrl::Detection(BOOL bDetect)
{
	if(m_Face)
	{
		m_bDetect = bDetect;
	}
	else m_bDetect = FALSE;
}

void CClientCtrl::_CB_VInput_(void* Ptr, VID_IMGSRC* pVid_img)
{
	((CClientCtrl*)Ptr)->SendRawImgSrc(pVid_img);
}
void CClientCtrl::SendRawImgSrc(VID_IMGSRC* pVid_img)
{
	if(m_Face)
	{
		if(m_bDetect)
		{
			m_bDetect = FALSE;

			vector<CRect> re_ary;
			_face_detect_Run(m_Face, pVid_img, re_ary);

			DX_PT_RECT dx_Rect(pVid_img->hsize, pVid_img->vsize);
			for(int i = 0; i < (int)re_ary.size(); i++)
			{
				RtObj obj(&re_ary[i]);
				dx_Rect.Insert(&obj);
			}
			if(m_pdx)
			{
				_dxs_SetPt_Rect(m_pdx, &dx_Rect);
			}
			if(m_bufVid)
			{
				AVS_PACKET* pAVS_dat = (AVS_PACKET*)m_bufVid;
				pAVS_dat->Default();

				int len = 0;
				uint8* buf = pAVS_dat->Payload;
				for(int i = 0; i < (int)re_ary.size(); i++)
				{
					memcpy(buf, &re_ary[i], sizeof(CRect));
					buf += sizeof(CRect);
					len += sizeof(CRect);
				}

				pAVS_dat->SetType(CD_OBJROI);
				pAVS_dat->SetSize(len);
				pAVS_dat->SetInfo(0, m_cfg.hsize);
				pAVS_dat->SetInfo(1, m_cfg.vsize);
				pAVS_dat->SetInfo(2, (int)re_ary.size());

				SendStreamData(pAVS_dat);
			}

			ZCsLock csl(&m_mblock);

			if(m_usItem.uiLevel)
			{
				DTime dt = _Get_DTime();
				if(re_ary.size())
				{
					if(!m_mbItem.dtStart)
					{
						m_mbItem.dtStart = dt;
					}
					m_mbItem.dtEnd = dt;

					if(m_pDlg)
					{
						m_pDlg->SetDuration(GetDuration());
					}
				}

				DTime dtDuration = m_mbItem.GetDuration();
				if(dtDuration)
				{
					bool b_insert = false;
					if(dt - m_mbItem.dtEnd > _DTIME_MINUTE)
						b_insert = true;
					else if(dtDuration > _DTIME_HOUR)
						b_insert = true;
					else if(dtDuration > _DTIME_MINUTE)
					{
						LTime lt;
						_cmn_dtime_DTimetoLTime(&dt, NULL, &lt);
						if(lt.stm.wHour == 23 && lt.stm.wMinute == 59)
						{
							if(lt.stm.wSecond > 50 && lt.stm.wSecond < 59)
								b_insert = true;
						}
					}
					if(b_insert)
					{
						_db_member_Insert(&m_mbItem);
						m_mbTbl.Insert(&m_mbItem);
						m_mbItem.dtStart = 0;
						m_mbItem.dtEnd = 0;
					}
				}
			}
		}
	}
	if(m_pdx)
	{
		_dxs_SendImageSrc(m_pdx, pVid_img);
	}
	if(m_bufVid)
	{
		AVS_PACKET* pAVS_dat = (AVS_PACKET*)m_bufVid;
		pAVS_dat->Default();
		if(m_encVid)
		{
			int nOutSize = MAX_VIDEO_SIZE; BOOL bRecvI = FALSE;
			if(m_encVid->Encode(pVid_img, pAVS_dat->Payload, nOutSize, bRecvI))
			{
				pAVS_dat->SetType(CD_VIDEO);
				pAVS_dat->SetSize(nOutSize);
				pAVS_dat->SetInfo(0, m_cfg.hsize);
				pAVS_dat->SetInfo(1, m_cfg.vsize);
				pAVS_dat->SetInfo(2, bRecvI ? I_TYPE : P_TYPE);

				SendStreamData(pAVS_dat);
			}
		}
	}
}

DTime CClientCtrl::GetDuration()
{
	if(m_usItem.uiLevel)
	{
		DTime dtDuration = m_mbTbl.GetDuration();
		dtDuration += m_mbItem.GetDuration();
		return dtDuration;
	}
	return 0;
}

void CClientCtrl::Clear()
{
	SAFE_CLOSESOCK(m_sockCnt);

	SAFE_DELETEA(m_buf);

	m_bActive = FALSE;
	m_bIStart = FALSE;

	if(m_pDlg)
	{
		m_pDlg->NetCon(FALSE);
	}
}

void CClientCtrl::CLOSE()
{
	m_bActive = FALSE;
	if(m_bClient)
	{
		m_bClient = FALSE;
		if(m_sockCnt != INVALID_SOCKET)
		{
			VMS_header hdr;
			hdr.cmd = CMD_DISCONNECT;
			tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));

			shutdown(m_sockCnt, SD_SEND);
			SAFE_CLOSESOCK(m_sockCnt);
		}
		if(m_hClientThread)
		{
			WaitForSingleObject(m_hClientThreadEnd, 1000);
			if(m_hClientThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hClientThread, &ExitCode))
				{
					TerminateThread(m_hClientThread, ExitCode);
					CloseHandle(m_hClientThread);
				}
				m_hClientThread = NULL;
			}
		}
	}

	Clear();
}
BOOL CClientCtrl::CONNECT(ConfigCon* pCfg)
{
	if(m_sockCnt != INVALID_SOCKET)
		return FALSE;

	m_sockCnt = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(m_sockCnt == INVALID_SOCKET) return FALSE;

	m_cfgCon = *pCfg;

	m_buf = new uint8[MAX_BUF_SIZE];

	m_bClient = TRUE;
	ResetEvent(m_hClientThreadEnd);
	m_hClientThread = (HANDLE)_beginthread(::ClientThread, 0, this);

	return TRUE;
}
void CClientCtrl::SendStreamData(AVS_PACKET* pAVS_dat)
{
	ZCsLock csl(&m_cslock);

	if(!m_bActive)
		return;
	if(m_sockCnt == INVALID_SOCKET) return;
	
	if(pAVS_dat->GetType() == CD_VIDEO)
	{
		if(pAVS_dat->GetInfo(2) == I_TYPE)
			m_bIStart = TRUE;
		if(!m_bIStart) return;
	}
	VMS_header hdr;
	hdr.cmd = CMD_SEND_STREAM;
	hdr.size = pAVS_dat->GetPacketLen();
	tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));
	tcp_send(m_sockCnt, (char*)pAVS_dat, pAVS_dat->GetPacketLen());
}
void CClientCtrl::SendMsgTextSrc(WCHAR* szMsg)
{
	ZCsLock csl(&m_cslock);

	if(!m_bActive)
		return;
	if(m_sockCnt == INVALID_SOCKET) return;

	VMS_header hdr;
	hdr.cmd = CMD_SEND_MESSAGE;
	hdr.size = ((int)wcslen(szMsg) + 1)*sizeof(WCHAR);
	tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));
	tcp_send(m_sockCnt, (char*)szMsg, hdr.size);
}

USER_Item* CClientCtrl::GetLoginUser()
{
	if(m_usItem.uiLevel)
	{
		return &m_usItem;
	}
	return NULL;
}
void CClientCtrl::SetLoginUser(USER_Item* pItem)
{
	ZCsLock csl(&m_mblock);

	if(pItem)
	{
		m_usItem = *pItem;
		m_mbItem = m_usItem;

		MEMBER_Search mbSch;
		mbSch.ltDate = _Get_LTime();
		mbSch.ltDate.SetDateFormat();
		wcscpy_s(mbSch.szUser, m_mbItem.szUser);
		_db_member_Search(&mbSch, &m_mbTbl);//<----------
	}
	else
	{
		CLOSE();

		if(m_mbItem.GetDuration())
		{
			_db_member_Insert(&m_mbItem);
		}
		m_usItem.Default();
		m_mbItem.Default();
		m_mbTbl.Clear();
	}
	if(m_pDlg)
	{
		m_pDlg->SetDuration(GetDuration());
	}
}

static void ClientThread(void * Ptr)
{
	((CClientCtrl*)Ptr)->ClientThread();
}
void CClientCtrl::ClientThread()
{
	char szIP[64];
	WideCharToMultiByte(CP_ACP, 0, m_cfgCon.szIP, -1, szIP, 64, NULL, NULL);

	BOOL bNetCon = FALSE;
	if(tcp_connect(m_sockCnt, szIP, m_cfgCon.iPort))
		bNetCon = TRUE;
	if(!m_bClient) // if close
	{
		m_hClientThread = NULL;
		SetEvent(m_hClientThreadEnd);
		
		Clear();
		return;
	}
	if(!bNetCon)
	{
		m_hClientThread = NULL;
		m_bClient = FALSE;
		
		Clear();
		return;
	}

	if(m_pDlg)
	{
		m_pDlg->NetCon(TRUE);
	}

	WCHAR szManager[64];
	_lang_(IDL_MANAGER, szManager);

	bool b_result = true;
	while(b_result)
	{
		VMS_header hdr;
		if(tcp_recv(m_sockCnt, (char*)&hdr, sizeof(VMS_header)) <= 0)
			b_result = false;
		if(hdr.size)
		{
			if(tcp_recv(m_sockCnt, (char*)m_buf, hdr.size) <= 0)
				break;
		}
		if(b_result)
		{
			if(hdr.cmd == CMD_OK)
				break;
			else if(hdr.cmd == CMD_REQ_AUTH)
			{
				WCHAR szAuth[256];
				swprintf_s(szAuth, L"%s::%s", m_cfgCon.szUser, m_cfgCon.szPass);

				hdr.cmd = CMD_ANS_AUTH;
				hdr.size = ((int)wcslen(szAuth) + 1)*sizeof(WCHAR);
				tcp_send(m_sockCnt, (char*)&hdr, sizeof(VMS_header));
				tcp_send(m_sockCnt, (char*)szAuth, hdr.size);
			}
			else if(hdr.cmd == CMD_FAIL) b_result = false;
		}
	}
	if(!b_result)
	{
		m_hClientThread = NULL;
		m_bClient = FALSE;

		Clear();
		return;
	}

	m_bActive = TRUE;

	if(m_pdx)
	{
		_dxs_SendCleanMsg(m_pdx, SCR_IMG_BLK);
	}
	while(m_bClient)
	{
		VMS_header hdr;
		if(tcp_recv(m_sockCnt, (char*)&hdr, sizeof(VMS_header)) <= 0)
			break;
		if(strcmp(hdr.marker, "vms") != 0)
		{
			recv(m_sockCnt, (char*)m_buf, MAX_BUF_SIZE, 0);
			continue;
		}
		if(hdr.size)
		{
			if(tcp_recv(m_sockCnt, (char*)m_buf, hdr.size) <= 0)
				break;
		}
		if(hdr.cmd == CMD_SEND_MESSAGE)
		{
			if(m_pDlg)
			{
				WCHAR szMsg[256];
				swprintf_s(szMsg, L"%s : %s", szManager, (WCHAR*)m_buf);
				m_pDlg->RecvMsgTextSrc(szMsg);
			}
		}
		else if(hdr.cmd == CMD_DISCONNECT)
		{
			m_bActive = FALSE;
			break;
		}
	}
	if(m_pdx)
	{
		_dxs_SendCleanMsg(m_pdx, SCR_IMG_LOGO);
	}

	Clear();

	m_hClientThread = NULL;
	if(m_bClient)
		m_bClient = FALSE;
	else SetEvent(m_hClientThreadEnd);
}
