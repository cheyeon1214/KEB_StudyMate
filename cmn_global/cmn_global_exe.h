// cmn_global_exe.h
//

#pragma once

class cmn_global_exe
{
private:
	WCHAR m_szMyPath[MAX_PATH];
	WCHAR m_szResPath[MAX_PATH];
	WCHAR m_szImgPath[MAX_PATH];
	WCHAR m_szIcoPath[MAX_PATH];
	WCHAR m_szCurPath[MAX_PATH];
	WCHAR m_szWavPath[MAX_PATH];

	WCHAR m_szDocPath[MAX_PATH];
	WCHAR m_szSetPath[MAX_PATH];
	WCHAR m_szAppPath[MAX_PATH];

	WCHAR m_szDbPath[MAX_PATH];
	WCHAR m_szMemberPath[MAX_PATH];

public:
	void _SetServerApp();

	WCHAR* _MyPath();
	WCHAR* _ResPath();
	WCHAR* _ImgPath();
	WCHAR* _IcoPath();
	WCHAR* _CurPath();
	WCHAR* _WavPath();

	WCHAR* _DocPath();
	WCHAR* _SetPath();
	WCHAR* _AppPath();

	WCHAR* _DbPath();
	WCHAR* _MemberPath();

public:
	cmn_global_exe();
	~cmn_global_exe();
};
