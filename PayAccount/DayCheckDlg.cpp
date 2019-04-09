// WorkCalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "DayCheckDlg.h"
#include "afxdialogex.h"
#include "WorkCalDlg.h"


// CDayCheckDlg 对话框

IMPLEMENT_DYNAMIC(CDayCheckDlg, CDialog)

void DayCheckCallback(void* p,string strData)
{
	USES_CONVERSION;
	CDayCheckDlg* pThis=(CDayCheckDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_DAYCHECK_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CDayCheckDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_DPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取日薪失败！",L"错误");
			else
			{
				GetDPay(root);
				SendToGetBook();
			}
		}
		break;
	case SOCK_CMD_GET_SAMPLE_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取图书信息失败！",L"错误");
			else
			{
				GetBook(root);
				SendToGetProject();
			}
		}
		break;
	case SOCK_CMD_GET_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取项目信息失败！",L"错误");
			else
			{
				GetProject(root);
				SendToGetDayPay();
			}
		}
		break;
	case SOCK_CMD_GET_DAYPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取做工统计失败！",L"错误");
			else
			{
				GetDayPay(root);
			}	
		}
		break;
	case SOCK_CMD_DEL_DAYPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"保存做工失败！",L"错误");
			else
			{
				SendToSaveDayPay();
			}	
		}
		break;
	case SOCK_CMD_SAVE_DAYPAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"保存做工统计失败！",L"错误");
			else
			{
				MessageBox(L"保存成功！",L"");
				OnOK();
			}	
		}
		break;
	case SOCK_CMD_GET_PAY:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取单价失败！",L"错误");
			else
			{
				GetOnePay(root);
			}	
		}
		break;
	}
	return TRUE;
}

CDayCheckDlg::CDayCheckDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDayCheckDlg::IDD, pParent)
{
	m_edit_per.m_type = EDIT_TYPE_FLOAT;
	m_edit_day.m_type = EDIT_TYPE_FLOAT;
	m_EditDel.m_type = EDIT_TYPE_FLOAT;
	pDlg = NULL;
}

CDayCheckDlg::~CDayCheckDlg()
{

}

HBRUSH CDayCheckDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID()==IDC_STA)
	{
		pDC->SetTextColor(RGB(255, 0, 0));//设置字体颜色
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CDayCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_GROUP, m_GroupCtrl);
	DDX_Control(pDX, IDC_EDIT_PER, m_edit_per);
	DDX_Control(pDX, IDC_EDIT_DAY, m_edit_day);
	DDX_Control(pDX, IDC_ALL, m_staticAll);
	DDX_Control(pDX, IDC_STA, m_ts);
	DDX_Control(pDX, IDC_EDIT_DEL, m_EditDel);
}


BEGIN_MESSAGE_MAP(CDayCheckDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDayCheckDlg::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDayCheckDlg::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDayCheckDlg::OnBnClickedBtnSave)
	ON_EN_CHANGE(IDC_EDIT_PER, &CDayCheckDlg::OnEnChangeEditPer)
	ON_EN_CHANGE(IDC_EDIT_DAY, &CDayCheckDlg::OnEnChangeEditDay)
	ON_MESSAGE(WM_DAYCHECK_CALL, &CDayCheckDlg::OnCallBack)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_DEL, &CDayCheckDlg::OnEnChangeEditDel)
END_MESSAGE_MAP()


// CDayCheckDlg 消息处理程序
BOOL CDayCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化字体大小
	m_font.CreatePointFont(180, _T("宋体"));
	m_staticAll.SetFont(&m_font);

	//初始化group控件文字信息
	CString str;
	str.Format(L"%s 核算", m_strDate);
	m_GroupCtrl.SetWindowText(str);

	//初始化列表控件
	m_ListCtrl.SetNotifyWnd(this);
	m_ListCtrl.OnInitDialog();

	//列表控件赋值
	SetListCtrlValue();
	
	STAFF_STU stu = *(STAFF_STU*)pDlg->m_ListCtrl.GetItemData(pDlg->m_nItem);
	m_strStaffID = stu.strStaffID;
	m_strStaffName = stu.strname;

	SetWindowText(m_strStaffName);

	UpdateDlg();

	return TRUE;
}

