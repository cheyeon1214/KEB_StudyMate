#pragma once

// CChildCtrl 대화 상자입니다.

class CStudyManagerDlg;
class CChildCtrl
{
private:
	int m_iCh;
	Cdx_control* m_pdx;
	CStudyManagerDlg* m_pDlg;

	BOOL m_bActive;
	BOOL m_bIStart;

	uint8* m_buf;
	codec_audio_dec* m_decAud;
	codec_video_dec* m_decVid;

	SOCKET m_sockCld;

	USER_Item m_usItem;
	MEMBER_Item m_mbItem;
	MEMBER_Table m_mbTbl;
	
	ZCriticalSection m_cslock;

	codec_scale m_scale;

	void CalDuration();

	void Clear();

public:
	BOOL Init(CStudyManagerDlg* pDlg, Cdx_control* pdx, int iCh);
	void Exit();
	
	void CLOSE();
	BOOL ACCEPT(SOCKET sdAccept);
	void SendMsgTextSrc(WCHAR* szMsg);

	BOOL IsValidSock()
	{
		if(m_sockCld != INVALID_SOCKET)
			return TRUE;
		return FALSE;
	}

private:
	friend static void ChildThread(void* Ptr);
	void ChildThread();

	BOOL m_bChild;
	HANDLE m_hChildThread;
	HANDLE m_hChildThreadEnd;

public:
	CChildCtrl();
	virtual ~CChildCtrl();
};
