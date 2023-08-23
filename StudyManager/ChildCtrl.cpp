// ChildCtrl.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ChildCtrl.h"

#include "StudyManagerDlg.h"

CChildCtrl::CChildCtrl()
{
	m_pDlg = NULL;
	m_pdx = NULL;
	m_iCh = -1;

	m_bActive = FALSE;
	m_bIStart = FALSE;

	m_buf = NULL;
	m_decAud = NULL;
	m_decVid = NULL;

	m_sockCld = INVALID_SOCKET;

	m_bChild = FALSE;
	m_hChildThread = NULL;
	m_hChildThreadEnd = NULL;
}

CChildCtrl::~CChildCtrl()
{
	Exit();
}

BOOL CChildCtrl::Init(CStudyManagerDlg* pDlg, Cdx_control* pdx, int iCh)
{
	if(iCh < 0) return FALSE;

	m_pDlg = pDlg;
	m_pdx = pdx;
	m_iCh = iCh;

	m_hChildThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}
void CChildCtrl::Exit()
{
	CLOSE();

	if(m_hChildThreadEnd)
	{
		CloseHandle(m_hChildThreadEnd);
		m_hChildThreadEnd = NULL;
	}
}

void CChildCtrl::CalDuration()
{
	if(m_pdx)
	{
		DTime dtDuration = m_mbTbl.GetDuration();
		dtDuration += m_mbItem.GetDuration();
		if(!dtDuration) return;

		WCHAR szDuration[64];
		DTime hh = dtDuration / _DTIME_HOUR; dtDuration -= (hh * _DTIME_HOUR);
		DTime mm = dtDuration / _DTIME_MINUTE; dtDuration -= (mm * _DTIME_MINUTE);
		DTime ss = dtDuration / _DTIME_SECOND; dtDuration -= (ss * _DTIME_SECOND);
		swprintf_s(szDuration, L"%02d:%02d:%02d", (int)hh, (int)mm, (int)ss);
		if(wcscmp(szDuration, _dxm_GetText(m_pdx, m_iCh)) != 0)
		{
			_dxm_SetText(m_pdx, m_iCh, szDuration);
		}
	}
}

void CChildCtrl::Clear()
{
	SAFE_CLOSESOCK(m_sockCld);

	if(m_decAud != NULL)
	{
		m_decAud->Exit();
		delete m_decAud;
		m_decAud = NULL;
	}
	if(m_decVid != NULL)
	{
		m_decVid->Exit();
		delete m_decVid;
		m_decVid = NULL;
	}
	SAFE_DELETEA(m_buf);

	m_bActive = FALSE;
	m_bIStart = FALSE;

	if(m_mbItem.GetDuration())
	{
		_db_member_Insert(&m_mbItem);
	}
	m_usItem.Default();
	m_mbItem.Default();
	m_mbTbl.Clear();
}

void CChildCtrl::CLOSE()
{
	m_bActive = FALSE;
	if(m_bChild)
	{
		m_bChild = FALSE;
		if(m_sockCld != INVALID_SOCKET)
		{
			VMS_header hdr;
			hdr.cmd = CMD_DISCONNECT;
			tcp_send(m_sockCld, (char*)&hdr, sizeof(VMS_header));

			shutdown(m_sockCld, SD_SEND);
			SAFE_CLOSESOCK(m_sockCld);
		}
		if(m_hChildThread)
		{
			WaitForSingleObject(m_hChildThreadEnd, 1000);
			if(m_hChildThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hChildThread, &ExitCode))
				{
					TerminateThread(m_hChildThread, ExitCode);
					CloseHandle(m_hChildThread);
				}
				m_hChildThread = NULL;
			}
		}
	}

	Clear();
}
BOOL CChildCtrl::ACCEPT(SOCKET sdAccept)
{
	if(m_sockCld != INVALID_SOCKET)
		return FALSE;

	VMS_header hdr; char buf[512];

	hdr.cmd = CMD_REQ_AUTH; hdr.size = 0;
	tcp_send(sdAccept, (char*)&hdr, sizeof(VMS_header));

	if(tcp_recv(sdAccept, (char*)&hdr, sizeof(VMS_header)) <= 0)
		return FALSE;
	if(hdr.size)
	{
		if(tcp_recv(sdAccept, buf, hdr.size) <= 0)
			return FALSE;
	}
	if(hdr.cmd != CMD_ANS_AUTH)
		return FALSE;

	WCHAR szAuth[256];
	memcpy(szAuth, buf, hdr.size);

	WCHAR* szUser = szAuth;
	WCHAR* szPass = wcsstr(szAuth, L"::");
	if(szPass) {
		*szPass = L'\0'; szPass += 2;
	}
	else return FALSE;

	USER_Search usSch;
	wcscpy_s(usSch.szUser, szUser);
	wcscpy_s(usSch.szPass, szPass);

	USER_Table usTbl;
	if(!_db_user_Search(&usSch, &usTbl))
		return FALSE;
	if(!usTbl.Cnt()) return FALSE;

	m_usItem = *(usTbl.Obj(0));
	m_mbItem = m_usItem;

	MEMBER_Search mbSch;
	mbSch.ltDate = _Get_LTime();
	mbSch.ltDate.SetDateFormat();
	wcscpy_s(mbSch.szUser, m_mbItem.szUser);
	_db_member_Search(&mbSch, &m_mbTbl);//<----------

	hdr.cmd = CMD_OK; hdr.size = 0;
	tcp_send(sdAccept, (char*)&hdr, sizeof(VMS_header));

	m_sockCld = sdAccept;

	m_decAud = new codec_audio_dec;
	if(!m_decAud->Init(16, 1, 8000))
	{
		m_decAud->Exit();
		delete m_decAud;
		m_decAud = NULL;
	}
	m_decVid = new codec_video_dec;
	if(!m_decVid->Init())
	{
		m_decVid->Exit();
		delete m_decVid;
		m_decVid = NULL;
	}
	m_buf = new uint8[MAX_BUF_SIZE];

	m_bChild = TRUE;
	ResetEvent(m_hChildThreadEnd);
	m_hChildThread = (HANDLE)_beginthread(::ChildThread, 0, this);

	return TRUE;
}
void CChildCtrl::SendMsgTextSrc(WCHAR* szMsg)
{
	ZCsLock csl(&m_cslock);

	if(!m_bActive)
		return;
	if(m_sockCld == INVALID_SOCKET) return;

	VMS_header hdr;
	hdr.cmd = CMD_SEND_MESSAGE;
	hdr.size = ((int)wcslen(szMsg) + 1)*sizeof(WCHAR);
	tcp_send(m_sockCld, (char*)&hdr, sizeof(VMS_header));
	tcp_send(m_sockCld, (char*)szMsg, hdr.size);
}