void CDayCheckDlg::SetNotifyWnd(CWorkCalDlg* pdata,CString strDate)
{
	pDlg = pdata;
	m_strDate = strDate;
}

void CDayCheckDlg::SendToGetDPay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_DPAY;
	root[CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]] = T2A(m_strStaffID);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDayCheckDlg::SendToGetBook()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_SAMPLE_BOOK;
	root[CMD_GETBOOK[GETBOOK_RKTYPE]] = BOOK_RK_NO;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDayCheckDlg::SendToGetProject()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PROJECT;
	root[CMD_GETPRO[EM_GETPRO_BWRITE]]=PRO_STAFF_TYPE_MAX;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
}

void CDayCheckDlg::SendToGetDayPay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_DAYPAY;
	root[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]]=T2A(m_strStaffID);
	root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]]=T2A(m_strDate);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDayCheckDlg::SendToDelDayPay(CString strStaffID,CString strDate)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_DEL_DAYPAY;
	root[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]]=T2A(strStaffID);
	root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]]=T2A(strDate);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDayCheckDlg::SendToGetOnePay(CString strStaffID,int proID,CString strBookID,int nItem)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PAY;
	root[GETPAYMSG[EM_GET_PAY_STAFFID]]=T2A(strStaffID);
	root[GETPAYMSG[EM_GET_PAY_PROID]]=proID;
	root[GETPAYMSG[EM_GET_PAY_BOOKID]]=T2A(strBookID);
	root[GETPAYMSG[EM_GET_PAY_NITEM]]=nItem;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDayCheckDlg::SendToSaveDayPay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SAVE_DAYPAY;
	root[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]]=T2A(m_strStaffID);
	root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]]=T2A(m_strDate);
	int nSize = m_vSaves.size();
	if (nSize > 0)
	{
		for (int i=0;i<nSize;i++)
		{
			Json::Value one;
			if (m_vSaves[i].type == DAYPAY_TYPE_DEL)
			{
				 one[DAYPAYMSG[EM_DAYPAY_MSG_DELMSG]] = T2A(m_vSaves[i].strMsg);
			}
			else if (m_vSaves[i].type == DAYPAY_TYPE_DAY)
			{
				one[DAYPAYMSG[EM_DAYPAY_MSG_PAYDAY]] = T2A(m_vSaves[i].strPayDay);
				one[DAYPAYMSG[EM_DAYPAY_MSG_DAYS]] = T2A(m_vSaves[i].strDays);
			}
			else if (m_vSaves[i].type == DAYPAY_TYPE_JIJIAN)
			{
				one[DAYPAYMSG[EM_DAYPAY_MSG_PROID]] = m_vSaves[i].proID;
				one[DAYPAYMSG[EM_DAYPAY_MSG_BOOKID]] = T2A(m_vSaves[i].strBookID);
				one[DAYPAYMSG[EM_DAYPAY_MSG_PAY]] = T2A(m_vSaves[i].pay);
				one[DAYPAYMSG[EM_DAYPAY_MSG_NUMBER]] = m_vSaves[i].number;
				one[DAYPAYMSG[EM_DAYPAY_MSG_PRONAME]] = T2A(m_vSaves[i].strProName);
				one[DAYPAYMSG[EM_DAYPAY_MSG_BOOKNAME]] = T2A(m_vSaves[i].strBookName);
			}
			one[DAYPAYMSG[EM_DAYPAY_MSG_MONEY]] = T2A(m_vSaves[i].money);
			one[DAYPAYMSG[EM_DAYPAY_MSG_TYPE]] = m_vSaves[i].type;
			root[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one);
		}
		Json::FastWriter writer;  
		string temp = writer.write(root);
		g_Globle.SendTo(temp);
	}
	else
	{
		MessageBox(L"保存成功！",L"提示");
	}
}

void CDayCheckDlg::GetDPay(Json::Value root)
{
	m_fDaypay = root[CMD_STAFFMSG[EM_STAFF_MSG_DAYPAY]].asDouble();
	CString str;
	str.Format(L"%.04f",m_fDaypay);
	SetDlgItemText(IDC_EDIT_PER,str);
}

