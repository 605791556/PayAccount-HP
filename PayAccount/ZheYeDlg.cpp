// ZheYeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "ZheYeDlg.h"
#include "afxdialogex.h"


// CZheYeDlg 对话框

IMPLEMENT_DYNAMIC(CZheYeDlg, CSetPayDlg)


void ZheyePayCallback(void* p,string strData)
{
	USES_CONVERSION;
	CZheYeDlg* pThis=(CZheYeDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_ZHEYEPAY_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CZheYeDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_ZHEYEPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取数据失败！",L"错误");
			else
			{
				GetZheyePay(root);
			}
		}
		break;
	case SOCK_CMD_SET_ZHEYEPAY:
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

CZheYeDlg::CZheYeDlg(CWnd* pParent /*=NULL*/)
: CSetPayDlg(pParent)
{
	m_a_q4.m_type = EDIT_TYPE_FLOAT;
	m_a_4.m_type = EDIT_TYPE_FLOAT;
	m_a_d3.m_type = EDIT_TYPE_FLOAT;
	m_a_3.m_type = EDIT_TYPE_FLOAT;
	m_a_2.m_type = EDIT_TYPE_FLOAT;

	m_sf_q4.m_type = EDIT_TYPE_FLOAT;
	m_sf_4.m_type = EDIT_TYPE_FLOAT;
	m_sf_d3.m_type = EDIT_TYPE_FLOAT;
	m_sf_3.m_type = EDIT_TYPE_FLOAT;
	m_sf_2.m_type = EDIT_TYPE_FLOAT;

	m_td_q4.m_type = EDIT_TYPE_FLOAT;
	m_td_4.m_type = EDIT_TYPE_FLOAT;
	m_td_d3.m_type = EDIT_TYPE_FLOAT;
	m_td_3.m_type = EDIT_TYPE_FLOAT;
	m_td_2.m_type = EDIT_TYPE_FLOAT;

	m_a_q4.m_maxLen = 10;
	m_a_4.m_maxLen = 10;
	m_a_d3.m_maxLen = 10;
	m_a_3.m_maxLen = 10;
	m_a_2.m_maxLen = 10;

	m_sf_q4.m_maxLen = 10;
	m_sf_4.m_maxLen = 10;
	m_sf_d3.m_maxLen = 10;
	m_sf_3.m_maxLen = 10;
	m_sf_2.m_maxLen = 10;

	m_td_q4.m_maxLen = 10;
	m_td_4.m_maxLen = 10;
	m_td_d3.m_maxLen = 10;
	m_td_3.m_maxLen = 10;
	m_td_2.m_maxLen = 10;
}

CZheYeDlg::~CZheYeDlg()
{
}

void CZheYeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_A_Q4, m_a_q4);
	DDX_Control(pDX, IDC_A_4, m_a_4);
	DDX_Control(pDX, IDC_A_D3, m_a_d3);
	DDX_Control(pDX, IDC_A_3, m_a_3);
	DDX_Control(pDX, IDC_A_2, m_a_2);

	DDX_Control(pDX, IDC_SF_Q4, m_sf_q4);
	DDX_Control(pDX, IDC_SF_4, m_sf_4);
	DDX_Control(pDX, IDC_SF_D3, m_sf_d3);
	DDX_Control(pDX, IDC_SF_3, m_sf_3);
	DDX_Control(pDX, IDC_SF_2, m_sf_2);

	DDX_Control(pDX, IDC_TD_Q4, m_td_q4);
	DDX_Control(pDX, IDC_TD_4, m_td_4);
	DDX_Control(pDX, IDC_TD_D3, m_td_d3);
	DDX_Control(pDX, IDC_TD_3, m_td_3);
	DDX_Control(pDX, IDC_TD_2, m_td_2);
}


BEGIN_MESSAGE_MAP(CZheYeDlg, CSetPayDlg)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CZheYeDlg::OnBnClickedBtnSave)
	ON_MESSAGE(WM_ZHEYEPAY_CALL, &CZheYeDlg::OnCallBack)
END_MESSAGE_MAP()


