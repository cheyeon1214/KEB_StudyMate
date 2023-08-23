// cmn_global_exe.cpp : 해당 DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "cmn_global_exe.h"

/*****************************************************************************
 *
 *  cmn_global
 *
 ****************************************************************************/
cmn_global_exe::cmn_global_exe()
{
	WCHAR szPgmDir[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szPgmDir, _MAX_PATH); 

	CString Path = szPgmDir;
	int index = Path.ReverseFind(L'\\');
	swprintf_s(m_szMyPath, L"%s", (LPCTSTR)Path.Left(index));
	if( m_szMyPath[0] >= L'a' && m_szMyPath[0] <= L'z' )
		m_szMyPath[0] -= (L'a' - L'A');
	::SetCurrentDirectory(m_szMyPath);//<----------------

	swprintf_s(m_szResPath, L"%s\\res", m_szMyPath);
	swprintf_s(m_szImgPath, L"%s\\img", m_szResPath);
	swprintf_s(m_szIcoPath, L"%s\\ico", m_szResPath);
	swprintf_s(m_szCurPath, L"%s\\cur", m_szResPath);
	swprintf_s(m_szWavPath, L"%s\\wav", m_szResPath);

	SHGetSpecialFolderPath(NULL, m_szDocPath, CSIDL_PERSONAL, FALSE);
	swprintf_s(m_szSetPath, L"%s\\StudyMate", m_szDocPath);
	swprintf_s(m_szAppPath, L"%s\\Client", m_szSetPath);
	swprintf_s(m_szDbPath, L"%s\\DB", m_szSetPath);
	swprintf_s(m_szMemberPath, L"%s\\Member", m_szAppPath);
}
cmn_global_exe::~cmn_global_exe()
{
}

void cmn_global_exe::_SetServerApp()
{
	swprintf_s(m_szAppPath, L"%s\\Server", m_szSetPath);
	swprintf_s(m_szMemberPath, L"%s\\Member", m_szAppPath);
}

WCHAR* cmn_global_exe::_MyPath()
{
	return m_szMyPath;
}
WCHAR* cmn_global_exe::_ResPath()
{
	return m_szResPath;
}
WCHAR* cmn_global_exe::_ImgPath()
{
	return m_szImgPath;
}
WCHAR* cmn_global_exe::_IcoPath()
{
	return m_szIcoPath;
}
WCHAR* cmn_global_exe::_CurPath()
{
	return m_szCurPath;
}
WCHAR* cmn_global_exe::_WavPath()
{
	return m_szWavPath;
}

WCHAR* cmn_global_exe::_DocPath()
{
	return m_szDocPath;
}
WCHAR* cmn_global_exe::_SetPath()
{
	return m_szSetPath;
}
WCHAR* cmn_global_exe::_AppPath()
{
	return m_szAppPath;
}

WCHAR* cmn_global_exe::_DbPath()
{
	return m_szDbPath;
}
WCHAR* cmn_global_exe::_MemberPath()
{
	return m_szMemberPath;
}

/*****************************************************************************
 *
 *  user interface
 *
 ****************************************************************************/

cmn_global_exe g_cmn;

void SetServerApp()
{
	g_cmn._SetServerApp();
}
WCHAR* _cmn_MyPath()
{
	return g_cmn._MyPath();
}
WCHAR* _cmn_ResPath()
{
	return g_cmn._ResPath();
}
WCHAR* _cmn_ImgPath()
{
	return g_cmn._ImgPath();
}
WCHAR* _cmn_IcoPath()
{
	return g_cmn._IcoPath();
}
WCHAR* _cmn_CurPath()
{
	return g_cmn._CurPath();
}
WCHAR* _cmn_WavPath()
{
	return g_cmn._WavPath();
}
WCHAR* _cmn_DocPath()
{
	return g_cmn._DocPath();
}
WCHAR* _cmn_SetPath()
{
	return g_cmn._SetPath();
}
WCHAR* _cmn_AppPath()
{
	return g_cmn._AppPath();
}
WCHAR* _cmn_DbPath()
{
	return g_cmn._DbPath();
}
WCHAR* _cmn_MemberPath()
{
	return g_cmn._MemberPath();
}

