// DaiNumberPayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "DaiNumberPayDlg.h"
#include "afxdialogex.h"


// CDaiNumberPayDlg 对话框

IMPLEMENT_DYNAMIC(CDaiNumberPayDlg, CSetPayDlg)


void DaiPayCallback(void* p,string strData)
{
	USES_CONVERSION;
	CDaiNumberPayDlg* pThis=(CDaiNumberPayDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_JDPAY_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CDaiNumberPayDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_DAIPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取数据失败！",L"错误");
			else
			{
				GetDaiPay(root);
			}
		}
		break;
	case SOCK_CMD_SET_DAIPAY:
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

CDaiNumberPayDlg::CDaiNumberPayDlg(CWnd* pParent /*=NULL*/)
: CSetPayDlg(pParent)
{
	m_edit_a_w.m_type = EDIT_TYPE_FLOAT;
	m_edit_a_2.m_type = EDIT_TYPE_FLOAT;
	m_edit_a_2_5.m_type = EDIT_TYPE_FLOAT;
	m_edit_a_5_9.m_type = EDIT_TYPE_FLOAT;
	m_edit_a_10.m_type = EDIT_TYPE_FLOAT;
	m_edit_a_18.m_type = EDIT_TYPE_FLOAT;

	m_edit_sf_w.m_type = EDIT_TYPE_FLOAT;
	m_edit_sf_2.m_type = EDIT_TYPE_FLOAT;
	m_edit_sf_2_5.m_type = EDIT_TYPE_FLOAT;
	m_edit_sf_5_9.m_type = EDIT_TYPE_FLOAT;
	m_edit_sf_10.m_type = EDIT_TYPE_FLOAT;
	m_edit_sf_18.m_type = EDIT_TYPE_FLOAT;

	m_edit_td_w.m_type = EDIT_TYPE_FLOAT;
	m_edit_td_2.m_type = EDIT_TYPE_FLOAT;
	m_edit_td_2_5.m_type = EDIT_TYPE_FLOAT;
	m_edit_td_5_9.m_type = EDIT_TYPE_FLOAT;
	m_edit_td_10.m_type = EDIT_TYPE_FLOAT;
	m_edit_td_18.m_type = EDIT_TYPE_FLOAT;

	m_edit_a_w.m_maxLen = 10;
	m_edit_a_2.m_maxLen = 10;
	m_edit_a_2_5.m_maxLen = 10;
	m_edit_a_5_9.m_maxLen = 10;
	m_edit_a_10.m_maxLen = 10;
	m_edit_a_18.m_maxLen = 10;

	m_edit_sf_w.m_maxLen = 10;
	m_edit_sf_2.m_maxLen = 10;
	m_edit_sf_2_5.m_maxLen = 10;
	m_edit_sf_5_9.m_maxLen = 10;
	m_edit_sf_10.m_maxLen = 10;
	m_edit_sf_18.m_maxLen = 10;

	m_edit_td_w.m_maxLen = 10;
	m_edit_td_2.m_maxLen = 10;
	m_edit_td_2_5.m_maxLen = 10;
	m_edit_td_5_9.m_maxLen = 10;
	m_edit_td_10.m_maxLen = 10;
	m_edit_td_18.m_maxLen = 10;
}

CDaiNumberPayDlg::~CDaiNumberPayDlg()
{
}

void CDaiNumberPayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_A_W, m_edit_a_w);
	DDX_Control(pDX, IDC_EDIT_A_2, m_edit_a_2);
	DDX_Control(pDX, IDC_EDIT_A_2_5, m_edit_a_2_5);
	DDX_Control(pDX, IDC_EDIT_A_5_9, m_edit_a_5_9);
	DDX_Control(pDX, IDC_EDIT_A_10, m_edit_a_10);
	DDX_Control(pDX, IDC_EDIT_A_18, m_edit_a_18);

	DDX_Control(pDX, IDC_EDIT_SF_W, m_edit_sf_w);
	DDX_Control(pDX, IDC_EDIT_SF_2, m_edit_sf_2);
	DDX_Control(pDX, IDC_EDIT_SF_2_5, m_edit_sf_2_5);
	DDX_Control(pDX, IDC_EDIT_SF_5_9, m_edit_sf_5_9);
	DDX_Control(pDX, IDC_EDIT_SF_10, m_edit_sf_10);
	DDX_Control(pDX, IDC_EDIT_SF_18, m_edit_sf_18);

	DDX_Control(pDX, IDC_EDIT_TD_W, m_edit_td_w);
	DDX_Control(pDX, IDC_EDIT_TD_2, m_edit_td_2);
	DDX_Control(pDX, IDC_EDIT_TD_2_5, m_edit_td_2_5);
	DDX_Control(pDX, IDC_EDIT_TD_5_9, m_edit_td_5_9);
	DDX_Control(pDX, IDC_EDIT_TD_10, m_edit_td_10);
	DDX_Control(pDX, IDC_EDIT_TD_18, m_edit_td_18);
}


