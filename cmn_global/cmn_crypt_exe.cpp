// cmn_crypt_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "cmn_crypt_exe.h"

#include "Base64.h"

/*****************************************************************************
 *
 *  cmn_encrypt
 *
 ****************************************************************************/
cmn_crypt_exe::cmn_crypt_exe()
{
}
cmn_crypt_exe::~cmn_crypt_exe()
{
}

BOOL cmn_crypt_exe::EncryptBase64(WCHAR* szIn, WCHAR* szOut)
{
	if(*szIn == L'\0')
		return FALSE;
	BOOL bEncrypt = FALSE;

	char cz[256];
	WideCharToMultiByte(CP_ACP, 0, szIn, -1, cz, 256, NULL, NULL);

	char* pEnc64 = base64Encode(cz, (uint32)strlen(cz));
	if(pEnc64)
	{
		MultiByteToWideChar(CP_ACP, 0, pEnc64, -1, szOut, 256);
		delete[] pEnc64;

		bEncrypt = TRUE;
	}
	return bEncrypt;
}
BOOL cmn_crypt_exe::DecryptBase64(WCHAR* szIn, WCHAR* szOut)
{
	if(*szIn == L'\0')
		return FALSE;
	BOOL bDecrypt = FALSE;

	char cz[256];
	WideCharToMultiByte(CP_ACP, 0, szIn, -1, cz, 256, NULL, NULL);

	uint32 len = 0;
	uint8* pDec64 = base64Decode(cz, len);
	if(pDec64 && len)
	{
		char dec[256]; memset(dec, 0, 256);
		memcpy((uint8*)dec, pDec64, len);
		delete[] pDec64;

		MultiByteToWideChar(CP_ACP, 0, dec, -1, szOut, 256);

		bDecrypt = TRUE;
	}
	return bDecrypt;
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

cmn_crypt_exe g_cmn_crypt;

BOOL _cmn_encript_base64(WCHAR* szIn, WCHAR* szOut)
{
	return g_cmn_crypt.EncryptBase64(szIn, szOut);
}
BOOL _cmn_decript_base64(WCHAR* szIn, WCHAR* szOut)
{
	return g_cmn_crypt.DecryptBase64(szIn, szOut);
}