void CDayCheckDlg::GetBook(Json::Value root)
{
	m_vBooks.clear();
	Json::FastWriter writer;  
	string temp = writer.write(root);

	int nCount = root[CMD_BOOKMSG[BOOKMSG_COUNT]].asInt();
	for (int i=0;i<nCount;i++)
	{
		char str[25]={0};
		BOOK_STU stu;
		Json::Value one = root[itoa(i,str,10)];

		stu.strBookID=one[CMD_BOOKMSG[BOOKMSG_ID]].asCString();
		stu.strname=one[CMD_BOOKMSG[BOOKMSG_NAME]].asCString();
		m_vBooks.push_back(stu);
	}
}

void CDayCheckDlg::GetProject(Json::Value root)
{
	m_vProjects.clear();
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
		//按ndex从小到大排列
		if(m_vProjects.size() == 0)
			m_vProjects.push_back(stu);
		else
		{
			bool bInset=false;
			vector <PROJECT_STU>::iterator it;
			for ( it = m_vProjects.begin( ) ; it != m_vProjects.end( ) ; it++ )
			{
				if (stu.ndex<(*it).ndex)
				{
					bInset = true;
					m_vProjects.insert(it,stu);
					break;
				}
			}
			if(!bInset)
				m_vProjects.push_back(stu);
		}
	}
}

void CDayCheckDlg::GetDayPay(Json::Value root)
{
	m_vCal.clear();
	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json:: Value js = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		if (js.isArray())
		{
			int nCount = js.size();
			for (int i=0;i<nCount;i++)
			{
				DAYPAY stu;
				stu.id = js[i][DAYPAYMSG[EM_DAYPAY_MSG_ID]].asInt();
				stu.type = (DAYPAY_TYPE)js[i][DAYPAYMSG[EM_DAYPAY_MSG_TYPE]].asInt();
				if (stu.type == DAYPAY_TYPE_DEL)
				{
					stu.strMsg = js[i][DAYPAYMSG[EM_DAYPAY_MSG_DELMSG]].asCString();
				}
				else if (stu.type == DAYPAY_TYPE_DAY)
				{
					stu.strPayDay = js[i][DAYPAYMSG[EM_DAYPAY_MSG_PAYDAY]].asCString();
					stu.strDays = js[i][DAYPAYMSG[EM_DAYPAY_MSG_DAYS]].asCString();
				}
				else if(stu.type == DAYPAY_TYPE_JIJIAN)
				{
					stu.proID = js[i][DAYPAYMSG[EM_DAYPAY_MSG_PROID]].asInt();
					stu.strBookID = js[i][DAYPAYMSG[EM_DAYPAY_MSG_BOOKID]].asCString();
					stu.pay = js[i][DAYPAYMSG[EM_DAYPAY_MSG_PAY]].asCString();
					stu.number = js[i][DAYPAYMSG[EM_DAYPAY_MSG_NUMBER]].asDouble();
					stu.strProName = js[i][DAYPAYMSG[EM_DAYPAY_MSG_PRONAME]].asCString();
					stu.strBookName = js[i][DAYPAYMSG[EM_DAYPAY_MSG_BOOKNAME]].asCString();
				}		
				stu.money = js[i][DAYPAYMSG[EM_DAYPAY_MSG_MONEY]].asCString();		
				m_vCal.push_back(stu);
			}
		}
	}
	SetListCtrlValue();
}

void CDayCheckDlg::GetOnePay(Json::Value root)
{
	USES_CONVERSION;
	int nItem = root[GETPAYMSG[EM_GET_PAY_NITEM]].asInt();
	CString strPay = A2T(root[GETPAYMSG[EM_GET_PAY_PAY]].asCString());
	m_ListCtrl.SetItemText(nItem, 3, strPay);
	CString strNum;
	m_ListCtrl.m_ListLine[nItem].pEdit->GetWindowText(strNum);
	int num = _ttoi(strNum);
	double pay = _ttof(strPay);
	double all = num * pay;
	m_ListCtrl.m_ListLine[nItem].money = all;
	CString strAll;
	strAll.Format(L"%.02f", all);
	m_ListCtrl.SetItemText(nItem, 5, strAll);


	double allMoney = 0;
	int nSize = m_ListCtrl.m_ListLine.size();
	for (int i = 0; i < nSize; i++)
	{
		allMoney += m_ListCtrl.m_ListLine[i].money;
	}
	//SetAllPayCtrl(DAYPAY_TYPE_JIJIAN,allMoney);
	SetAllPayCtrl();
}

