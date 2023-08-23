#ifndef _DIALOG_DEFS_H_
#define _DIALOG_DEFS_H_

struct ConfigMate
{
	BOOL bUseCam;
	int index;
	int hsize, vsize;

	ConfigMate()
	{
		this->bUseCam = FALSE;
		this->index = 0;
		this->hsize = 640;
		this->vsize = 480;
	}
	inline ConfigMate& operator = (ConfigMate& other)
	{
		this->bUseCam = other.bUseCam;
		this->index = other.index;
		this->hsize = other.hsize;
		this->vsize = other.vsize;

		return *this;
	}
	inline BOOL operator == (ConfigMate& other)
	{
		if(this->bUseCam != other.bUseCam) return FALSE;
		if(this->index != other.index) return FALSE;
		if(this->hsize != other.hsize) return FALSE;
		if(this->vsize != other.vsize) return FALSE;

		return TRUE;
	}
	inline BOOL operator != (ConfigMate& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
};

struct ConfigCon
{
	WCHAR szIP[64]; int iPort;
	WCHAR szUser[64], szPass[256];

	ConfigCon()
	{
		wcscpy_s(this->szIP, L"127.0.0.1");
		this->iPort = 12000;
		wcscpy_s(this->szUser, L"admin");
		wcscpy_s(this->szPass, L"admin");
	}
	inline ConfigCon& operator = (ConfigCon& other)
	{
		wcscpy_s(this->szIP, other.szIP);
		this->iPort = other.iPort;
		wcscpy_s(this->szUser, other.szUser);
		wcscpy_s(this->szPass, other.szPass);

		return *this;
	}
	inline BOOL operator == (ConfigCon& other)
	{
		if(wcscmp(this->szIP, other.szIP)) return FALSE;
		if(this->iPort != other.iPort) return FALSE;
		if(wcscmp(this->szUser, other.szUser)) return FALSE;
		if(wcscmp(this->szPass, other.szPass)) return FALSE;

		return TRUE;
	}
	inline BOOL operator != (ConfigCon& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
};

const LOGFONT BTN_FONT = {-14, 0, 0, 0, 700, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const LOGFONT COMB_FONT = {-11, 0, 0, 0, 400, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const LOGFONT DATE_FONT = {-14, 0, 0, 0, 700, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const LOGFONT TIME_FONT = {-22, 0, 0, 0, 700, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const COLORREF bg_crtext = RGB(0, 0, 0);
const COLORREF bg_crback = RGB(162, 172, 181);
const COLORREF btn_crtext = RGB(63, 63, 63);
const COLORREF btn_crtextd = RGB(255, 255, 255);

const DWORD dwPushStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT;
const DWORD dwCheckStyle = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_FLAT;

#define TMR_CLOCKID 0
#define TMR_FDETECT 1

#endif // _DIALOG_DEFS_H_
