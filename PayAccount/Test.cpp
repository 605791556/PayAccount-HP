// Test.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PayAccount.h"
#include "Test.h"
#include "afxdialogex.h"


// CTest �Ի���

IMPLEMENT_DYNAMIC(CTest, CDialog)

CTest::CTest(CWnd* pParent /*=NULL*/)
: CPayAccountDlg(pParent)
{

}

CTest::~CTest()
{
}

void CTest::DoDataExchange(CDataExchange* pDX)
{
	CPayAccountDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTest, CPayAccountDlg)
END_MESSAGE_MAP()


// CTest ��Ϣ�������
