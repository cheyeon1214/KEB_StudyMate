// lang_pack_exe.h
//

#pragma once

class lang_pack_exe
{
private:
	USHORT usLangID;

	UINT m_Lang;
	UINT m_LangID[MAX_Lang_IDL];

	UINT GetIDS(int nIDL);

public:
	UINT _Get();
	void _Set(UINT uiLang);
	BOOL _Get(UINT nIDL, WCHAR* szLang, int nLen);

public:
	lang_pack_exe();
	~lang_pack_exe();
};
