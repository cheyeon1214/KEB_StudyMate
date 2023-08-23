// cmn_crypt_exe.h
//

#pragma once

class cmn_crypt_exe
{
public:
	BOOL EncryptBase64(WCHAR* szIn, WCHAR* szOut);
	BOOL DecryptBase64(WCHAR* szIn, WCHAR* szOut);

public:
	cmn_crypt_exe();
	~cmn_crypt_exe();
};
