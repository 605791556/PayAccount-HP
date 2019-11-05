// MdfPwdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "MdfPwdDlg.h"
#include "afxdialogex.h"


// CMdfPwdDlg 对话框

IMPLEMENT_DYNAMIC(CMdfPwdDlg, CDialogEx)

void MdfPwdCallback(void* p,string strData)
{
	USES_CONVERSION;
	CMdfPwdDlg* pThis=(CMdfPwdDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_MDFPWD_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CMdfPwdDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_MDFPWD:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"修改失败！",L"错误");
			else
			{
				CString strNewPwd;
				GetDlgItemText(IDC_NEWPWD, strNewPwd);
				g_Globle.m_strUserPwd = strNewPwd;
				MessageBox(L"修改成功！");
				CDialogEx::OnOK();
			}
		}
		break;
	}
	return TRUE;
}

CMdfPwdDlg::CMdfPwdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMdfPwdDlg::IDD, pParent)
{
	m_edit_old_pass.m_type = EDIT_TYPE_NO_CHN;
	m_edit_new_pass.m_type = EDIT_TYPE_NO_CHN;
	m_edit_new_pass2.m_type = EDIT_TYPE_NO_CHN;

	//设置回调
	g_Globle.SetCallback(MdfPwdCallback,this);
}

CMdfPwdDlg::~CMdfPwdDlg()
{
}

void CMdfPwdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OLDPWD, m_edit_old_pass);
	DDX_Control(pDX, IDC_NEWPWD, m_edit_new_pass);
	DDX_Control(pDX, IDC_NEWPWD2, m_edit_new_pass2);
}


BEGIN_MESSAGE_MAP(CMdfPwdDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMdfPwdDlg::OnBnClickedOk)
	ON_MESSAGE(WM_MDFPWD_CALL, &CMdfPwdDlg::OnCallBack)
END_MESSAGE_MAP()


// CMdfPwdDlg 消息处理程序

void CMdfPwdDlg::SendToMdfPwd(CString strName,CString strPwd)
{
	USES_CONVERSION;

	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_MDFPWD;
	root[CMD_MDFPWD[EM_MDFPWD_NAME]]=T2A(strName);
	root[CMD_MDFPWD[EM_MDFPWD_PWD]]=T2A(strPwd);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CMdfPwdDlg::OnBnClickedOk()
{
	CString strOldPwd, strNewPwd, strNewPwd2;
	GetDlgItemText(IDC_OLDPWD,strOldPwd);
	GetDlgItemText(IDC_NEWPWD, strNewPwd);
	GetDlgItemText(IDC_NEWPWD2, strNewPwd2);

	if (strOldPwd.IsEmpty())
	{
		MessageBox(L"请输入密码");
		return;
	}
	else if (strOldPwd!=g_Globle.m_strUserPwd)
	{
		MessageBox(L"原密码输入错误！");
		return;
	}
	else if (strNewPwd.IsEmpty())
	{
		MessageBox(L"请输入新密码");
		return;
	}
	else if (strNewPwd2.IsEmpty())
	{
		MessageBox(L"请输入确认密码");
		return;
	}
	else if (strNewPwd!=strNewPwd2)
	{
		MessageBox(L"新密码不一致，请重新输入");
		return;
	}
	else
	{
		SendToMdfPwd(g_Globle.m_strUserName, strNewPwd);
	}
}
