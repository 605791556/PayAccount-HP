// PayAccount.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "sockClient.h"
#include "..\common\globle.h"

// CPayAccountApp:
// �йش����ʵ�֣������ PayAccount.cpp
//

class CPayAccountApp : public CWinApp
{
public:
	CPayAccountApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPayAccountApp theApp;