BEGIN_MESSAGE_MAP(CDaiNumberPayDlg, CSetPayDlg)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDaiNumberPayDlg::OnBnClickedBtnSave)
	ON_MESSAGE(WM_JDPAY_CALL, &CDaiNumberPayDlg::OnCallBack)
END_MESSAGE_MAP()


// CDaiNumberPayDlg 消息处理程序

BOOL CDaiNumberPayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CDaiNumberPayDlg::SendToGetDaiPay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_DAIPAY;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CDaiNumberPayDlg::GetDaiPay(Json::Value root)
{
	STU_DAI_PAY stu;
	stu.strA_w = root[CMD_DAIPAY[EM_DAI_PAY_A_W]].asCString();
	stu.strSf_w = root[CMD_DAIPAY[EM_DAI_PAY_Sf_w]].asCString();
	stu.strTd_w = root[CMD_DAIPAY[EM_DAI_PAY_Td_w]].asCString();

	stu.strA_2 = root[CMD_DAIPAY[EM_DAI_PAY_2]].asCString();
	stu.strSf_2 = root[CMD_DAIPAY[EM_DAI_PAY_Sf_2]].asCString();
	stu.strTd_2 = root[CMD_DAIPAY[EM_DAI_PAY_Td_2]].asCString();

	stu.strA_2_5 = root[CMD_DAIPAY[EM_DAI_PAY_2_5]].asCString();
	stu.strSf_2_5 = root[CMD_DAIPAY[EM_DAI_PAY_Sf_2_5]].asCString();
	stu.strTd_2_5 = root[CMD_DAIPAY[EM_DAI_PAY_Td_2_5]].asCString();

	stu.strA_5_9 = root[CMD_DAIPAY[EM_DAI_PAY_5_9]].asCString();
	stu.strSf_5_9 = root[CMD_DAIPAY[EM_DAI_PAY_Sf_5_9]].asCString();
	stu.strTd_5_9 = root[CMD_DAIPAY[EM_DAI_PAY_Td_5_9]].asCString();

	stu.strA_10 = root[CMD_DAIPAY[EM_DAI_PAY_10]].asCString();
	stu.strSf_10 = root[CMD_DAIPAY[EM_DAI_PAY_Sf_10]].asCString();
	stu.strTd_10 = root[CMD_DAIPAY[EM_DAI_PAY_Td_10]].asCString();

	stu.strA_18 = root[CMD_DAIPAY[EM_DAI_PAY_18]].asCString();
	stu.strSf_18 = root[CMD_DAIPAY[EM_DAI_PAY_Sf_18]].asCString();
	stu.strTd_18 = root[CMD_DAIPAY[EM_DAI_PAY_Td_18]].asCString();

	SetDlgItemText(IDC_EDIT_A_W, stu.strA_w);
	SetDlgItemText(IDC_EDIT_A_2, stu.strA_2);
	SetDlgItemText(IDC_EDIT_A_2_5, stu.strA_2_5);
	SetDlgItemText(IDC_EDIT_A_5_9,stu.strA_5_9);
	SetDlgItemText(IDC_EDIT_A_10, stu.strA_10);
	SetDlgItemText(IDC_EDIT_A_18, stu.strA_18);

	SetDlgItemText(IDC_EDIT_SF_W, stu.strSf_w);
	SetDlgItemText(IDC_EDIT_SF_2, stu.strSf_2);
	SetDlgItemText(IDC_EDIT_SF_2_5, stu.strSf_2_5);
	SetDlgItemText(IDC_EDIT_SF_5_9,stu.strSf_5_9);
	SetDlgItemText(IDC_EDIT_SF_10, stu.strSf_10);
	SetDlgItemText(IDC_EDIT_SF_18, stu.strSf_18);
	SetDlgItemText(IDC_EDIT_TD_W, stu.strTd_w);
	SetDlgItemText(IDC_EDIT_TD_2, stu.strTd_2);
	SetDlgItemText(IDC_EDIT_TD_2_5, stu.strTd_2_5);
	SetDlgItemText(IDC_EDIT_TD_5_9, stu.strTd_5_9);
	SetDlgItemText(IDC_EDIT_TD_10, stu.strTd_10);
	SetDlgItemText(IDC_EDIT_TD_18, stu.strTd_18);
}

void CDaiNumberPayDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(DaiPayCallback,this);
	SendToGetDaiPay();
}