// CZheYeDlg 消息处理程序
BOOL CZheYeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CZheYeDlg::SendToGetZheyePay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_ZHEYEPAY;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CZheYeDlg::SendToSaveZheyePay(STU_ZHEYE_PAY stu)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SET_ZHEYEPAY;

	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_AQ4]]=T2A(stu.strAQ4);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A4]]=T2A(stu.strA4);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_AD3]]=T2A(stu.strAD3);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A3]]=T2A(stu.strA3);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A2]]=T2A(stu.strA2);

	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_Q4]]=T2A(stu.str_sf_Q4);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_4]]=T2A(stu.str_sf_4);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_D3]]=T2A(stu.str_sf_D3);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_3]]=T2A(stu.str_sf_3);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_2]]=T2A(stu.str_sf_2);

	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_Q4]]=T2A(stu.str_td_Q4);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_4]]=T2A(stu.str_td_4);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_D3]]=T2A(stu.str_td_D3);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_3]]=T2A(stu.str_td_3);
	root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_2]]=T2A(stu.str_td_2);

	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CZheYeDlg::GetZheyePay(Json::Value root)
{
	STU_ZHEYE_PAY stu;
	stu.strAQ4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_AQ4]].asCString();
	stu.strA4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A4]].asCString();
	stu.strAD3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_AD3]].asCString();
	stu.strA3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A3]].asCString();
	stu.strA2 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A2]].asCString();

	stu.str_sf_Q4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_Q4]].asCString();
	stu.str_sf_4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_4]].asCString();
	stu.str_sf_D3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_D3]].asCString();
	stu.str_sf_3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_3]].asCString();
	stu.str_sf_2 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_2]].asCString();

	stu.str_td_Q4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_Q4]].asCString();
	stu.str_td_4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_4]].asCString();
	stu.str_td_D3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_D3]].asCString();
	stu.str_td_3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_3]].asCString();
	stu.str_td_2 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_2]].asCString();

	SetDlgItemText(IDC_A_Q4, stu.strAQ4);
	SetDlgItemText(IDC_A_4, stu.strA4);
	SetDlgItemText(IDC_A_D3, stu.strAD3);
	SetDlgItemText(IDC_A_3, stu.strA3);
	SetDlgItemText(IDC_A_2, stu.strA2);

	SetDlgItemText(IDC_SF_Q4, stu.str_sf_Q4);
	SetDlgItemText(IDC_SF_4, stu.str_sf_4);
	SetDlgItemText(IDC_SF_D3, stu.str_sf_D3);
	SetDlgItemText(IDC_SF_3, stu.str_sf_3);
	SetDlgItemText(IDC_SF_2, stu.str_sf_2);

	SetDlgItemText(IDC_TD_Q4, stu.str_td_Q4);
	SetDlgItemText(IDC_TD_4, stu.str_td_4);
	SetDlgItemText(IDC_TD_D3, stu.str_td_D3);
	SetDlgItemText(IDC_TD_3, stu.str_td_3);
	SetDlgItemText(IDC_TD_2, stu.str_td_2);

}

void CZheYeDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(ZheyePayCallback,this);
	SendToGetZheyePay();
}

void CZheYeDlg::OnBnClickedBtnSave()
{
	STU_ZHEYE_PAY stu;
	GetDlgItemText(IDC_A_Q4, stu.strAQ4);
	GetDlgItemText(IDC_A_4, stu.strA4);
	GetDlgItemText(IDC_A_D3, stu.strAD3);
	GetDlgItemText(IDC_A_3, stu.strA3);
	GetDlgItemText(IDC_A_2, stu.strA2);

	GetDlgItemText(IDC_SF_Q4, stu.str_sf_Q4);
	GetDlgItemText(IDC_SF_4, stu.str_sf_4);
	GetDlgItemText(IDC_SF_D3, stu.str_sf_D3);
	GetDlgItemText(IDC_SF_3, stu.str_sf_3);
	GetDlgItemText(IDC_SF_2, stu.str_sf_2);

	GetDlgItemText(IDC_TD_Q4, stu.str_td_Q4);
	GetDlgItemText(IDC_TD_4, stu.str_td_4);
	GetDlgItemText(IDC_TD_D3, stu.str_td_D3);
	GetDlgItemText(IDC_TD_3, stu.str_td_3);
	GetDlgItemText(IDC_TD_2, stu.str_td_2);

	if (stu.strAQ4.IsEmpty() || stu.strA4.IsEmpty() || stu.strAD3.IsEmpty() || stu.strA3.IsEmpty() || stu.strA2.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}
	if (stu.str_sf_Q4.IsEmpty() || stu.str_sf_4.IsEmpty() || stu.str_sf_D3.IsEmpty() || stu.str_sf_3.IsEmpty() || stu.str_sf_2.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}
	if (stu.str_td_Q4.IsEmpty() || stu.str_td_4.IsEmpty() || stu.str_td_D3.IsEmpty() || stu.str_td_3.IsEmpty() || stu.str_td_2.IsEmpty())
	{
		MessageBox(L"请输入完整");
		return;
	}
	SendToSaveZheyePay(stu);
}
