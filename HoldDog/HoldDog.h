
// HoldDog.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "..\common\globle.h"
#include "..\common\common.h"

// CHoldDogApp:
// �йش����ʵ�֣������ HoldDog.cpp
//

class CHoldDogApp : public CWinApp
{
public:
	CHoldDogApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHoldDogApp theApp;