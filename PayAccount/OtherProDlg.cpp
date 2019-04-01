// OtherProDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "OtherProDlg.h"
#include "afxdialogex.h"


// COtherProDlg 对话框

void OtherPayCallback(void* p,string strData)
{
	USES_CONVERSION;
	COtherProDlg* pThis=(COtherProDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_OTHERPAY_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT COtherProDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取项目信息失败！",L"错误");
			else
			{
				GetProject(root);
				SendToGetBook();
			}
		}
		break;
	case SOCK_CMD_GET_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取图书信息失败！",L"错误");
			else
			{
				GetBook(root);
				int ndex = m_ComboProject.GetCurSel();
				if (ndex>=0)
				{
					PROJECT_STU stu= *((PROJECT_STU*)m_ComboProject.GetItemData(ndex));
					if (stu.nAllBook == 0)
						SendToGetOtherPay(stu.nID);
					else
						SetListCtrlValue();
				}
			}
		}
		break;
	case SOCK_CMD_GET_OTHERPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取数据失败！",L"错误");
			else
			{
				GetOtherPay(root);
				SetListCtrlValue();
			}
		}
		break;
	case SOCK_CMD_SET_OTHERPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"保存数据失败！",L"错误");
			else
			{
				int ndex = m_ComboProject.GetCurSel();
				if (ndex>=0)
				{
					CString strPay;
					m_payEdit.GetWindowTextW(strPay);
					PROJECT_STU* stu= ((PROJECT_STU*)m_ComboProject.GetItemData(ndex));
					stu->nAllBook = 0;
					stu->strPay = L"";
				}
				MessageBox(L"保存成功！",L"");
			}
		}
		break;
	case SOCK_CMD_SET_OTHERALLBOOKPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"保存数据失败！",L"错误");
			else
			{
				int ndex = m_ComboProject.GetCurSel();
				if (ndex>=0)
				{
					CString strPay;
					m_payEdit.GetWindowTextW(strPay);
					PROJECT_STU* stu= ((PROJECT_STU*)m_ComboProject.GetItemData(ndex));
					stu->nAllBook = 1;
					stu->strPay = strPay;
				}
				MessageBox(L"保存成功！",L"");
			}
		}
		break;
	}
	return TRUE;
}

IMPLEMENT_DYNAMIC(COtherProDlg, CSetPayDlg)

COtherProDlg::COtherProDlg(CWnd* pParent /*=NULL*/)
: CSetPayDlg(pParent)
{

}

COtherProDlg::~COtherProDlg()
{
}

void COtherProDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO_PRO, m_ComboProject);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_EDIT2, m_payEdit);
}


BEGIN_MESSAGE_MAP(COtherProDlg, CSetPayDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PRO, &COtherProDlg::OnCbnSelchangeComboPro)
	ON_BN_CLICKED(IDC_BUTTON1, &COtherProDlg::OnBnClickedButton1)
	ON_MESSAGE(WM_OTHERPAY_CALL, &COtherProDlg::OnCallBack)
	ON_EN_KILLFOCUS(IDC_EDIT1, &COtherProDlg::OnEnKillfocusEdit1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &COtherProDlg::OnNMClickList1)
	ON_BN_CLICKED(IDC_CHECK, &COtherProDlg::OnBnClickedCheck)
END_MESSAGE_MAP()


// COtherProDlg 消息处理程序
BOOL COtherProDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_edit.m_type = EDIT_TYPE_FLOAT;
	m_edit.m_maxLen = 10;

	m_payEdit.m_type = EDIT_TYPE_FLOAT;
	m_payEdit.m_maxLen = 10;

	m_ListCtrl.SetNotifyWnd(this);
	m_ListCtrl.InitListCtrl();
	m_edit.SetParent(&m_ListCtrl);
	return TRUE;
}

