// AddUserDlg.cpp : ʵ���ļ�
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

// CAddUserDlg �Ի���

IMPLEMENT_DYNAMIC(CAddUserDlg, CDialogEx)

CAddUserDlg::CAddUserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddUserDlg::IDD, pParent)
{
	m_edit_no_chn_name.m_type = EDIT_TYPE_NO_CHN;
	m_edit_no_chn_pass.m_type = EDIT_TYPE_NO_CHN;
	m_edit_no_chn_pass2.m_type = EDIT_TYPE_NO_CHN;

	//���ûص�
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


// CAddUserDlg ��Ϣ�������
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
				MessageBox(L"���ʧ�ܣ�",L"����");
			else
			{
				MessageBox(L"��ӳɹ���");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_JUDGE_USER:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"�ж��˺��Ƿ����ʱ��������",L"����");
			else
			{
				bool bHad = root[CMD_JUDGEUSER[EM_JUDGE_USER_BHAD]].asBool();
				if(bHad)
					MessageBox(L"���˺��Ѵ���",L"��ʾ");
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

	int nCk=((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck();//��ͨ�û�
	int nCk2 = ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck();//����Ա
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

	int nCk=((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck();//��ͨ�û�
	int nCk2 = ((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck();//����Ա

	if (strName.IsEmpty())
	{
		MessageBox(L"�������˺�");
		return;
	}
	else if (strPass.IsEmpty())
	{
		MessageBox(L"����������");
		return;
	}
	else if (strPass2.IsEmpty())
	{
		MessageBox(L"������ȷ������");
		return;
	}
	else if (strPass!=strPass2)
	{
		MessageBox(L"���벻һ��");
		return;
	}
	else if (nCk == 0 && nCk2 == 0)
	{
		MessageBox(L"��ѡ���û�Ȩ��");
		return;
	}

	SendToJudgeUser(strName);
}
