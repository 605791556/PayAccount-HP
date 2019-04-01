// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "SetAdressDlg.h"


// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{
	m_edit_no_chn_pass.m_type = EDIT_TYPE_NO_CHN;
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER, m_comboUser);
	DDX_Control(pDX, IDC_PASS, m_edit_no_chn_pass);
	DDX_Control(pDX, IDOK, m_btnLogin);
	DDX_Control(pDX, IDC_STA_MSG, m_staMsg);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_USER, &CLoginDlg::OnCbnSelchangeUser)
	ON_CBN_EDITCHANGE(IDC_USER, &CLoginDlg::OnCbnEditchangeUser)
	ON_MESSAGE(WM_INITDLG, &CLoginDlg::OnInitDlg)
	ON_MESSAGE(WM_CALLBACK, &CLoginDlg::OnCallBack)
	ON_STN_CLICKED(IDC_STABTN, &CLoginDlg::OnStnClickedStabtn)
END_MESSAGE_MAP()


void LoginCallback(void* p,string strData)
{
	USES_CONVERSION;
	CLoginDlg* pThis=(CLoginDlg*) p;

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

// CLoginDlg 消息处理程序
DWORD WINAPI InitDlgThread(LPVOID lpParam);
BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	PostMessage(WM_INITDLG);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

DWORD WINAPI InitDlgThread(LPVOID lpParam)
{
	CLoginDlg* pThis = (CLoginDlg*)lpParam;

	//设置回调
	g_Globle.SetCallback(LoginCallback,pThis);
	if (g_Globle.InitGloble())
	{
		if(g_Globle.ConnectSer())
		{
			pThis->m_staMsg.SetWindowTextW(L"");
			Json::Value root;
			root[CONNECT_CMD]=SOCK_CMD_GET_USER;
			Json::FastWriter writer;  
			string temp = writer.write(root);
			g_Globle.SendTo(temp);
		}
		else
		    pThis->m_staMsg.SetWindowTextW(L"服务连接失败！");
	}
	pThis->m_btnLogin.ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CLoginDlg::OnInitDlg(WPARAM wParam, LPARAM lParam)
{
	Log("----------------------start----------------------");
	//g_SockClient.createWin();
	CreateThread(NULL,0,InitDlgThread,this,0,NULL);
	return TRUE;
}

LRESULT CLoginDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_USER:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取账号信息失败！",L"错误");
			else
				GetUser(root);
		}
		break;
	case SOCK_CMD_LOGIN:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"登录失败！",L"错误");
			else
				GetLogin(root);
		}
	}
	return TRUE;
}

void CLoginDlg::GetUser(Json::Value root)
{
	Json:: Value js = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
	if (js.isArray())
	{
		int nCount = js.size();
		for (int i=0;i<nCount;i++)
		{
			USER_STU stu;
			stu.strname = js[i][CMD_GETUSER[EM_USER_NAME]].asCString();
			stu.type = js[i][CMD_GETUSER[EM_USER_TYPE]].asInt();
			stu.strTime = js[i][CMD_GETUSER[EM_USER_TIME]].asCString();
			stu.show_pass = js[i][CMD_GETUSER[EM_USER_SHOWPASS]].asInt();
			stu.strPass = js[i][CMD_GETUSER[EM_USER_PASS]].asCString();
			m_vet.push_back(stu);
		}

		int ndex = 0;
		nCount = m_vet.size();
		for (int i = 0; i < nCount; i++)
		{
			if (m_vet[i].show_pass)
			{
				((CComboBox*)GetDlgItem(IDC_USER))->AddString(m_vet[i].strname);
				((CComboBox*)GetDlgItem(IDC_USER))->SetItemData(ndex,(DWORD_PTR)&m_vet[i]);
				ndex++;
			}
		}
	}
}

void CLoginDlg::GetLogin(Json::Value root)
{
	bool bOk = root[CMD_LOGINMSG[EM_LOGINMSG_BOK]].asBool();
	if (bOk)
	{
		g_Globle.m_strUserName = root[CMD_LOGINMSG[EM_LOGINMSG_NAME]].asCString();
		g_Globle.m_strUserPwd = root[CMD_LOGINMSG[EM_LOGINMSG_PASS]].asCString();
		g_Globle.m_nType = root[CMD_LOGINMSG[EM_LOGINMSG_TYPE]].asInt();
		g_Globle.SetCallback(NULL,NULL);
		OnOK();
	}
	else
	{
		bool bLogind = root[CMD_LOGINMSG[EM_LOGINMSG_LOGIND]].asBool();
		if(bLogind)
			MessageBox(L"该账号已登录！",L"提示");
		else
		    MessageBox(L"账号或密码错误，请重新输入！",L"提示");
		return;
	}
}

void CLoginDlg::OnBnClickedOk()
{
	CString strName, strPass;
	GetDlgItemText(IDC_USER, strName);
	GetDlgItemText(IDC_PASS, strPass);
	int nCheck = ((CButton*)GetDlgItem(IDC_CK_PASS))->GetCheck();

	if (strName.IsEmpty())
	{
		MessageBox(L"请输入账号",L"提示");
		return;
	}
	else if (strPass.IsEmpty())
	{
		MessageBox(L"请输入密码",L"提示");
		return;
	}
	else
	{
		SendToLogin(strName,strPass,nCheck);
	}
}

void CLoginDlg::SendToLogin(CString strName,CString strPass,int show_pass)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_LOGIN;
	root[CMD_LOGIN[EM_LOGIN_NAME]] =T2A(strName);
	root[CMD_LOGIN[EM_LOGIN_PASS]]=T2A(strPass);
	root[CMD_LOGIN[EM_LOGIN_SHOWPASS]]=show_pass;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CLoginDlg::OnCbnSelchangeUser()
{
	CString strName;
	int idex = ((CComboBox*)GetDlgItem(IDC_USER))->GetCurSel();
	((CComboBox*)GetDlgItem(IDC_USER))->GetLBText(idex, strName);
	

	int ndex = -1;
	int nCount = m_vet.size();
	for (int i = 0; i < nCount; i++)
	{
		if (strName == m_vet[i].strname)
		{
			ndex = i;
			break;
		}
	}
	if (ndex >= 0)
	{
		if (m_vet[ndex].show_pass)
		{
			//SetDlgItemText(IDC_PASS, m_vet[ndex].strPass);
			((CButton*)GetDlgItem(IDC_CK_PASS))->SetCheck(1);
		}
		else
		{
			//SetDlgItemText(IDC_PASS, L"");
			((CButton*)GetDlgItem(IDC_CK_PASS))->SetCheck(0);
		}
	}
}


void CLoginDlg::OnCbnEditchangeUser()
{
	
}


void CLoginDlg::OnStnClickedStabtn()
{
	CSetAdressDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		OnCancel();
	}
}