void CDayCheckDlg::UpdateDlg()
{
	CRect rectThis;
	GetClientRect(&rectThis);
	if (m_ts)
	{
		CRect rc;
		m_ts.GetClientRect(rc);
		int width = rc.Width();

		rc.top = rectThis.bottom-25;
		rc.left = 0;
		rc.right = width;
		rc.bottom = rc.top+25;
		m_ts.MoveWindow(rc, TRUE);
	}
	//设置回调
	g_Globle.SetCallback(DayCheckCallback,this);
	SendToGetDPay();
}

void CDayCheckDlg::SetListCtrlValue()
{
	//4.控件赋值
	SetAllPayCtrl(DAYPAY_TYPE_MAX,0);
	m_ListCtrl.DeleteAllItems();
	SetDlgItemText(IDC_EDIT_DAY,  L"");
	int ndex=0;
	for (int i = 0; i < m_vCal.size(); i++)
	{
		if (m_vCal[i].type == DAYPAY_TYPE_JIJIAN)
		{
			CString str,strVdex;
			str.Format(L"%d", ndex + 1);
			m_ListCtrl.InsertItem(ndex, str);//序号
			strVdex.Format(L"%d", i);
			m_ListCtrl.SetItemText(ndex, 1, strVdex);//项目
			m_ListCtrl.SetItemText(ndex, 2, strVdex);//图书
			m_ListCtrl.SetItemText(ndex, 3, m_vCal[i].pay);//单价
			str.Format(L"%.1f", m_vCal[i].number);
			m_ListCtrl.SetItemText(ndex, 4, str);//件数
			m_ListCtrl.SetItemText(ndex, 5, m_vCal[i].money);//金额
			str.Format(L"%d", m_vCal[i].id);
			m_ListCtrl.SetItemText(ndex, 6, str);//id
			ndex++;
		}
		else if (m_vCal[i].type == DAYPAY_TYPE_DAY)
		{
			SetDlgItemText(IDC_EDIT_PER, m_vCal[i].strPayDay);
			SetDlgItemText(IDC_EDIT_DAY, m_vCal[i].strDays);

			//double money = _ttof(m_vCal[i].money);
			//SetAllPayCtrl(DAYPAY_TYPE_DAY, money);
		}
		else if (m_vCal[i].type == DAYPAY_TYPE_DEL)
		{
			m_EditDel.SetWindowTextW(m_vCal[i].money);
			SetDlgItemText(IDC_EDIT_MSG, m_vCal[i].strMsg);
			//SetAllPayCtrl(DAYPAY_TYPE_DEL, _ttof(m_vCal[i].money));
		}
	}
	SetAllPayCtrl();
}

void CDayCheckDlg::SetPerDayCtrlShow(int nCmdShow)
{
	GetDlgItem(IDC_EDIT_PER)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_EDIT_DAY)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_STC1)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_STC2)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_BTN_SAVE_DAY)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_GROUP1)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_ZJ2)->ShowWindow(nCmdShow);
}

void CDayCheckDlg::SetJiJianCtrlShow(int nCmdShow)
{
	GetDlgItem(IDC_BTN_ADD)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_BTN_DEL)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_LIST1)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_BTN_SAVE)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_ZJ)->ShowWindow(nCmdShow);
	m_GroupCtrl.ShowWindow(nCmdShow);
}

//添加一行
void CDayCheckDlg::OnBnClickedBtnAdd()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCount = m_ListCtrl.GetItemCount();
	CString str;
	str.Format(L"%d", nCount + 1);
	m_ListCtrl.InsertItem(nCount, str);
	m_ListCtrl.SetItemText(nCount, 1,  L"");
	m_ListCtrl.SetItemText(nCount, 2,  L"");
	m_ListCtrl.SetItemText(nCount, 3,  L"");
	m_ListCtrl.SetItemText(nCount, 4,  L"");
	m_ListCtrl.SetItemText(nCount, 5,  L"");
	m_ListCtrl.GetPay(nCount);
}