static void ChildThread(void * Ptr)
{
	((CChildCtrl*)Ptr)->ChildThread();
}
void CChildCtrl::ChildThread()
{
	m_bActive = TRUE;

	BOOL bIStart = FALSE;
	
	if(m_pdx)
	{
		_dxm_SendCleanMsg(m_pdx, m_iCh, SCR_IMG_BLK);
	}
	while(m_bChild)
	{
		VMS_header hdr;
		if(tcp_recv(m_sockCld, (char*)&hdr, sizeof(VMS_header)) <= 0)
			break;
		if(strcmp(hdr.marker, "vms") != 0)
		{
			recv(m_sockCld, (char*)m_buf, MAX_BUF_SIZE, 0);
			continue;
		}
		if(hdr.size)
		{
			if(tcp_recv(m_sockCld, (char*)m_buf, hdr.size) <= 0)
				break;
		}
		if(hdr.cmd == CMD_SEND_STREAM)
		{
			AVS_PACKET* pAVS_dat = (AVS_PACKET*)m_buf;
			switch(pAVS_dat->GetType())
			{
			case CD_AUDIO:
				{
					AUD_SNDSRC stAud_snd;
					stAud_snd.channel = pAVS_dat->GetInfo(0);
					stAud_snd.samples = pAVS_dat->GetInfo(1);
					stAud_snd.databit = pAVS_dat->GetInfo(2);
					if(m_decAud)
					{
						if(m_decAud->Decode(pAVS_dat->Payload, pAVS_dat->GetSize(), &stAud_snd))
						{
							if(m_pdx)
							{
								if(!_dxm_GetAudCh(m_pdx, m_iCh))
									_dxm_SetAudCh(m_pdx, m_iCh, TRUE);
								_dxm_SendSoundSrc(m_pdx, m_iCh, &stAud_snd);
							}
						}
					}
				}
				break;
			case CD_VIDEO:
				if(pAVS_dat->GetInfo(2) == I_TYPE)
					bIStart = TRUE;
				if(bIStart)
				{
					VID_IMGSRC stVid_img;
					if(m_decVid)
					{
						if(m_decVid->Decode(pAVS_dat->Payload, pAVS_dat->GetSize(), &stVid_img))
						{
							if(stVid_img.IsImage())
							{
								if(m_pdx)
								{
									_dxm_SendImageSrc(m_pdx, m_iCh, &stVid_img);
								}
							}
						}
					}
				}
				break;
			case CD_OBJROI:
				{
					int hsize = pAVS_dat->GetInfo(0);
					int vsize = pAVS_dat->GetInfo(1);
					int iCnt = pAVS_dat->GetInfo(2);

					DX_PT_RECT dx_Rect(hsize, vsize);
					
					uint8* buf = pAVS_dat->Payload;
					for(int i = 0; i < iCnt; i++)
					{
						CRect re = *((CRect*)buf);
						buf += sizeof(CRect);

						RtObj obj(&re);
						dx_Rect.Insert(&obj);
					}
					if(m_pdx)
					{
						_dxm_SetPt_Rect(m_pdx, m_iCh, &dx_Rect);
					}

					DTime dt = _Get_DTime();
					if(iCnt)
					{
						if(!m_mbItem.dtStart)
						{
							m_mbItem.dtStart = dt;
						}
						m_mbItem.dtEnd = dt;

						CalDuration();
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
				break;
			}
		}
		else if(hdr.cmd == CMD_SEND_MESSAGE)
		{
			if(m_pDlg)
			{
				WCHAR szMsg[256];
				swprintf_s(szMsg, L"%s : %s", m_usItem.szName, (WCHAR*)m_buf);
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
		_dxm_SendCleanMsg(m_pdx, m_iCh, SCR_IMG_LOGO);
	}

	Clear();

	m_hChildThread = NULL;
	if(m_bChild)
		m_bChild = FALSE;
	else SetEvent(m_hChildThreadEnd);
}
