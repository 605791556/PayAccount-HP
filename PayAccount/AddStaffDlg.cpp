// AddStaffDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "..\common\globle.h"
#include "PayAccount.h"
#include "AddStaffDlg.h"


// CAddStaffDlg 对话框

IMPLEMENT_DYNAMIC(CAddStaffDlg, CDialog)

void AddStaffCallback(void* p,string strData)
{
	USES_CONVERSION;
	CAddStaffDlg* pThis=(CAddStaffDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_ADDSTAFF_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CAddStaffDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_ADD_STAFF:
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
	case SOCK_CMD_MDF_STAFF:
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
	case SOCK_CMD_JUDGE_STAFF:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"判断职工是否存在时发生错误！",L"提示");
			else
			{
				bool bHad = root[CMD_JUDGESTAFF[EM_JUDGE_STAFF_BHAD]].asBool();
				if(bHad)
					MessageBox(L"该身份证职工已存在！",L"提示");
				else
				{
					SendToAddStaff();	
				}
			}
		}
		break;
	}
	return TRUE;
}

CAddStaffDlg::CAddStaffDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAddStaffDlg::IDD, pParent)
{
	m_dlg=NULL;
	m_bAdd=true;
	m_ndex = -1;
	m_edit_number.m_type = EDIT_TYPE_NUMBER;
	m_edit_number_x.m_type = EDIT_TYPE_NUMBER_X;
	m_edit_daypay.m_type = EDIT_TYPE_FLOAT;
}

CAddStaffDlg::CAddStaffDlg(CStaffMngDlg* dlg)
{
	m_dlg=dlg;
	m_bAdd=false;
}

CAddStaffDlg::~CAddStaffDlg()
{
}

void CAddStaffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SEX, m_ComBoSex);
	DDX_Control(pDX, IDC_TEL, m_edit_number);
	DDX_Control(pDX, IDC_IDCARD, m_edit_number_x);
	DDX_Control(pDX, IDC_DAYPAY, m_edit_daypay);
	DDX_Control(pDX, IDC_TYPE, m_comboType);
}


BEGIN_MESSAGE_MAP(CAddStaffDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddStaffDlg::OnBnClickedOk)
	ON_MESSAGE(WM_ADDSTAFF_CALL, &CAddStaffDlg::OnCallBack)
END_MESSAGE_MAP()


BOOL CAddStaffDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ComBoSex.AddString(L"男");
	m_ComBoSex.AddString(L"女");
	m_ComBoSex.SetCurSel(0);

	for (int i =0;i<STAFF_TYPE_MAX;i++)
	{
		m_comboType.InsertString(i,StaffType[i]);
	}
	m_comboType.SetCurSel(0);

	if (m_dlg)
	{
		SetModifyInit();
	}

	//设置回调
	g_Globle.SetCallback(AddStaffCallback,this);
	return TRUE;
}

void CAddStaffDlg::SetNotifyWnd(CStaffMngDlg* dlg)
{
	m_dlg=dlg;
	m_bAdd=false;
}

void CAddStaffDlg::SendToJudgeStaff(CString strIdcard)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_JUDGE_STAFF;
	root[CMD_JUDGESTAFF[EM_JUDGE_STAFF_IDCARD]]=T2A(strIdcard);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CAddStaffDlg::SendToAddStaff()
{
	CString strName= L"";
	GetDlgItemText(IDC_NAME,strName);
	CString strSex= L"";
	int ndex=m_ComBoSex.GetCurSel();
	m_ComBoSex.GetLBText(ndex,strSex);
	int age;
	age = GetDlgItemInt(IDC_AGE);
	CString strID= L"";
	GetDlgItemText(IDC_IDCARD,strID);
	CString strTel= L"";
	GetDlgItemText(IDC_TEL,strTel);
	STAFF_TYPE type = (STAFF_TYPE)m_comboType.GetCurSel();

	int sort = GetDlgItemInt(IDC_SORT);

	CString strDayPay= L"";
	GetDlgItemText(IDC_DAYPAY,strDayPay);
	double fDayPay = _ttof(strDayPay);

	//生成唯一ID
	CTime time_now;
	time_now = CTime::GetCurrentTime();
	CString  strStaffID = time_now.Format(L"%Y%m%d%H%M%S");

	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_ADD_STAFF;
	root[CMD_STAFFMSG[EM_STAFF_MSG_NAME]]=T2A(strName);
	root[CMD_STAFFMSG[EM_STAFF_MSG_SEX]]=T2A(strSex);
	root[CMD_STAFFMSG[EM_STAFF_MSG_AGE]]=age;
	root[CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]]=T2A(strStaffID);
	root[CMD_STAFFMSG[EM_STAFF_MSG_IDCARD]]=T2A(strID);
	root[CMD_STAFFMSG[EM_STAFF_MSG_TEL]]=T2A(strTel);
	root[CMD_STAFFMSG[EM_STAFF_MSG_TYPE]]=type;
	root[CMD_STAFFMSG[EM_STAFF_MSG_SORT]]=sort;
	root[CMD_STAFFMSG[EM_STAFF_MSG_DAYPAY]]=fDayPay;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);

}

