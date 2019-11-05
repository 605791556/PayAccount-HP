// WorkCalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "WorkCalDlg.h"
#include "afxdialogex.h"
#include "DayCheckDlg.h"

// CWorkCalDlg 对话框

IMPLEMENT_DYNAMIC(CWorkCalDlg, CDialog)

void WorkCalCallback(void* p,string strData)
{
	USES_CONVERSION;
	CWorkCalDlg* pThis=(CWorkCalDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_WORKCAL_MSG,NULL,(LPARAM)pStrData);
	}
}

LRESULT CWorkCalDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_SAMPLE_STAFF:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取职工信息失败！",L"错误");
			else
			{
				GetStaff(root);
				SendToGetOnePayList();
			}
		}
		break;
	case SOCK_CMD_GET_DAYPAY_LIST:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取做工统计列表失败！",L"错误");
			else
			{
				GetOnePayList(root);
				SetListValue();
			}
		}
		break;
	}
	return TRUE;
}

CWorkCalDlg::CWorkCalDlg(CWnd* pParent /*=NULL*/)
	: CPayAccountDlg(pParent)
{

}

CWorkCalDlg::~CWorkCalDlg()
{
}

void CWorkCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_DateCtrl);
	DDX_Control(pDX, IDC_STA_MSG, m_total);
	DDX_Control(pDX, IDC_STA2, m_sta2);
}


BEGIN_MESSAGE_MAP(CWorkCalDlg, CPayAccountDlg)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CWorkCalDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_MESSAGE(WM_WORKCAL_MSG, &CWorkCalDlg::OnCallBack)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CWorkCalDlg::OnNMRClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CWorkCalDlg::OnNMDblclkList1)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CWorkCalDlg 消息处理程序


void CWorkCalDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//设置回调
	g_Globle.SetCallback(WorkCalCallback,this);

	SendToGetOnePayList();
	*pResult = 0;
}

void CWorkCalDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(WorkCalCallback,this);
	//初始化控件位置
	CRect rectThis;
	GetClientRect(&rectThis);
	CRect rectList;
	m_ListCtrl.GetClientRect(&rectList);
	rectList.top = rectThis.top + 50;
	rectList.left = rectThis.left+10;
	rectList.right = rectThis.right-10;
	rectList.bottom = rectThis.bottom -40;
	m_ListCtrl.MoveWindow(rectList, TRUE);

	CRect rectmsg,rect2;
	m_ListCtrl.GetClientRect(&rectmsg);
	int width = rectmsg.Width();
	int height = rectmsg.Height();
	rectmsg.left = rectList.right - 250;
	rectmsg.top = rectList.bottom+1;
	rectmsg.right = rectList.right;
	rectmsg.bottom = rectmsg.top+height;
	m_total.MoveWindow(rectmsg, TRUE);

	rect2.left = 5;
	rect2.top = rectList.bottom+10;
	rect2.right = rect2.left+250;
	rect2.bottom = rectmsg.top+height;
	m_sta2.MoveWindow(rect2, TRUE);

	SendToGetStaff();
}

BOOL CWorkCalDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
		return TRUE;
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CWorkCalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//初始化字体大小
	m_font.CreatePointFont(180, _T("宋体"));
	m_total.SetFont(&m_font);

	InitListCtrl();
	SetListFontSize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CWorkCalDlg::InitListCtrl()
{
	TCHAR rgtsz[6][10] = { _T("序号"), _T("姓名"), _T("按天工资"), _T("计件工资"), _T("扣除"), _T("共计金额") };

	LV_COLUMN lvcolumn;
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	for (int i=0;i<6;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i==0)
		{
			lvcolumn.cx = 60;
		}
		else
		{
			lvcolumn.cx = 100;
		}
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(205, 205, 180));
	m_ListCtrl.SetTextBkColor(RGB(205, 205, 180)); 
}

void CWorkCalDlg::SetListFontSize()
{
	LOGFONT logfont;
	CFont *pfont1 = m_ListCtrl.GetFont();  
	pfont1->GetLogFont(&logfont);  
	logfont.lfHeight = logfont.lfHeight * 1.2; 
	logfont.lfWidth = logfont.lfWidth * 1.2;
	static CFont font1;  
	font1.CreateFontIndirect(&logfont);  
	m_ListCtrl.SetFont(&font1);  
	font1.Detach();
}

HBRUSH CWorkCalDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CPayAccountDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STA2)
	{
		pDC->SetTextColor(RGB(255, 0, 0));//设置字体颜色
	}
	return hbr;
}

void CWorkCalDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

//双击
void CWorkCalDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_nItem = pNMItemActivate->iItem;
	if (m_nItem>=0)
	{
		CTime t;
		m_DateCtrl.GetTime(t);
		CString strDate = t.Format(L"%Y/%m/%d");

		CDayCheckDlg dlg;
		dlg.SetNotifyWnd(this,strDate);
		if(dlg.DoModal() == IDOK)
		{
			UpdateDlg();
		}
		//设置回调
		g_Globle.SetCallback(WorkCalCallback,this);
	}
	*pResult = 0;
}

