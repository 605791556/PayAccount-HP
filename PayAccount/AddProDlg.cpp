// AddProDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PayAccount.h"
#include "AddProDlg.h"
#include "afxdialogex.h"


// CAddProDlg �Ի���

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
				MessageBox(L"���ʧ�ܣ�",L"����");
			else
			{
				MessageBox(L"��ӳɹ�",L"��ʾ");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_MDF_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"�޸�ʧ�ܣ�",L"����");
			else
			{
				MessageBox(L"�޸ĳɹ�",L"��ʾ");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_JUDGE_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"�ж���Ŀ�Ƿ����ʱ��������",L"��ʾ");
			else
			{
				bool bHad = root[CMD_JUDGEPROJECT[EM_JUDGE_PROJECT_BHAD]].asBool();
				if(bHad)
				{
					MessageBox(L"����Ŀ�����Ѵ��ڣ�",L"��ʾ");
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


// CAddProDlg ��Ϣ�������


BOOL CAddProDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if (m_dlg)
	{
		SetModifyInit();
	}
	//���ûص�
	g_Globle.SetCallback(AddProCallback,this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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
		MessageBox(L"������������ʧ�ܣ����������Ƿ�������",L"��ʾ");
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
		MessageBox(L"������������ʧ�ܣ����������Ƿ�������",L"��ʾ");
	}
}

void CAddProDlg::SetModifyInit()
{
	PROJECT_STU stu = *((PROJECT_STU*)m_dlg->m_ListCtrl.GetItemData(m_dlg->m_nItem));
	//��Ŀ����
	SetDlgItemText(IDC_EDIT_NAME,stu.strName);
	//�����ǰ8������Ʋ����޸�
	if (m_dlg->m_nItem<8)
	{
		GetDlgItem(IDC_EDIT_NAME)->EnableWindow(FALSE);
	}
	//������Χ����
	if(stu.pn_type == PRO_NUM_TYPE_YS)
		((CButton*)GetDlgItem(IDC_RDO_YS))->SetCheck(TRUE);
	else if(stu.pn_type == PRO_NUM_TYPE_LS)
		((CButton*)GetDlgItem(IDC_RDO_LS))->SetCheck(TRUE);
	//��Ŀ���ȹ�������
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
		MessageBox(L"��д��Ϣ������",L"��ʾ");
		return;
	}
	if (m_bAdd)
		SendToJudgePro(strName);
	else
		SendToAddOrMdfProject(SOCK_CMD_MDF_PROJECT);
}
