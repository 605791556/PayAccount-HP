// UserMngDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "UserMngDlg.h"
#include "afxdialogex.h"
#include "AddUserDlg.h"


// CUserMngDlg 对话框

IMPLEMENT_DYNAMIC(CUserMngDlg, CDialogEx)

CUserMngDlg::CUserMngDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUserMngDlg::IDD, pParent)
{

}

CUserMngDlg::~CUserMngDlg()
{
}

void CUserMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_USER, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CUserMngDlg, CDialogEx)
	ON_BN_CLICKED(ID_DEL, &CUserMngDlg::OnBnClickedDel)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_USER, &CUserMngDlg::OnLvnColumnclickUser)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CUserMngDlg::OnHdnItemclickUser)
	ON_NOTIFY(NM_CLICK, IDC_USER, &CUserMngDlg::OnNMClickUser)
	ON_BN_CLICKED(ID_ADD, &CUserMngDlg::OnBnClickedAdd)
	ON_MESSAGE(WM_USERMNG_CALL, &CUserMngDlg::OnCallBack)
END_MESSAGE_MAP()


void UserMngCallback(void* p,string strData)
{
	USES_CONVERSION;
	CUserMngDlg* pThis=(CUserMngDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_USERMNG_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CUserMngDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_DEL_USER:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"删除失败！",L"错误");
			else
			{
				SendToFindUser();
				MessageBox(L"删除成功！",L"错误");
			}
		}
		break;
	}
	return TRUE;
}

// CUserMngDlg 消息处理程序
BOOL CUserMngDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR rgtsz[4][10] = { _T("序号"), _T("账号"), _T("类型"), _T("创建时间") };

	LV_COLUMN lvcolumn;
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	int nWidth = rect.Width();
	for (int i = 0; i < 4; i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i == 0)
		{
			lvcolumn.cx = 40;
		}
		else if (i==3)
		{
			lvcolumn.cx = 160;
		}
		else
		{
			lvcolumn.cx = (rect.Width() - 200) / 2;
		}
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(102, 205, 170));
	m_ListCtrl.SetTextBkColor(RGB(102, 205, 170));

	//设置回调
	g_Globle.SetCallback(UserMngCallback,this);
	SendToFindUser();

	/*if (g_Globle.m_nType == TYPE_COMMON)
	{
		((CButton*)GetDlgItem(ID_ADD))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(ID_DEL))->EnableWindow(FALSE);
	}*/
	return TRUE;
}

void CUserMngDlg::SendToFindUser()
{
	m_vet.clear();
	m_ListCtrl.DeleteAllItems();

	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_USER;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CUserMngDlg::SendToDelUser(CString strName)
{
	USES_CONVERSION;

	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_DEL_USER;
	root[CMD_DELUSER[EM_DEL_USER_NAME]]=T2A(strName);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CUserMngDlg::GetUser(Json::Value root)
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
	}
	SetListCtrlValue();
}

void CUserMngDlg::SetListCtrlValue()
{
	CString str;
	for (int i = 0; i < m_vet.size(); i++)
	{
		str.Format(L"%d", i+1);
		m_ListCtrl.InsertItem(i, str);
		m_ListCtrl.SetItemText(i, 1, m_vet[i].strname);
		int tmp = m_vet[i].type;
		m_ListCtrl.SetItemText(i, 2, TypeName[tmp]);
		m_ListCtrl.SetItemText(i, 3, m_vet[i].strTime);
	}
	UpdateData();
}

void CUserMngDlg::OnBnClickedDel()
{
	if (m_nItem>=0)
	{
		CString strName = m_ListCtrl.GetItemText(m_nItem, 1);
		if (strName=="admin")
		{
			m_nItem = -1;
			MessageBox(L"超级管理员作为保留账号，不能将其删除！");
			return;
		}
		CString tmp;
		tmp.Format(L"确认删除 '%s' ？", strName);
		if (MessageBox(tmp, TEXT("警告"), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
		{
			SendToDelUser(strName);
		}
		m_nItem = -1;
	}
	else
	{
		MessageBox(L"请先选择要删除的账号");
	}
}


void CUserMngDlg::OnLvnColumnclickUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


void CUserMngDlg::OnHdnItemclickUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	*pResult = 0;
}


void CUserMngDlg::OnNMClickUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	m_nItem = pNMItemActivate->iItem;
	*pResult = 0;
}


void CUserMngDlg::OnBnClickedAdd()
{
	CAddUserDlg dlg;
	int ret = dlg.DoModal();
	g_Globle.SetCallback(UserMngCallback,this);

	if(ret== IDOK)
	{
		SendToFindUser();
	}
}