void COtherProDlg::SendToGetProject()
{
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PROJECT;
	root[CMD_GETPRO[EM_GETPRO_BWRITE]] = PRO_STAFF_TYPE_MAX;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void COtherProDlg::SendToGetBook()
{
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_BOOK;
	root[CMD_GETBOOK[GETBOOK_KEYWORD]] = ""; 
	root[CMD_GETBOOK[GETBOOK_RKTYPE]] = BOOK_RK_NO;
	root[CMD_GETBOOK[GETBOOK_NSTART]] = 0;
	root[CMD_GETBOOK[GETBOOK_NUMBER]] = 1000;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void COtherProDlg::SendToGetOtherPay(int nID)
{
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_OTHERPAY;
	root[CMD_OTHERPAY[EM_OTHER_PAY_PROID]] = nID;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void COtherProDlg::SendToSetOtherPay(int proID, vector<OTHER_PRO_PAY> vec)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SET_OTHERPAY;
	for (int i=0;i<vec.size();i++)
	{
		Json::Value one;
		one[CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_BOOKID]] = T2A(vec[i].strBookID);
		one[CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_PAY]] = T2A(vec[i].strPay);
		root[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one);
	}
	root[CMD_OTHERPAY[EM_OTHER_PAY_PROID]] = proID;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void COtherProDlg::SendToSetOtherAllBookPay(int proID,CString strPay)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SET_OTHERALLBOOKPAY;
	root[CMD_PROMSG[EM_PROMSG_ID]] = proID;
	root[CMD_PROMSG[EM_PROMSG_PAY]] = T2A(strPay);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void COtherProDlg::GetProject(Json::Value root)
{
	m_vProList.clear();
	m_ComboProject.ResetContent();
	Json::FastWriter writer;  
	string temp = writer.write(root);

	int nCount = root[CMD_PROMSG[EM_PROMSG_COUNT]].asInt();
	for (int i=0;i<nCount;i++)
	{
		char str[25]={0};
		PROJECT_STU stu;
		Json::Value one = root[itoa(i,str,10)];

		stu.nID=one[CMD_PROMSG[EM_PROMSG_ID]].asInt();
		stu.strName=one[CMD_PROMSG[EM_PROMSG_NAME]].asCString();
		stu.pn_type=(PRO_NUM_TYPE)one[CMD_PROMSG[EM_PROMSG_NUM_TYPE]].asInt();
		stu.ps_type=(PRO_STAFF_TYPE)one[CMD_PROMSG[EM_PROMSG_BWRITE]].asInt();
		stu.ndex=one[CMD_PROMSG[EM_PROMSG_NDEX]].asInt();
		stu.nAllBook=one[CMD_PROMSG[EM_PROMSG_ALL_BOOK]].asInt();
		stu.strPay=one[CMD_PROMSG[EM_PROMSG_PAY]].asCString();
		//按ndex从小到大排列
		if(m_vProList.size() == 0)
			m_vProList.push_back(stu);
		else
		{
			bool bInset=false;
			vector <PROJECT_STU>::iterator it;
			for ( it = m_vProList.begin( ) ; it != m_vProList.end( ) ; it++ )
			{
				if (stu.ndex<(*it).ndex)
				{
					bInset = true;
					m_vProList.insert(it,stu);
					break;
				}
			}
			if(!bInset)
				m_vProList.push_back(stu);
		}
	}
	int ndex = 0;
	nCount = m_vProList.size();
	for (int i = 0; i < nCount; i++)
	{
		if (m_vProList[i].nID >= PROJECT_TYPE_OTHER)
		{
			m_ComboProject.InsertString(ndex, m_vProList[i].strName);
			m_ComboProject.SetItemData(ndex,(DWORD_PTR)&m_vProList[i]);
			ndex++;
		}
	}
	if (nCount>0)
	{
		m_ComboProject.SetCurSel(0);
		PROJECT_STU stu= *((PROJECT_STU*)m_ComboProject.GetItemData(0));
		if (stu.nAllBook == 1)
		{
			((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(1);
			m_payEdit.SetWindowTextW(stu.strPay);
			m_payEdit.EnableWindow(TRUE);
			m_ListCtrl.EnableWindow(FALSE);
		}
		else
		{
			m_payEdit.EnableWindow(FALSE);
			m_ListCtrl.EnableWindow(TRUE);
		}
	}
}

void COtherProDlg::GetBook(Json::Value root)
{
	m_vBookList.clear();
	Json::FastWriter writer;  
	string temp = writer.write(root);

	int nCount = root[CMD_BOOKMSG[BOOKMSG_COUNT]].asInt();
	for (int i=0;i<nCount;i++)
	{
		char str[25]={0};
		BOOK_STU stu;
		Json::Value one = root[itoa(i,str,10)];

		stu.nBc1=one[CMD_BOOKMSG[BOOKMSG_BC1]].asInt();
		stu.strBookID=one[CMD_BOOKMSG[BOOKMSG_ID]].asCString();
		stu.strname=one[CMD_BOOKMSG[BOOKMSG_NAME]].asCString();
		stu.strCbs=one[CMD_BOOKMSG[BOOKMSG_CBS]].asCString();
		stu.strDate=one[CMD_BOOKMSG[BOOKMSG_DATE]].asCString();

		stu.nBc2=one[CMD_BOOKMSG[BOOKMSG_BC2]].asInt();
		stu.nSize1=one[CMD_BOOKMSG[BOOKMSG_SIZE1]].asInt();
		stu.nSize2=one[CMD_BOOKMSG[BOOKMSG_SIZE2]].asInt();
		stu.nKb=one[CMD_BOOKMSG[BOOKMSG_KB]].asInt();
		stu.fYz=one[CMD_BOOKMSG[BOOKMSG_YZ]].asDouble();
		stu.nYs=one[CMD_BOOKMSG[BOOKMSG_YS]].asInt();
		stu.nBc=one[CMD_BOOKMSG[BOOKMSG_BC]].asInt();
		stu.fLs=one[CMD_BOOKMSG[BOOKMSG_LS]].asDouble();
		stu.type=(BOOK_TYPE)one[CMD_BOOKMSG[BOOKMSG_B_TYPE]].asInt();
		stu.zyType=(ZHEYEPAY_TYPE)one[CMD_BOOKMSG[BOOKMSG_ZY_TYPE]].asInt();
		stu.rkType=(BOOK_RK)one[CMD_BOOKMSG[BOOKMSG_RK_TYPE]].asInt();
		stu.strMsg=one[CMD_BOOKMSG[BOOKMSG_MSG]].asCString();
		m_vBookList.push_back(stu);
	}
}

void COtherProDlg::GetOtherPay(Json::Value root)
{
	m_vOtherPay.clear();
	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json::Value value = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		if (value.isArray())
		{
			for (int i=0;i<value.size();i++)
			{
				OTHER_PRO_PAY pay;
				pay.strBookID=value[i][CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_BOOKID]].asCString();
				pay.strPay=value[i][CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_PAY]].asCString();
				m_vOtherPay.push_back(pay);
			}
		}
	}
}

