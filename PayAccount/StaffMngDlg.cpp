// StaffMngDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "StaffMngDlg.h"
#include "AddStaffDlg.h"
#include "PayAccountDlg.h"

extern CPayAccountDlg* g_PayAccountDlg;
// CStaffMngDlg 对话框

IMPLEMENT_DYNAMIC(CStaffMngDlg, CDialog)

void StaffMngCallback(void* p,string strData)
{
	USES_CONVERSION;
	CStaffMngDlg* pThis=(CStaffMngDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_STAFFMNG_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CStaffMngDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_STAFF:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取职工信息失败！",L"错误");
			else
			{
				GetStaff(root);
			}
		}
		break;
	case SOCK_CMD_DEL_STAFF:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"删除失败！",L"错误");
			else
			{
				OnBnClickedStaffFind();
				MessageBox(L"删除成功！",L"");
			}
		}
		break;
	}
	return TRUE;
}

CStaffMngDlg::CStaffMngDlg(CWnd* pParent /*=NULL*/)
: CDialog(CStaffMngDlg::IDD, pParent)
{
	m_nPage=1;
	m_dex=1;
}

CStaffMngDlg::~CStaffMngDlg()
{
}

void CStaffMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
	DDX_Control(pDX, IDC_STA_ALL, m_sta_all);
	DDX_Control(pDX, IDC_STA_GO, m_sta_go);
	DDX_Control(pDX, IDC_EDIT_GO, m_edit_go);
	DDX_Control(pDX, IDC_BTN_GO, m_btn_go);
	DDX_Control(pDX, IDC_BTN_LAST, m_BtnLast);
	DDX_Control(pDX, IDC_BTN_NEXT, m_BtnNext);
}


BEGIN_MESSAGE_MAP(CStaffMngDlg, CDialog)
	ON_BN_CLICKED(IDC_STAFF_ADD, &CStaffMngDlg::OnBnClickedStaffAdd)
	ON_BN_CLICKED(IDC_STAFF_FIND, &CStaffMngDlg::OnBnClickedStaffFind)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CStaffMngDlg::OnNMRClickList2)
	ON_MESSAGE(WM_STAFFMNG_CALL, &CStaffMngDlg::OnCallBack)
	ON_COMMAND(ID_MU_DEL, DelStaff)//ID_MENUITEM1消息映射
	ON_COMMAND(ID_MU_MDF, MdfStaff)
	ON_BN_CLICKED(IDC_BTN_GO, &CStaffMngDlg::OnBnClickedBtnGo)
	ON_BN_CLICKED(IDC_BTN_LAST, &CStaffMngDlg::OnBnClickedBtnLast)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CStaffMngDlg::OnBnClickedBtnNext)
END_MESSAGE_MAP()


// CStaffMngDlg 消息处理程序
BOOL CStaffMngDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR rgtsz[9][10] = { _T("序号"), _T("姓名"), _T("性别"), _T("年龄"), _T("身份证"), _T("入职时间"), _T("联系方式"), _T("类型"), _T("日薪") };

	LV_COLUMN lvcolumn;
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	for (int i=0;i<9;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i>=0 && i<=3)
		{
			if (i==1)
			{
				lvcolumn.cx = 100;
			}
			else
			{
				lvcolumn.cx = 60;
			}
		}
		else
		{
			lvcolumn.cx = (rect.Width()-60*3-100)/5;
		}
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(102 ,205 ,170));
	m_ListCtrl.SetTextBkColor(RGB(102 ,205, 170)); 

	//设置回调
	g_Globle.SetCallback(StaffMngCallback,this);
	SendToGetStaff(L"",(m_dex-1)*20,20);
	return TRUE;
}

void CStaffMngDlg::GetStaff(Json::Value root)
{
	m_nCount = 0;
	m_vet.clear();
	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json:: Value js = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		if (js.isArray())
		{
			int nCount = js.size();
			for (int i=0;i<nCount;i++)
			{
				STAFF_STU stu;
				stu.strname = js[i][CMD_STAFFMSG[EM_STAFF_MSG_NAME]].asCString();
				stu.strSex = js[i][CMD_STAFFMSG[EM_STAFF_MSG_SEX]].asCString();
				stu.age = js[i][CMD_STAFFMSG[EM_STAFF_MSG_AGE]].asInt();
				stu.strStaffID = js[i][CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]].asCString();
				stu.strIdCard = js[i][CMD_STAFFMSG[EM_STAFF_MSG_IDCARD]].asCString();
				stu.strInTime = js[i][CMD_STAFFMSG[EM_STAFF_MSG_INTIME]].asCString();
				stu.strTel = js[i][CMD_STAFFMSG[EM_STAFF_MSG_TEL]].asCString();
				stu.type = (STAFF_TYPE)js[i][CMD_STAFFMSG[EM_STAFF_MSG_TYPE]].asInt();
				stu.sort = js[i][CMD_STAFFMSG[EM_STAFF_MSG_SORT]].asInt();
				stu.fDaypay = js[i][CMD_STAFFMSG[EM_STAFF_MSG_DAYPAY]].asDouble();
				m_vet.push_back(stu);
			}
		}
	}
	m_nCount = root[CMD_STAFFMSG[EM_STAFF_MSG_COUNT]].asInt();
	SetListCtrlValue();
}

