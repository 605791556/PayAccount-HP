
// PayServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once
#include "dbData.h"

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPayServerApp:
// �йش����ʵ�֣������ PayServer.cpp
//

class CPayServerApp : public CWinApp
{
public:
	CDbData* m_dbData;
public:
	CPayServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPayServerApp theApp;