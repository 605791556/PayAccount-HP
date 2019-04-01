// LockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "LockDlg.h"
#include "afxdialogex.h"


// CLockDlg 对话框
DWORD WINAPI SetVisibleThread(LPVOID lpParam);
IMPLEMENT_DYNAMIC(CLockDlg, CDialogEx)

CLockDlg::CLockDlg(BOOL bLogin,CWnd* pParent /*=NULL*/)
	: CDialogEx(CLockDlg::IDD, pParent)
{
	m_bLock = true;
	m_bLogin = bLogin;
}

CLockDlg::~CLockDlg()
{
}

void CLockDlg::SetType(bool bLock)
{
	m_bLock = bLock;
}

void LockCallback(void* p,string strData)
{
	USES_CONVERSION;
	CLockDlg* pThis=(CLockDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_CALLBACK,NULL,(LPARAM)pStrData);
	}
}

BOOL CLockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (!m_bLock)
	{
		if (m_bLogin)
		{
			SetWindowText(L"登录确认");
		}
		else
		{
			SetWindowText(L"请输入密码");
		}
	}
	m_editSet.m_type = EDIT_TYPE_NO_CHN;

	//设置回调
	g_Globle.SetCallback(LockCallback,this);
	return TRUE;
}

LRESULT CLockDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
{
	string* pStrData = (string*)lParam;
	Json::Reader r;
	Json::Value root;
	r.parse(*pStrData,root);
	delete pStrData;

	EM_SOCK_CMD cmd = (EM_SOCK_CMD)root[CONNECT_CMD].asInt();
	EM_CMD_RET ret = (EM_CMD_RET)root[CMD_RetType[EM_CMD_RETYPE_RESULT]].asInt();

	switch (cmd)
	{
	case SOCK_CMD_CHECKPWD:
		{
			if (ret == NET_CMD_FAIL)
			{
				GetDlgItem(IDC_TISHI)->ShowWindow(SW_SHOW);
				CreateThread(NULL, 0, SetVisibleThread, this, 0, NULL);
			}
			else
			{
				CString strPwd;
				GetDlgItemText(IDC_PWD, strPwd);
				g_Globle.m_strUserPwd = strPwd;
				CDialogEx::OnOK();
			}
		}
		break;
	}
	return TRUE;
}

void CLockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PWD, m_editSet);
}


BEGIN_MESSAGE_MAP(CLockDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLockDlg::OnBnClickedOk)
	ON_MESSAGE(WM_HEDE, &CLockDlg::OnSetVisible)
	ON_MESSAGE(WM_CALLBACK, &CLockDlg::OnCallBack)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()


// CLockDlg 消息处理程序

DWORD WINAPI SetVisibleThread(LPVOID lpParam)
{
	CLockDlg* pthis =(CLockDlg*) lpParam;
	Sleep(1500);
	//pthis->GetDlgItem(IDC_TISHI)->ShowWindow(SW_HIDE);
	pthis->PostMessage(WM_HEDE);
	return 0;
}

LRESULT CLockDlg::OnSetVisible(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_TISHI)->ShowWindow(SW_HIDE);
	return 0;
}

void CLockDlg::OnBnClickedOk()
{
	CString strPwd;
	GetDlgItemText(IDC_PWD, strPwd);
	if (strPwd.IsEmpty())
	{
		GetDlgItem(IDC_TISHI)->ShowWindow(SW_SHOW);
		CreateThread(NULL, 0, SetVisibleThread, this, 0, NULL);
	}
	else
	{
		USES_CONVERSION;
		Json::Value root;
		root[CONNECT_CMD]=SOCK_CMD_CHECKPWD;
		root[CMD_CHECKPWD[EM_CHECKPWD_NAME]]=T2A(g_Globle.m_strUserName);
		root[CMD_CHECKPWD[EM_CHECKPWD_PWD]]=T2A(strPwd);
		Json::FastWriter writer;  
		string temp = writer.write(root);
		g_Globle.SendTo(temp);
	}
}


void CLockDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_bLock)
	{
	  
	}
	else
	{
		CDialogEx::OnCancel();
	}
}


HBRUSH CLockDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_TISHI)
	{
		pDC->SetTextColor(RGB(255, 0, 0));//设置字体颜色
	}
	return hbr;
}


void CLockDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLockDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (HTCAPTION == nHitTest)
	{
		return;
	}
	CDialogEx::OnNcLButtonDown(nHitTest, point);
}