void COtherProDlg::SetListCtrlValue()
{
	m_ListCtrl.DeleteAllItems();
	CString str;

	int nVecSize = m_vBookList.size();
	for (int i=0;i<nVecSize;i++)
	{
		m_vBookList[i].strPay = L"";
	}
	int nOtherSize = m_vOtherPay.size();

	for (int i = 0; i < nOtherSize; i++)
	{
		for (int j = 0; j < nVecSize; j++)
		{
			if (m_vOtherPay[i].strBookID == m_vBookList[j].strBookID)
			{
				m_vBookList[j].strPay = m_vOtherPay[i].strPay;
				break;
			}
		}
	}
	for (int i = 0; i < nVecSize; i++)
	{
		BOOK_STU stu = m_vBookList[i];
		str.Format(L"%d", i + 1);
		m_ListCtrl.InsertItem(i, str);
		m_ListCtrl.SetItemText(i, 1, stu.strname);
		m_ListCtrl.SetItemText(i, 2, stu.strPay);
		m_ListCtrl.SetItemData(i,(DWORD_PTR)&m_vBookList[i]);
	}
}

void COtherProDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(OtherPayCallback,this);
	SendToGetProject();
}

void COtherProDlg::OnCbnSelchangeComboPro()
{
	int ndex = m_ComboProject.GetCurSel();
	if (ndex>=0)
	{
		PROJECT_STU stu= *((PROJECT_STU*)m_ComboProject.GetItemData(ndex));
		if (stu.nAllBook == 1)
		{
			((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(1);
			m_payEdit.SetWindowTextW(stu.strPay);
			m_payEdit.EnableWindow(TRUE);
			m_ListCtrl.EnableWindow(FALSE);
		}
		else
		{
			((CButton*)GetDlgItem(IDC_CHECK))->SetCheck(0);
			m_payEdit.SetWindowText(L"");
			SendToGetOtherPay(stu.nID);
			m_payEdit.EnableWindow(FALSE);
			m_ListCtrl.EnableWindow(TRUE);
		}
	}
}

//save
void COtherProDlg::OnBnClickedButton1()
{
	int proID=0;
	int ndex = m_ComboProject.GetCurSel();
	if (ndex>=0)
	{
		proID= *((int*)m_ComboProject.GetItemData(ndex));
	}
	else
	{
		MessageBox(L"请先添加项目信息",L"提示");
		return;
	}
	int nCheck = ((CButton*)GetDlgItem(IDC_CHECK))->GetCheck();
	if (nCheck >0)
	{
		CString strPay;
		m_payEdit.GetWindowTextW(strPay);
		if (strPay.IsEmpty())
		{
			MessageBox(L"请先输入单价",L"提示");
			return;
		}
		else
		{
			SendToSetOtherAllBookPay(proID,strPay);
		}
	}
	else
	{
		vector<OTHER_PRO_PAY> vec;
		int nSize = m_ListCtrl.GetItemCount();
		for (int i = 0; i < nSize;i++)
		{
			BOOK_STU stu = *(BOOK_STU*)m_ListCtrl.GetItemData(i);
			OTHER_PRO_PAY one;
			one.strBookID = stu.strBookID;
			one.strPay =  m_ListCtrl.GetItemText(i,2);
			if (!one.strPay.IsEmpty())
			{
				vec.push_back(one);
			}
		}
		if (vec.size()==0)
		{
			MessageBox(L"请先添加信息",L"提示");
			return;
		}
		SendToSetOtherPay(proID,vec);
	}
}


void COtherProDlg::OnEnKillfocusEdit1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetSubItemValue();
}

void COtherProDlg::SetSubItemValue()
{
	int nSize = m_ListCtrl.GetItemCount();
	if (nSize>0 && m_row<nSize)
	{
		POINT pt;
		GetCursorPos(&pt);
		m_ListCtrl.ScreenToClient(&pt);
		CRect rect;
		m_ListCtrl.GetSubItemRect(m_row,m_col,LVIR_BOUNDS,rect);
		if(!rect.PtInRect(pt))//如果单击在一个节点文本区域内
		{
			CString text;
			m_edit.GetWindowText(text);
			m_ListCtrl.SetItemText(m_row,m_col,text);
			m_edit.ShowWindow(SW_HIDE);//将组合框隐藏
		}
	}
}

void COtherProDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	POINT PT;
	GetCursorPos(&PT);
	m_ListCtrl.ScreenToClient(&PT);
	LVHITTESTINFO hitInfo;
	hitInfo.pt=PT;

	m_ListCtrl.SubItemHitTest(&hitInfo);
	if(hitInfo.flags & LVHT_ONITEMLABEL)//判断是否单击在文本上
	{
		CRect rect;
		m_ListCtrl.GetSubItemRect(hitInfo.iItem,hitInfo.iSubItem,LVIR_BOUNDS,rect);
		if (hitInfo.iSubItem==2)
		{
			CString mes=m_ListCtrl.GetItemText(hitInfo.iItem,hitInfo.iSubItem);
			// rect.InflateRect(0,0,0,0);//增大组合框的高度使其可以容纳整行文本
			m_col=hitInfo.iSubItem;
			m_row=hitInfo.iItem;

			rect.top = rect.top+1;
			rect.left = rect.left+1;
			rect.bottom = rect.bottom-1;
			rect.right = rect.right-1;
			m_edit.MoveWindow(&rect,TRUE);
			m_edit.ShowWindow(SW_NORMAL);
			m_edit.SetWindowText(mes);
			m_edit.BringWindowToTop();
			m_edit.SetFocus();//使组合框聚焦
			m_edit.SetSel(0,-1);
		}
		*pResult = 0;
	}
}


void COtherProDlg::OnBnClickedCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCheck = ((CButton*)GetDlgItem(IDC_CHECK))->GetCheck();
	if (nCheck > 0)
	{
		m_payEdit.EnableWindow(TRUE);
		m_ListCtrl.EnableWindow(FALSE);
	}
	else
	{
		m_payEdit.EnableWindow(FALSE);
		m_ListCtrl.EnableWindow(TRUE);
	}
}