void CDaiNumberPayDlg::SendToSaveDaiPay(STU_DAI_PAY stu)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SET_DAIPAY;

	root[CMD_DAIPAY[EM_DAI_PAY_A_W]]=T2A(stu.strA_w);
	root[CMD_DAIPAY[EM_DAI_PAY_Sf_w]]=T2A(stu.strSf_w);
	root[CMD_DAIPAY[EM_DAI_PAY_Td_w]] = T2A(stu.strTd_w);

	root[CMD_DAIPAY[EM_DAI_PAY_2]]=T2A(stu.strA_2);
	root[CMD_DAIPAY[EM_DAI_PAY_Sf_2]]=T2A(stu.strSf_2);
	root[CMD_DAIPAY[EM_DAI_PAY_Td_2]] = T2A(stu.strTd_2);

	root[CMD_DAIPAY[EM_DAI_PAY_2_5]]=T2A(stu.strA_2_5);
	root[CMD_DAIPAY[EM_DAI_PAY_Sf_2_5]]=T2A(stu.strSf_2_5);
	root[CMD_DAIPAY[EM_DAI_PAY_Td_2_5]] =T2A(stu.strTd_2_5);

	root[CMD_DAIPAY[EM_DAI_PAY_5_9]]=T2A(stu.strA_5_9);
	root[CMD_DAIPAY[EM_DAI_PAY_Sf_5_9]]=T2A(stu.strSf_5_9);
	root[CMD_DAIPAY[EM_DAI_PAY_Td_5_9]] =T2A(stu.strTd_5_9);

	root[CMD_DAIPAY[EM_DAI_PAY_10]]=T2A(stu.strA_10);
	root[CMD_DAIPAY[EM_DAI_PAY_Sf_10]]=T2A(stu.strSf_10);
	root[CMD_DAIPAY[EM_DAI_PAY_Td_10]] =T2A(stu.strTd_10);

	root[CMD_DAIPAY[EM_DAI_PAY_18]]=T2A(stu.strA_18);
	root[CMD_DAIPAY[EM_DAI_PAY_Sf_18]]=T2A(stu.strSf_18);
	root[CMD_DAIPAY[EM_DAI_PAY_Td_18]] =T2A(stu.strTd_18);

	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CDaiNumberPayDlg::OnBnClickedBtnSave()
{
	STU_DAI_PAY stu;
	GetDlgItemText(IDC_EDIT_A_W, stu.strA_w);
	GetDlgItemText(IDC_EDIT_A_2, stu.strA_2);
	GetDlgItemText(IDC_EDIT_A_2_5, stu.strA_2_5);
	GetDlgItemText(IDC_EDIT_A_5_9, stu.strA_5_9);
	GetDlgItemText(IDC_EDIT_A_10, stu.strA_10);
	GetDlgItemText(IDC_EDIT_A_18, stu.strA_18);
	GetDlgItemText(IDC_EDIT_SF_W, stu.strSf_w);
	GetDlgItemText(IDC_EDIT_SF_2, stu.strSf_2);
	GetDlgItemText(IDC_EDIT_SF_2_5, stu.strSf_2_5);
	GetDlgItemText(IDC_EDIT_SF_5_9, stu.strSf_5_9);
	GetDlgItemText(IDC_EDIT_SF_10, stu.strSf_10);
	GetDlgItemText(IDC_EDIT_SF_18, stu.strSf_18);
	GetDlgItemText(IDC_EDIT_TD_W, stu.strTd_w);
	GetDlgItemText(IDC_EDIT_TD_2, stu.strTd_2);
	GetDlgItemText(IDC_EDIT_TD_2_5, stu.strTd_2_5);
	GetDlgItemText(IDC_EDIT_TD_5_9, stu.strTd_5_9);
	GetDlgItemText(IDC_EDIT_TD_10, stu.strTd_10);
	GetDlgItemText(IDC_EDIT_TD_18, stu.strTd_18);

	if (stu.strA_w.IsEmpty() || stu.strA_2.IsEmpty() || stu.strA_2_5.IsEmpty() || stu.strA_5_9.IsEmpty() || stu.strA_10.IsEmpty() || stu.strA_18.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}
	if (stu.strSf_w.IsEmpty() || stu.strSf_2.IsEmpty() || stu.strSf_2_5.IsEmpty() || stu.strSf_5_9.IsEmpty() || stu.strSf_10.IsEmpty() || stu.strSf_18.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}
	if (stu.strTd_w.IsEmpty() || stu.strTd_2.IsEmpty() || stu.strTd_2_5.IsEmpty() || stu.strTd_5_9.IsEmpty() || stu.strTd_10.IsEmpty() || stu.strTd_18.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}

	SendToSaveDaiPay(stu);
}