void CStaffMngDlg::SetListCtrlValue()
{
	m_ListCtrl.DeleteAllItems();

	int n=1;
	CString str;
	for (int i=0;i<m_vet.size();i++)
	{
		str.Format(L"%d",n+(m_dex-1)*20);
		m_ListCtrl.InsertItem(i,str);
		m_ListCtrl.SetItemText(i,1,m_vet[i].strname);
		m_ListCtrl.SetItemText(i,2,m_vet[i].strSex);
		CString tmp;
		tmp.Format(L"%d",m_vet[i].age);
		m_ListCtrl.SetItemText(i,3,tmp);
		m_ListCtrl.SetItemText(i,4,m_vet[i].strIdCard);
		m_ListCtrl.SetItemText(i,5,m_vet[i].strInTime);
		m_ListCtrl.SetItemText(i,6,m_vet[i].strTel);
		if (m_vet[i].type != STAFF_TYPE_MAX)
		    m_ListCtrl.SetItemText(i, 7, StaffType[m_vet[i].type]);
		tmp.Format(L"%.02f",m_vet[i].fDaypay);
		m_ListCtrl.SetItemText(i,8,tmp);
		n++;
	}
	UpdateData();

	str.Format(L"共%d条记录",m_nCount);
	m_sta_all.SetWindowTextW(str);
	int ns = m_nCount%20;
	m_nPage = m_nCount/20;
	if(ns>0)
		m_nPage+=1;
	if (m_nPage==0)
		m_nPage=1;
	str.Format(L"共%d页  第%d页",m_nPage,m_dex);
	m_sta_go.SetWindowTextW(str);

	if (m_nPage == 1)
	{
		m_BtnLast.EnableWindow(FALSE);
		m_BtnNext.EnableWindow(FALSE);
	}
	else if (m_nPage>1)
	{
		if (m_dex==1)
			m_BtnLast.EnableWindow(FALSE);
		else if (m_dex>1)
			m_BtnLast.EnableWindow(TRUE);
		if (m_dex<m_nPage)
			m_BtnNext.EnableWindow(TRUE);
		else
			m_BtnNext.EnableWindow(FALSE);
	}
}

void CStaffMngDlg::SendToGetStaff(CString strKeyWord,int nStart,int nNum)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_STAFF;
	root[CMD_GETSTAFF[EM_GET_STAFF_KEYWORD]]=T2A(strKeyWord);
	root[CMD_GETSTAFF[EM_GET_STAFF_NSTART]]=nStart;
	root[CMD_GETSTAFF[EM_GET_STAFF_NUMBER]]=nNum;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffMngDlg::SendToDelStaff(CString strStaffID)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_DEL_STAFF;
	root[CMD_DELSTAFF[EM_DEL_STAFF_ID]]=T2A(strStaffID);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffMngDlg::OnBnClickedStaffAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	CAddStaffDlg dlg;
	int ret = dlg.DoModal();
	g_Globle.SetCallback(StaffMngCallback,this);
	if(ret==IDOK)
	{
		//添加了新职工,通知相关页面更新数据
		//g_PayAccountDlg->PostMessageW(WM_VDATAUPDATE);
		OnBnClickedStaffFind();
	}
}

void CStaffMngDlg::OnBnClickedStaffFind()
{
	m_dex = 1;
	GetDlgItemText(IDC_NAME,m_strKeyWord);
	SendToGetStaff(m_strKeyWord,(m_dex-1)*20,20);
}

void CStaffMngDlg::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_nItem = pNMItemActivate->iItem;
	if (m_nItem>=0)
	{
		CMenu m_Menu;
		m_Menu.LoadMenu(IDR_MENU2);
		CMenu *m_SubMenu = m_Menu.GetSubMenu(0);
		CPoint pt;
		pt=pNMItemActivate->ptAction;
		ClientToScreen(&pt);
		m_SubMenu->TrackPopupMenu(TPM_LEFTBUTTON |TPM_LEFTALIGN ,pt.x,pt.y,this);
		m_SubMenu->DestroyMenu();	
	}
	*pResult = 0;
}

