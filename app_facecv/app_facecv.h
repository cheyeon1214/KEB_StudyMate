﻿// app_facecv.h : app_facecv DLL의 주 헤더 파일
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CappfacecvApp
// 이 클래스 구현에 대해서는 app_facecv.cpp를 참조하세요.
//

class CappfacecvApp : public CWinApp
{
public:
	CappfacecvApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
