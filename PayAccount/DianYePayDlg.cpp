// DianYePayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "DianYePayDlg.h"
#include "afxdialogex.h"


// CDianYePayDlg 对话框

IMPLEMENT_DYNAMIC(CDianYePayDlg, CSetPayDlg)

void DyPayCallback(void* p,string strData)
{
	USES_CONVERSION;
	CDianYePayDlg* pThis=(CDianYePayDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_DYPAY_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CDianYePayDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_DYPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取数据失败！",L"错误");
			else
			{
				GetDyPay(root);
			}
		}
		break;
	case SOCK_CMD_SET_DYPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"保存数据失败！",L"错误");
			else
			{
				MessageBox(L"保存成功！",L"");
			}
		}
		break;
	}
	return TRUE;
}

CDianYePayDlg::CDianYePayDlg(CWnd* pParent /*=NULL*/)
	: CSetPayDlg(pParent)
{
	m_w_down.m_type = EDIT_TYPE_FLOAT;
	m_w_up.m_type = EDIT_TYPE_FLOAT;
	m_w_down.m_maxLen = 10;
	m_w_up.m_maxLen = 10;
}

CDianYePayDlg::~CDianYePayDlg()
{
}

void CDianYePayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_W_DOWN, m_w_down);
	DDX_Control(pDX, IDC_W_UP, m_w_up);
}


BEGIN_MESSAGE_MAP(CDianYePayDlg, CSetPayDlg)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDianYePayDlg::OnBnClickedBtnSave)
	ON_MESSAGE(WM_DYPAY_CALL, &CDianYePayDlg::OnCallBack)
END_MESSAGE_MAP()


// CDianYePayDlg 消息处理程序

BOOL CDianYePayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CDianYePayDlg::SendToGetDyPay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_DYPAY;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CDianYePayDlg::SendToSaveDyPay(CString strDown,CString strUp)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SET_DYPAY;

	root[CMD_DYPAY[EM_DY_PAY_DOWN]]=T2A(strDown);
	root[CMD_DYPAY[EM_DY_PAY_UP]]=T2A(strUp);

	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CDianYePayDlg::GetDyPay(Json::Value root)
{
	CString strdown,strup;
	strdown = root[CMD_DYPAY[EM_DY_PAY_DOWN]].asCString();
	strup = root[CMD_DYPAY[EM_DY_PAY_UP]].asCString();

	SetDlgItemText(IDC_W_DOWN, strdown);
	SetDlgItemText(IDC_W_UP, strup);
}

void CDianYePayDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(DyPayCallback,this);
	SendToGetDyPay();
}

void CDianYePayDlg::OnBnClickedBtnSave()
{
	CString strdown,strup;
	GetDlgItemText(IDC_W_DOWN, strdown);
	GetDlgItemText(IDC_W_UP, strup);

	if (strdown.IsEmpty() || strup.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}
	SendToSaveDyPay(strdown,strup);
}