void CStaffMngDlg::DelStaff()
{
	CString idcard=m_ListCtrl.GetItemText(m_nItem,4);
	CString strStaffID = m_vet[m_nItem].strStaffID;
	CString tmp;
	tmp.Format(L"删除职工将会删除该职工相关的所有工作记录，确认删除身份证号为：%s 的职工？",idcard);
	if(MessageBox(tmp,TEXT("警告"),MB_OKCANCEL|MB_ICONEXCLAMATION )==IDOK)
	{
		//删除该职工的所有相关信息
		SendToDelStaff(strStaffID);
	}
}

void CStaffMngDlg::MdfStaff()
{
	CAddStaffDlg dlg;
	dlg.SetNotifyWnd(this);
	int ret = dlg.DoModal();
	g_Globle.SetCallback(StaffMngCallback,this);
	if(ret==IDOK)
	{
		OnBnClickedStaffFind();
	}
}

void CStaffMngDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rectThis;
	GetClientRect(&rectThis);
	if (m_ListCtrl)
	{
		CRect rc;
		m_ListCtrl.GetClientRect(rc);
		rc.top = rc.top + 80;
		rc.left = rectThis.left+10;
		rc.right = rectThis.right-10;
		rc.bottom = rectThis.bottom-50;
		m_ListCtrl.MoveWindow(rc, TRUE);
	}
	if (m_sta_all)
	{
		CRect rc;
		m_sta_all.GetClientRect(rc);
		int nw = rc.Width();
		rc.top = rectThis.bottom-35;
		rc.left = rectThis.left+10;
		rc.right = rc.left+nw;
		rc.bottom = rectThis.bottom;
		m_sta_all.MoveWindow(rc, TRUE);
	}

	if (m_BtnNext)
	{
		CRect rc;
		m_BtnNext.GetClientRect(rc);
		rc.right = rectThis.right-10;
		rc.top = rectThis.bottom-40;
		rc.left = rc.right-50;
		rc.bottom = rc.top+25;
		m_BtnNext.MoveWindow(rc, TRUE);
	}
	if (m_BtnLast)
	{
		CRect rc;
		m_BtnLast.GetClientRect(rc);
		rc.right = rectThis.right-10-50-3;
		rc.top = rectThis.bottom-40;
		rc.left = rc.right-50;
		rc.bottom = rc.top + 25;
		m_BtnLast.MoveWindow(rc, TRUE);
	}
	if (m_btn_go)
	{
		CRect rc;
		m_btn_go.GetClientRect(rc);
		rc.right = rectThis.right-10-50*2-3-20;
		rc.top = rectThis.bottom-40;
		rc.left = rc.right-25;
		rc.bottom = rc.top + 25;
		m_btn_go.MoveWindow(rc, TRUE);
	}
	if (m_edit_go)
	{
		CRect rc;
		m_edit_go.GetClientRect(rc);
		rc.right = rectThis.right-10-50*2-3-20-25-10;
		rc.top = rectThis.bottom-40;
		rc.left = rc.right-25;
		rc.bottom = rc.top + 25;
		m_edit_go.MoveWindow(rc, TRUE);
	}
	if (m_sta_go)
	{
		CRect rc;
		m_sta_go.GetClientRect(rc);
		int nw = rc.Width();
		rc.right = rectThis.right-10-50*2-3-20-25-10-25-15;
		rc.top = rectThis.bottom-35;
		rc.left = rc.right-nw;
		rc.bottom = rc.top + 25;
		m_sta_go.MoveWindow(rc, TRUE);
	}
}

void CStaffMngDlg::OnBnClickedBtnGo()
{
	CString strDex;
	GetDlgItemText(IDC_EDIT_GO,strDex);
	int ndex = _ttoi(strDex);
	if (ndex==0 || ndex> m_nPage)
	{
		return;
	}
	else
	{
		m_dex=ndex;
		SendToGetStaff(m_strKeyWord,(m_dex-1)*20,20);
	}
}


void CStaffMngDlg::OnBnClickedBtnLast()
{
	if (m_dex>1)
	{
		m_dex--;
		SendToGetStaff(m_strKeyWord,(m_dex-1)*20,20);
	}
}


void CStaffMngDlg::OnBnClickedBtnNext()
{
	if (m_dex<m_nPage)
	{
		m_dex++;
		SendToGetStaff(m_strKeyWord,(m_dex-1)*20,20);
	}
}
