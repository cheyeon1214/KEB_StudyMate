// ServerCtrl.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ServerCtrl.h"

#include "StudyManagerDlg.h"

CServerCtrl::CServerCtrl()
{
	m_pDlg = NULL;

	m_sockSvr = INVALID_SOCKET;

	m_bAcpt = FALSE;
	m_hAcptThread = NULL;
	m_hAcptThreadEnd = NULL;
}
CServerCtrl::~CServerCtrl()
{
	Exit();
}

BOOL CServerCtrl::Init(CStudyManagerDlg* pDlg)
{
	m_pDlg = pDlg;

	m_hAcptThreadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}
void CServerCtrl::Exit()
{
	CLOSE();

	if(m_hAcptThreadEnd)
	{
		CloseHandle(m_hAcptThreadEnd);
		m_hAcptThreadEnd = NULL;
	}
}

void CServerCtrl::CLOSE()
{
	if(m_bAcpt)
	{
		m_bAcpt = FALSE;
		SAFE_CLOSESOCK(m_sockSvr);
		if(m_hAcptThread)
		{
			WaitForSingleObject(m_hAcptThreadEnd, 1000);
			if(m_hAcptThread)
			{
				DWORD ExitCode = 0;
				if(::GetExitCodeThread(m_hAcptThread, &ExitCode))
				{
					TerminateThread(m_hAcptThread, ExitCode);
					CloseHandle(m_hAcptThread);
				}
				m_hAcptThread = NULL;
			}
		}
	}
}

BOOL CServerCtrl::SetCfg(ConfigManager* pCfg)
{
	if(m_cfg != *pCfg)
	{
		CLOSE();
		if(pCfg->bUseSvr)
		{
			m_sockSvr = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			if(m_sockSvr != INVALID_SOCKET)
			{
				char szIP[64];
				WideCharToMultiByte(CP_ACP, 0, pCfg->szIP, -1, szIP, 64, NULL, NULL);

				if(!tcp_listen(m_sockSvr, szIP, pCfg->iPort))
					SAFE_CLOSESOCK(m_sockSvr);
			}
			if(m_sockSvr != INVALID_SOCKET)
			{
				m_bAcpt = TRUE;
				ResetEvent(m_hAcptThreadEnd);
				m_hAcptThread = (HANDLE)_beginthread(::AcceptThread, 0, this);
			}
		}

		m_cfg = *pCfg;
	}
	return TRUE;
}

static void AcceptThread(void * Ptr)
{
	((CServerCtrl*)Ptr)->AcceptThread();
}
void CServerCtrl::AcceptThread()
{
	if(!m_bAcpt) // if close
	{
		m_hAcptThread = NULL;
		SetEvent(m_hAcptThreadEnd);
		return;
	}

	SOCKET sdAccept = INVALID_SOCKET;
	SOCKADDR_IN	addrFrom;
	int nAddFromLen = sizeof(addrFrom);

	while(m_bAcpt)
	{
		nAddFromLen = sizeof(addrFrom);
		memset(&addrFrom,0,sizeof(addrFrom));
		sdAccept = accept(m_sockSvr, (LPSOCKADDR)&addrFrom,  &nAddFromLen);
		if(m_bAcpt && sdAccept != INVALID_SOCKET)
		{
			bool b_accept = false;
			if(m_pDlg)
			{
				VMS_header hdr;
				if(!m_pDlg->ACCEPT(sdAccept))
				{
					hdr.cmd = CMD_FAIL;
					tcp_send(sdAccept, (char*)&hdr, sizeof(VMS_header));
				}
				else b_accept = true;
			}
			if(!b_accept)
			{
				shutdown(sdAccept, SD_SEND);
				SAFE_CLOSESOCK(sdAccept);
			}
		}
	}

	SAFE_CLOSESOCK(m_sockSvr);

	m_hAcptThread = NULL;
	if(m_bAcpt)
		m_bAcpt = FALSE;
	else SetEvent(m_hAcptThreadEnd);
}
