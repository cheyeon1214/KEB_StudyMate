#pragma once

// CClientCtrl 대화 상자입니다.

class CStudyMateDlg;
class CClientCtrl
{
private:
	Cdx_control* m_pdx;
	CStudyMateDlg* m_pDlg;

	ConfigMate m_cfg;//<----------

	uint8* m_bufVid;
	codec_video_enc* m_encVid;

	BOOL m_bDetect;
	Ccpu_FaceDetect* m_Face;

	ConfigCon m_cfgCon;//<----------

	BOOL m_bActive;
	BOOL m_bIStart;

	uint8* m_buf;

	SOCKET m_sockCnt;

	USER_Item m_usItem;
	MEMBER_Item m_mbItem;
	MEMBER_Table m_mbTbl;
	
	ZCriticalSection m_cslock;
	ZCriticalSection m_mblock;

	DTime GetDuration();

	void Clear();

public:
	BOOL Init(CStudyMateDlg* pDlg, Cdx_control* pdx);
	void Exit();

	BOOL SetCfg(ConfigMate* pCfg);
	ConfigMate* GetCfg() { return &m_cfg; }

	void Detection(BOOL bDetect);
	
	void CLOSE();
	BOOL CONNECT(ConfigCon* pCfg);
	void SendStreamData(AVS_PACKET* pAVS_dat);
	void SendMsgTextSrc(WCHAR* szMsg);

	USER_Item* GetLoginUser();
	void SetLoginUser(USER_Item* pItem);

	BOOL IsValidSock()
	{
		if(m_sockCnt != INVALID_SOCKET)
			return TRUE;
		return FALSE;
	}

public:
	static void _CB_VInput_(void* Ptr, VID_IMGSRC* pVid_img);
	void SendRawImgSrc(VID_IMGSRC* pVid_img);

private:
	friend static void ClientThread(void* Ptr);
	void ClientThread();

	BOOL m_bClient;
	HANDLE m_hClientThread;
	HANDLE m_hClientThreadEnd;

public:
	CClientCtrl();
	virtual ~CClientCtrl();
};