//删除一行
void CDayCheckDlg::OnBnClickedBtnDel()
{
	// TODO:  在此添加控件通知处理程序代码
	int nCount = m_ListCtrl.GetItemCount();
	if (nCount>0)
	{
		m_ListCtrl.DeleteItem(nCount - 1);

		double all_money = 0;
		int nSize = m_ListCtrl.m_ListLine.size();
		for (int i = 0; i < nSize; i++)
		{
			all_money += m_ListCtrl.m_ListLine[i].money;
		}

		CString strPayDay,strDays,strMoney;
		GetDlgItemText(IDC_EDIT_PER, strPayDay);
		GetDlgItemText(IDC_EDIT_DAY, strDays);
		all_money = _ttof(strPayDay) * _ttof(strDays)+all_money;

		strMoney.Format(L"总计：%.02f", all_money);
		SetDlgItemText(IDC_ALL,strMoney);
	}
}

//保存
void CDayCheckDlg::OnBnClickedBtnSave()
{
	m_vSaves.clear();

	//天
	DAYPAY day_cal;
	day_cal.type = DAYPAY_TYPE_DAY;
	CString strPayDay,strDays,strMoney;
	GetDlgItemText(IDC_EDIT_PER, strPayDay);
	GetDlgItemText(IDC_EDIT_DAY, strDays);
	double money = _ttof(strPayDay) * _ttof(strDays);
	if (money > 0)
	{
		strMoney.Format(L"%.02f", money);
		day_cal.strPayDay = strPayDay;
		day_cal.strDays = strDays;
		day_cal.money = strMoney;
		m_vSaves.push_back(day_cal);
	}
	
	//扣除
	CString strDelMoney,strMsg;
	m_EditDel.GetWindowTextW(strDelMoney);
	if ( _ttof(strDelMoney) != 0)
	{
		GetDlgItemText(IDC_EDIT_MSG,strMsg);
		DAYPAY cal;
		cal.type = DAYPAY_TYPE_DEL;
		cal.money = strDelMoney;
		cal.strMsg = strMsg;
		m_vSaves.push_back(cal);
	}

	//件
	int nCount = m_ListCtrl.m_ListLine.size();
	for (int i = 0; i < nCount;i++)
	{
		DAYPAY cal;
		cal.type = DAYPAY_TYPE_JIJIAN;
		STU_LINE line = m_ListCtrl.m_ListLine[i];
		int nSel = line.pComboProject->GetCurSel();
		if (nSel>=0)
		{
			cal.proID=*((int*)line.pComboProject->GetItemData(nSel));
			line.pComboProject->GetLBText(nSel,cal.strProName);
		}

		nSel = line.pComboBook->GetCurSel();
		if (nSel>=0)
		{
			cal.strBookID=*((CString*)line.pComboBook->GetItemData(nSel));
			line.pComboBook->GetLBText(nSel,cal.strBookName);
		}

		if (cal.proID == 0 || cal.strBookID == "")
		{
			MessageBox(L"请选择项目或图书",  L"提示");
			return;
		}
		cal.pay = m_ListCtrl.GetItemText(line.nItem, 3);
		CString strNumber;
		line.pEdit->GetWindowText(strNumber);
		cal.number = _ttof(strNumber);
		if (cal.number>0)
		{
			cal.money = m_ListCtrl.GetItemText(line.nItem, 5);
			m_vSaves.push_back(cal);
		}
		else
		{
			MessageBox(L"请输入完整信息",  L"提示");
			return;
		}
	}
	//先删除，再保存
	SendToDelDayPay(m_strStaffID,m_strDate);
}

void CDayCheckDlg::SetAllPayCtrl()
{
	CString strMoney;
	double all_money = 0;
	double jj_money = 0;
	int nSize = m_ListCtrl.m_ListLine.size();
	for (int i = 0; i < nSize; i++)
	{
		jj_money += m_ListCtrl.m_ListLine[i].money;
	}
	all_money += jj_money;
	strMoney.Format(L"金额：%.04f", jj_money);
	SetDlgItemText(IDC_ZJ, strMoney);

	CString strPayDay,strDays;
	GetDlgItemText(IDC_EDIT_PER, strPayDay);
	GetDlgItemText(IDC_EDIT_DAY, strDays);
	double f_daymoney = _ttof(strPayDay) * _ttof(strDays);
	all_money += f_daymoney;
	strMoney.Format(L"金额：%.04f", f_daymoney);
	SetDlgItemText(IDC_ZJ2, strMoney);

	CString strDelMoney;
	m_EditDel.GetWindowTextW(strDelMoney);
	all_money -= _ttof(strDelMoney);

	strMoney.Format(L"总计：%.04f", all_money);
	SetDlgItemText(IDC_ALL,strMoney);
}