void _cmn_gui_background(CDC* pDC, CDC* pMemDC, CBitmap* pBitmap, CRect *reDst)
{
	if(!pBitmap->m_hObject)
		return;

	BITMAP bitmap;
	pBitmap->GetBitmap(&bitmap);
	CBitmap* pOldBitmap = pMemDC->SelectObject(pBitmap);

	int y_off = reDst->top;
	for(int y=reDst->top; y<reDst->bottom; y+=bitmap.bmHeight)
	{
		int x_off = reDst->left;
		for(int x=reDst->left; x<reDst->right; x+=bitmap.bmWidth)
		{
			pDC->BitBlt(x, y, bitmap.bmWidth, bitmap.bmHeight, pMemDC, 0, 0, SRCCOPY);
			x_off += bitmap.bmWidth;
		}
		if(x_off < reDst->right)
		{
			pDC->BitBlt(x_off, y, reDst->right - x_off, bitmap.bmHeight, pMemDC, 0, 0, SRCCOPY);
		}
		y_off += bitmap.bmHeight;
	}
	if(y_off < reDst->bottom)
	{
		int x_off = reDst->left;
		for(int x=reDst->left; x<reDst->right; x+=bitmap.bmWidth)
		{
			pDC->BitBlt(x, y_off, bitmap.bmWidth, reDst->bottom - y_off, pMemDC, 0, 0, SRCCOPY);
			x_off += bitmap.bmWidth;
		}
		if(x_off < reDst->right)
		{
			pDC->BitBlt(x_off, y_off, reDst->right - x_off, reDst->bottom - y_off, pMemDC, 0, 0, SRCCOPY);
		}
	}
	pMemDC->SelectObject(pOldBitmap);
}
void _cmn_gui_stretchframe(CDC* pDC, CDC* pMemDC, CBitmap* pBitmap, CRect *reDst)
{
	if(!pBitmap->m_hObject)
		return;

	BITMAP bitmap;
	pBitmap->GetBitmap(&bitmap);
	CBitmap* pOldBitmap = pMemDC->SelectObject(pBitmap);

	int x, y, nWidth, nHeight, xSrc, ySrc;
	SIZE szDst;

	nWidth = bitmap.bmWidth/3;
	nHeight = bitmap.bmHeight/3;

	x = reDst->left + nWidth;
	y = reDst->top + nHeight;
	xSrc = nWidth;
	ySrc = nHeight;
	szDst.cx = reDst->Width() - 2*nWidth;
	szDst.cy = reDst->Height() - 2*nHeight;
	pDC->StretchBlt(x, y, szDst.cx, szDst.cy,
					pMemDC,
					xSrc, ySrc, nWidth, nHeight, 
					SRCCOPY);

	x = reDst->left;
	y = reDst->top;
	xSrc = 0;
	ySrc = 0;
	pDC->BitBlt(x, y, nWidth, nHeight, pMemDC, xSrc, ySrc, SRCCOPY);

	x = reDst->left + nWidth;
	y = reDst->top;
	xSrc = nWidth;
	ySrc = 0;
	szDst.cx = reDst->Width() - 2*nWidth;
	szDst.cy = nHeight;
	pDC->StretchBlt(x, y, szDst.cx, szDst.cy,
					pMemDC, 
					xSrc, ySrc, nWidth, nHeight, 
					SRCCOPY);

	x = reDst->right - nWidth;
	y = reDst->top;
	xSrc = bitmap.bmWidth - nWidth;
	ySrc = 0;
	pDC->BitBlt(x, y, nWidth, nHeight, pMemDC, xSrc, ySrc, SRCCOPY);

	x = reDst->right - nWidth;
	y = reDst->top + nHeight;
	xSrc = bitmap.bmWidth - nWidth;
	ySrc = nHeight;
	szDst.cx = nWidth;
	szDst.cy = reDst->Height() - 2*nHeight;
	pDC->StretchBlt(x, y, szDst.cx, szDst.cy, 
					pMemDC, 
					xSrc, ySrc, nWidth, nHeight, 
					SRCCOPY);

	x = reDst->left;
	y = reDst->bottom - nHeight;
	xSrc = 0;
	ySrc = bitmap.bmHeight - nHeight;
	pDC->BitBlt(x, y, nWidth, nHeight, pMemDC, xSrc, ySrc, SRCCOPY);

	x = reDst->left + nWidth;
	y = reDst->bottom - nHeight;
	xSrc = nWidth;
	ySrc = bitmap.bmHeight - nHeight;
	szDst.cx = reDst->Width() - 2*nWidth;
	szDst.cy = nHeight;
	pDC->StretchBlt(x, y, szDst.cx, szDst.cy,
					pMemDC, 
					xSrc, ySrc, nWidth, nHeight, 
					SRCCOPY);

	x = reDst->right - nWidth;
	y = reDst->bottom - nHeight;
	xSrc = bitmap.bmWidth - nWidth;
	ySrc = bitmap.bmHeight - nHeight;
	pDC->BitBlt(x, y, nWidth, nHeight, pMemDC, xSrc, ySrc, SRCCOPY);

	x = reDst->left;
	y = reDst->top + nHeight;
	xSrc = 0;
	ySrc = nHeight;
	szDst.cx = nWidth;
	szDst.cy = reDst->Height() - 2*nHeight;
	pDC->StretchBlt(x, y, szDst.cx, szDst.cy, 
					pMemDC, 
					xSrc, ySrc, nWidth, nHeight, 
					SRCCOPY);

	pMemDC->SelectObject(pOldBitmap);
}
