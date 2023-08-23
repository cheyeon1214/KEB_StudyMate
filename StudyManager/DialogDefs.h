#ifndef _DIALOG_DEFS_H_
#define _DIALOG_DEFS_H_

struct ConfigManager
{
	BOOL bUseSvr;
	WCHAR szIP[64]; int iPort;

	ConfigManager()
	{
		this->bUseSvr = FALSE;
		wcscpy_s(this->szIP, L"127.0.0.1");
		this->iPort = 12000;
	}
	inline ConfigManager& operator = (ConfigManager& other)
	{
		this->bUseSvr = other.bUseSvr;
		wcscpy_s(this->szIP, other.szIP);
		this->iPort = other.iPort;

		return *this;
	}
	inline BOOL operator == (ConfigManager& other)
	{
		if(this->bUseSvr != other.bUseSvr) return FALSE;
		if(wcscmp(this->szIP, other.szIP)) return FALSE;
		if(this->iPort != other.iPort) return FALSE;

		return TRUE;
	}
	inline BOOL operator != (ConfigManager& other)
	{
		if((*this) == other)
			return FALSE;
		return TRUE;
	}
};

const LOGFONT BTN_FONT = {-14, 0, 0, 0, 700, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const LOGFONT COMB_FONT = {-11, 0, 0, 0, 400, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const LOGFONT SNAP_FONT = {-14, 0, 0, 0, 400, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const COLORREF bg_crtext = RGB(0, 0, 0);
const COLORREF bg_crback = RGB(162, 172, 181);
const COLORREF btn_crtext = RGB(63, 63, 63);
const COLORREF btn_crtextd = RGB(255, 255, 255);

const DWORD dwPushStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT;
const DWORD dwCheckStyle = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_FLAT;

const RECT xyRECT = { 0, 0, 1000, 600 };
const RECT xyTOOL = { 0, 0, 640, 46 };
const RECT xySIDE = { 644, 0, 1000, 600 };
const RECT xySCRN = { 0, 50, 640, 600 };

const RECT xyTOOL_div_btn = { 8, 7, 50, 41 };
const int xyTOOL_div_interval = 44;
const RECT xyTOOL_setting_btn = { 552, 7, 632, 41 };

const RECT xySIDE_ctrl = { 6, 6, 350, 544 };
const RECT xySIDE_member_btn = { 16, 544, 177, 584 };
const RECT xySIDE_manager_btn = { 179, 544, 340, 584 };

#define TMR_CLOCKID 0
#define TMR_FDETECT 1

#endif // _DIALOG_DEFS_H_