void CDayCheckDlg::SetAllPayCtrl(DAYPAY_TYPE type, double money)
{
	CString str;
	str.Format(L"金额：%.04f", money);
	if (type == DAYPAY_TYPE_DAY)
	{
		SetDlgItemText(IDC_ZJ2, str);

		double all_money = money;
		int nSize = m_ListCtrl.m_ListLine.size();
		for (int i = 0; i < nSize; i++)
		{
			all_money += m_ListCtrl.m_ListLine[i].money;
		}
		CString strDelMoney;
		m_EditDel.GetWindowTextW(strDelMoney);
		all_money -= _ttof(strDelMoney);

		CString strMoney;
		strMoney.Format(L"总计：%.04f", all_money);
		SetDlgItemText(IDC_ALL,strMoney);
	}
	else if (type == DAYPAY_TYPE_JIJIAN)
	{
		SetDlgItemText(IDC_ZJ, str);

		CString strPayDay,strDays,strMoney;
		GetDlgItemText(IDC_EDIT_PER, strPayDay);
		GetDlgItemText(IDC_EDIT_DAY, strDays);
		double all_money = _ttof(strPayDay) * _ttof(strDays)+money;

		CString strDelMoney;
		m_EditDel.GetWindowTextW(strDelMoney);
		all_money -= _ttof(strDelMoney);

		strMoney.Format(L"总计：%.04f", all_money);
		SetDlgItemText(IDC_ALL,strMoney);
	}
	else if (type == DAYPAY_TYPE_DEL)
	{
		double all_money = 0;
		int nSize = m_ListCtrl.m_ListLine.size();
		for (int i = 0; i < nSize; i++)
		{
			all_money += m_ListCtrl.m_ListLine[i].money;
		}

		CString strPayDay,strDays,strMoney;
		GetDlgItemText(IDC_EDIT_PER, strPayDay);
		GetDlgItemText(IDC_EDIT_DAY, strDays);
		all_money += _ttof(strPayDay) * _ttof(strDays);

		all_money -= money;

		strMoney.Format(L"总计：%.04f", all_money);
		SetDlgItemText(IDC_ALL,strMoney);
	}
	else if (type == DAYPAY_TYPE_MAX)
	{
		SetDlgItemText(IDC_ZJ, str);
		SetDlgItemText(IDC_ZJ2, str);
		str.Format(L"总计：%.04f", money);
		SetDlgItemText(IDC_ALL,str);
	}
}


void CDayCheckDlg::OnEnChangeEditPer()
{
	/*CString strPayDay, strDays;
	GetDlgItemText(IDC_EDIT_PER, strPayDay);
	GetDlgItemText(IDC_EDIT_DAY, strDays);
	double fPayDay = 0, fDays = 0;
	fPayDay = _ttof(strPayDay);
	fDays = _ttof(strDays);
	double money = fPayDay * fDays;
	CString  str;
	str.Format(L"金额：%.02f", money);
	SetDlgItemText(IDC_ZJ2, str);*/
	//SetAllPayCtrl(DAYPAY_TYPE_DAY,money);
	SetAllPayCtrl();
}

void CDayCheckDlg::OnEnChangeEditDay()
{
	/*CString strPayDay, strDays;
	GetDlgItemText(IDC_EDIT_PER, strPayDay);
	GetDlgItemText(IDC_EDIT_DAY, strDays);
	double fPayDay = 0, fDays = 0;
	fPayDay = _ttof(strPayDay);
	fDays = _ttof(strDays);
	double money = fPayDay * fDays;
	CString  str;
	str.Format(L"金额：%.02f", money);
	SetDlgItemText(IDC_ZJ2, str);*/
	//SetAllPayCtrl(DAYPAY_TYPE_DAY,money);
	SetAllPayCtrl();
}


void CDayCheckDlg::OnEnChangeEditDel()
{
	SetAllPayCtrl();
}
