// AddProDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "AddProDlg.h"
#include "afxdialogex.h"


// CAddProDlg 对话框

IMPLEMENT_DYNAMIC(CAddProDlg, CDialogEx)

void AddProCallback(void* p,string strData)
{
	USES_CONVERSION;
	CAddProDlg* pThis=(CAddProDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_ADDPRO_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CAddProDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_ADD_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"添加失败！",L"错误");
			else
			{
				MessageBox(L"添加成功",L"提示");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_MDF_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"修改失败！",L"错误");
			else
			{
				MessageBox(L"修改成功",L"提示");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_JUDGE_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"判断项目是否存在时发生错误！",L"提示");
			else
			{
				bool bHad = root[CMD_JUDGEPROJECT[EM_JUDGE_PROJECT_BHAD]].asBool();
				if(bHad)
				{
					MessageBox(L"该项目名称已存在！",L"提示");
				}
				else
				{
					SendToAddOrMdfProject();	
				}
			}
		}
		break;
	}
	return TRUE;
}

CAddProDlg::CAddProDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddProDlg::IDD, pParent)
{
	m_dlg=NULL;
	m_bAdd=true;
}

CAddProDlg::CAddProDlg(CProjectMngDlg* dlg)
{
	m_dlg=dlg;
	m_bAdd=false;
}

CAddProDlg::~CAddProDlg()
{
}

void CAddProDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAddProDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CAddProDlg::OnBnClickedBtnSave)
	ON_MESSAGE(WM_ADDPRO_CALL, &CAddProDlg::OnCallBack)
END_MESSAGE_MAP()


// CAddProDlg 消息处理程序


BOOL CAddProDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_dlg)
	{
		SetModifyInit();
	}
	//设置回调
	g_Globle.SetCallback(AddProCallback,this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CAddProDlg::SetNotifyWnd(CProjectMngDlg* dlg)
{
	m_dlg=dlg;
	m_bAdd=false;
}

void CAddProDlg::SendToJudgePro(CString strProName)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_JUDGE_PROJECT;
	root[CMD_JUDGEBOOK[EM_JUDGE_PROJECT_NAME]]=T2A(strProName);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CAddProDlg::SendToAddOrMdfProject(EM_SOCK_CMD type)
{
	CString strName;
	GetDlgItemText(IDC_EDIT_NAME,strName);

	PRO_NUM_TYPE pro_num_type = PRO_NUM_TYPE_MAX;
	if(((CButton*)GetDlgItem(IDC_RDO_YS))->GetCheck())
		pro_num_type=PRO_NUM_TYPE_YS;
	else if(((CButton*)GetDlgItem(IDC_RDO_LS))->GetCheck())
		pro_num_type=PRO_NUM_TYPE_LS;

	PRO_STAFF_TYPE pro_staff_type = PRO_STAFF_TYPE_MAX;
	if(((CButton*)GetDlgItem(IDC_RDO_Y))->GetCheck())
		pro_staff_type=PRO_STAFF_TYPE_YES;
	else if(((CButton*)GetDlgItem(IDC_RDO_N))->GetCheck())
		pro_staff_type=PRO_STAFF_TYPE_NO;

	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=type;
	root[CMD_PROMSG[EM_PROMSG_NAME]]=T2A(strName);
	root[CMD_PROMSG[EM_PROMSG_NUM_TYPE]]=pro_num_type;
	root[CMD_PROMSG[EM_PROMSG_BWRITE]]=pro_staff_type;
	if (type == SOCK_CMD_MDF_PROJECT)
	{
		PROJECT_STU stu = *((PROJECT_STU*)m_dlg->m_ListCtrl.GetItemData(m_dlg->m_nItem));
		root[CMD_PROMSG[EM_PROMSG_ID]]=stu.nID;
	}
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CAddProDlg::SetModifyInit()
{
	PROJECT_STU stu = *((PROJECT_STU*)m_dlg->m_ListCtrl.GetItemData(m_dlg->m_nItem));
	//项目名称
	SetDlgItemText(IDC_EDIT_NAME,stu.strName);
	//如果是前8项，则名称不可修改
	if (m_dlg->m_nItem<8)
	{
		GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
	}
	//做工范围类型
	if(stu.pn_type == PRO_NUM_TYPE_YS)
		((CButton*)GetDlgItem(IDC_RDO_YS))->SetCheck(TRUE);
	else if(stu.pn_type == PRO_NUM_TYPE_LS)
		((CButton*)GetDlgItem(IDC_RDO_LS))->SetCheck(TRUE);
	//项目进度工人填入
	if(stu.ps_type == PRO_STAFF_TYPE_YES)
		((CButton*)GetDlgItem(IDC_RDO_Y))->SetCheck(TRUE);
	else if(stu.ps_type == PRO_STAFF_TYPE_NO)
		((CButton*)GetDlgItem(IDC_RDO_N))->SetCheck(TRUE);
}

void CAddProDlg::OnBnClickedBtnSave()
{
	CString strName;
	GetDlgItemText(IDC_EDIT_NAME,strName);

	PRO_NUM_TYPE pro_num_type = PRO_NUM_TYPE_MAX;
	if(((CButton*)GetDlgItem(IDC_RDO_YS))->GetCheck())
		pro_num_type=PRO_NUM_TYPE_YS;
	else if(((CButton*)GetDlgItem(IDC_RDO_LS))->GetCheck())
		pro_num_type=PRO_NUM_TYPE_LS;

	PRO_STAFF_TYPE pro_staff_type = PRO_STAFF_TYPE_MAX;
	if(((CButton*)GetDlgItem(IDC_RDO_Y))->GetCheck())
		pro_staff_type=PRO_STAFF_TYPE_YES;
	else if(((CButton*)GetDlgItem(IDC_RDO_N))->GetCheck())
		pro_staff_type=PRO_STAFF_TYPE_NO;

	if (strName.IsEmpty() || pro_num_type ==  PRO_NUM_TYPE_MAX || pro_staff_type==PRO_STAFF_TYPE_MAX)
	{
		MessageBox(L"填写信息不完整",L"提示");
		return;
	}
	if (m_bAdd)
		SendToJudgePro(strName);
	else
		SendToAddOrMdfProject(SOCK_CMD_MDF_PROJECT);
}