void CWorkCalDlg::SendToGetStaff()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_SAMPLE_STAFF;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CWorkCalDlg::GetStaff(Json::Value root)
{
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
				stu.strStaffID = js[i][CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]].asCString();
				stu.fDaypay = js[i][CMD_STAFFMSG[EM_STAFF_MSG_DAYPAY]].asDouble();
				m_vet.push_back(stu);
			}
		}
	}
}

void CWorkCalDlg::SendToGetOnePayList()
{
	CTime t;
	m_DateCtrl.GetTime(t);
	CString strDate = t.Format(L"%Y/%m/%d");

	USES_CONVERSION;

	int nSize = m_vet.size();
	if (nSize>0)
	{
		Json::Value root;
		root[CONNECT_CMD]=SOCK_CMD_GET_DAYPAY_LIST;
		root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]]=T2A(strDate);
		for (int i = 0; i < nSize;i++)
		{
			Json::Value one1;
			one1[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]]=T2A(m_vet[i].strStaffID);
			one1[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFNAME]]=T2A(m_vet[i].strname);
			root[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one1);
		}
		Json::FastWriter writer;  
		string temp = writer.write(root);
		g_Globle.SendTo(temp);
	}
}

void CWorkCalDlg::GetOnePayList(Json::Value root)
{
	Json::FastWriter writer;  
	string temp = writer.write(root);
	m_vWorkCal.clear();
	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json:: Value ones1 = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		if (ones1.isArray())
		{
			int nCount = ones1.size();
			for (int i=0;i<nCount;i++)//人数循环
			{
				Json:: Value one1 = ones1[i];
				WORKMONEY cal;
				cal.strStaffID = one1[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]].asCString();
				cal.strName = one1[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFNAME]].asCString();
				if (one1.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
				{
					Json:: Value ones = one1[CMD_RetType[EM_CMD_RETYPE_VALUE]];
					if (ones.isArray())
					{
						int nNum = ones.size();
						for (int j=0;j<nNum;j++)//做工循环
						{
							Json:: Value one = ones[j];

							DAYPAY_TYPE type;
							type = (DAYPAY_TYPE)one[DAYPAYMSG[EM_DAYPAY_MSG_TYPE]].asInt();

							CString strMoney;
							strMoney = one[DAYPAYMSG[EM_DAYPAY_MSG_MONEY]].asCString();

							if (type == DAYPAY_TYPE_DEL)
							{
								cal.del_money += _ttof(strMoney);
							}
							else if (type == DAYPAY_TYPE_DAY)
							{
								cal.day_money += _ttof(strMoney);
							}
							else if(type == DAYPAY_TYPE_JIJIAN)
							{
								cal.jj_money += _ttof(strMoney);
							}				
						}
						m_vWorkCal.push_back(cal);
					}
				}
			}
		}
	}
}

void CWorkCalDlg::SetListValue()
{
	double all_money = 0;
	CString str;
	m_ListCtrl.DeleteAllItems();
	int nNum = m_vet.size();
	for (int j=0;j<nNum;j++)
	{
		int bFind = false;
		int nCount = m_vWorkCal.size();
		for (int i=0;i<nCount;i++)
		{
			if (m_vet[j].strStaffID == m_vWorkCal[i].strStaffID)
			{
				bFind = true;
				double money = 0;
				money = m_vWorkCal[i].day_money + m_vWorkCal[i].jj_money - m_vWorkCal[i].del_money;

				str.Format(L"%d",j+1);
				m_ListCtrl.InsertItem(j,str);//序号

				m_ListCtrl.SetItemText(j,1,m_vWorkCal[i].strName);//姓名

				if (m_vWorkCal[i].day_money == 0)
					str.Format(L"0");
				else
				    str.Format(L"%.02f",m_vWorkCal[i].day_money);
				m_ListCtrl.SetItemText(j,2,str);//按天工资

				if (m_vWorkCal[i].jj_money == 0)
					str.Format(L"0");
				else
					str.Format(L"%.02f",m_vWorkCal[i].jj_money);
				m_ListCtrl.SetItemText(j,3,str);//计件工资

				if (m_vWorkCal[i].del_money == 0)
					str.Format(L"0");
				else
					str.Format(L"%.02f",m_vWorkCal[i].del_money);
				m_ListCtrl.SetItemText(j,4,str);//扣除工资

				str.Format(L"%.02f",money);
				m_ListCtrl.SetItemText(j,5,str);//共计
				all_money += money;
				break;
			}
		}
		if (!bFind)
		{
			str.Format(L"%d",j+1);
			m_ListCtrl.InsertItem(j,str);//序号
			m_ListCtrl.SetItemText(j,1,m_vet[j].strname);//姓名
			str.Format(L"0");
			m_ListCtrl.SetItemText(j,2,str);//按天工资
			m_ListCtrl.SetItemText(j,3,str);//计件工资
			m_ListCtrl.SetItemText(j,4,str);//扣除工资
			m_ListCtrl.SetItemText(j,5,str);//共计
		}
		m_ListCtrl.SetItemData(j,(DWORD_PTR)&m_vet[j]);
	}
	
	str.Format(L"总计：%.04f",all_money);
	m_total.SetWindowTextW(str);
}
