// AddUserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "AddUserDlg.h"
#include "afxdialogex.h"


void AddUserCallback(void* p,string strData)
{
	USES_CONVERSION;
	CAddUserDlg* pThis=(CAddUserDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_ADDUSER_CALL,NULL,(LPARAM)pStrData);
	}
}

// CAddUserDlg 对话框

IMPLEMENT_DYNAMIC(CAddUserDlg, CDialogEx)

CAddUserDlg::CAddUserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddUserDlg::IDD, pParent)
{
	m_edit_no_chn_name.m_type = EDIT_TYPE_NO_CHN;
	m_edit_no_chn_pass.m_type = EDIT_TYPE_NO_CHN;
	m_edit_no_chn_pass2.m_type = EDIT_TYPE_NO_CHN;

	//设置回调
	g_Globle.SetCallback(AddUserCallback,this);
}

CAddUserDlg::~CAddUserDlg()
{
}

void CAddUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NAME, m_edit_no_chn_name);
	DDX_Control(pDX, IDC_PASS, m_edit_no_chn_pass);
	DDX_Control(pDX, IDC_PASS2, m_edit_no_chn_pass);
}


BEGIN_MESSAGE_MAP(CAddUserDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddUserDlg::OnBnClickedOk)
	ON_MESSAGE(WM_ADDUSER_CALL, &CAddUserDlg::OnCallBack)
END_MESSAGE_MAP()


// CAddUserDlg 消息处理程序
LRESULT CAddUserDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_ADD_USER:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"添加失败！",L"错误");
			else
			{
				MessageBox(L"添加成功！");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_JUDGE_USER:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"判断账号是否存在时发生错误！",L"错误");
			else
			{
				bool bHad = root[CMD_JUDGEUSER[EM_JUDGE_USER_BHAD]].asBool();
				if(bHad)
					MessageBox(L"该账号已存在",L"提示");
				else
					SendToAddUser();
			}
		}
		break;
	}
	return TRUE;
}

void CAddUserDlg::SendToJudgeUser(CString strName)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_JUDGE_USER;
	root[CMD_JUDGEUSER[EM_JUDGE_USER_NAME]]=T2A(strName);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CAddUserDlg::SendToAddUser()
{
	USER_TYPE type = TYPE_MAX;
	CString strName,strPass,strPass2;
	GetDlgItemText(IDC_NAME, strName);
	GetDlgItemText(IDC_PASS, strPass);
	GetDlgItemText(IDC_PASS2, strPass2);

	int nCk=((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck();//普通用户
	int nCk2 = ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck();//管理员
	if(nCk)
		type = TYPE_COMMON;
	else if(nCk2)
		type = TYPE_MNG;

	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_ADD_USER;
	root[CMD_ADDUSER[EM_ADD_USER_NAME]]=T2A(strName);
	root[CMD_ADDUSER[EM_ADD_USER_PWD]]=T2A(strPass);
	root[CMD_ADDUSER[EM_ADD_USER_TYPE]]=type;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CAddUserDlg::OnBnClickedOk()
{
	CString strName,strPass,strPass2;
	GetDlgItemText(IDC_NAME, strName);
	GetDlgItemText(IDC_PASS, strPass);
	GetDlgItemText(IDC_PASS2, strPass2);

	int nCk=((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck();//普通用户
	int nCk2 = ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck();//管理员

	if (strName.IsEmpty())
	{
		MessageBox(L"请输入账号");
		return;
	}
	else if (strPass.IsEmpty())
	{
		MessageBox(L"请输入密码");
		return;
	}
	else if (strPass2.IsEmpty())
	{
		MessageBox(L"请输入确认密码");
		return;
	}
	else if (strPass!=strPass2)
	{
		MessageBox(L"密码不一致");
		return;
	}
	else if (nCk == 0 && nCk2 == 0)
	{
		MessageBox(L"请选择用户权限");
		return;
	}

	SendToJudgeUser(strName);
}
