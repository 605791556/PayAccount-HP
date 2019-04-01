// Test.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "Test.h"
#include "afxdialogex.h"


// CTest 对话框

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


// CTest 消息处理程序
