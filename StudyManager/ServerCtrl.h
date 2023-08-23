#pragma once

class CStudyManagerDlg;
class CServerCtrl
{
private:
	CStudyManagerDlg* m_pDlg;

	SOCKET m_sockSvr;

	ConfigManager m_cfg;

public:
	BOOL Init(CStudyManagerDlg* pDlg);
	void Exit();

	void CLOSE();

	BOOL SetCfg(ConfigManager* pCfg);
	ConfigManager* GetCfg() { return &m_cfg; }

private:
	friend static void AcceptThread(void* Ptr);
	void AcceptThread();

	BOOL m_bAcpt;
	HANDLE m_hAcptThread;
	HANDLE m_hAcptThreadEnd;

public:
	CServerCtrl();
	virtual ~CServerCtrl();
};