void CAddStaffDlg::SendToMdfStaff(CString strName,CString strSex,int age,CString strStaffID, CString strIdcard,CString strTel,STAFF_TYPE type,int sort,double fDaypay)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_MDF_STAFF;
	root[CMD_STAFFMSG[EM_STAFF_MSG_NAME]]=T2A(strName);
	root[CMD_STAFFMSG[EM_STAFF_MSG_SEX]]=T2A(strSex);
	root[CMD_STAFFMSG[EM_STAFF_MSG_AGE]]=age;
	root[CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]]=T2A(strStaffID);
	root[CMD_STAFFMSG[EM_STAFF_MSG_IDCARD]]=T2A(strIdcard);
	root[CMD_STAFFMSG[EM_STAFF_MSG_TEL]]=T2A(strTel);
	root[CMD_STAFFMSG[EM_STAFF_MSG_TYPE]]=type;
	root[CMD_STAFFMSG[EM_STAFF_MSG_SORT]]=sort;
	root[CMD_STAFFMSG[EM_STAFF_MSG_DAYPAY]]=fDaypay;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CAddStaffDlg::OnBnClickedOk()
{
	CString strName= L"";
	GetDlgItemText(IDC_NAME,strName);
	CString strSex= L"";
	int ndex=m_ComBoSex.GetCurSel();
	m_ComBoSex.GetLBText(ndex,strSex);
	int age;
	age = GetDlgItemInt(IDC_AGE);
	CString strID= L"";
	GetDlgItemText(IDC_IDCARD,strID);
	CString strTel= L"";
	GetDlgItemText(IDC_TEL,strTel);

	if (strName.IsEmpty())
	{
		MessageBox(L"请输入姓名",L"提示");
		return;
	}
	else if(age==0)
	{
		MessageBox(L"请输入姓名",L"提示");
		return;
	}
	else if(strID.IsEmpty())
	{
		MessageBox(L"请输入身份证号",L"提示");
		return;
	}
	else if(strTel.IsEmpty())
	{
		MessageBox(L"请输入联系方式",L"提示");
		return;
	}

	STAFF_TYPE type = (STAFF_TYPE)m_comboType.GetCurSel();
	if (type == STAFF_TYPE_MAX)
	{
		MessageBox(L"请选择职工类型");
		return;
	}

	int sort = GetDlgItemInt(IDC_SORT);
	CString strDayPay= L"";
	GetDlgItemText(IDC_DAYPAY,strDayPay);
	double fDayPay = _ttof(strDayPay);

	if(m_bAdd)
		SendToJudgeStaff(strID);
	else
	{
		if (m_ndex<0)
		{
			MessageBox(L"参数错误！",L"错误");
			return;
		}
		CString strStaffID = m_dlg->m_vet[m_ndex].strStaffID;
		SendToMdfStaff(strName,strSex,age,strStaffID,strID,strTel,type,sort,fDayPay);
	}
}

void CAddStaffDlg::SetModifyInit()
{
	m_ndex = m_dlg->m_nItem;
	CString strname=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,1);
	SetDlgItemText(IDC_NAME,strname);
	CString strsex=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,2);
	if (strsex==L"男")
	{
		m_ComBoSex.SetCurSel(0);
	}
	else
	{
		m_ComBoSex.SetCurSel(1);
	}
	CString strage=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,3);
	SetDlgItemInt(IDC_AGE,_ttoi(strage));
	CString idcard=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,4);
	strOldIdcard=idcard;
	SetDlgItemText(IDC_IDCARD,idcard);
	CString strtell=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,6);
	SetDlgItemText(IDC_TEL,strtell);
	CString strType = m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem, 7);
	SetDlgItemInt(IDC_SORT,m_dlg->m_vet[m_ndex].sort);
	for (int i =0;i<STAFF_TYPE_MAX;i++)
	{
		if (strType == StaffType[i])
		{
			m_comboType.SetCurSel(i);
		}
	}
	CString strDaypay = m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem, 8);
	SetDlgItemText(IDC_DAYPAY,strDaypay);

	CButton *pBtn = (CButton *)GetDlgItem(IDOK);
	pBtn->SetWindowText(L"保存");
}
