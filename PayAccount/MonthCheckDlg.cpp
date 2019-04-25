// PayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "MonthCheckDlg.h"
#include "afxdialogex.h"


// CMonthCheckDlg 对话框

IMPLEMENT_DYNAMIC(CMonthCheckDlg, CDialog)

void MonthCheckCallback(void* p,string strData)
{
	USES_CONVERSION;
	CMonthCheckDlg* pThis=(CMonthCheckDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_MONTHCHECK_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CMonthCheckDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
				SendToGetMonthPay();
			}
		}
		break;
	case SOCK_CMD_GET_MPAY:
		{
			if (ret == NET_CMD_FAIL)
			{
				m_comboMonth.EnableWindow(TRUE);
				GetDlgItem(IDC_BTN_UPDATE)->EnableWindow(TRUE);
				m_staw.ShowWindow(SW_HIDE);
				m_listCtrl.ShowWindow(SW_SHOW);
				MessageBox(L"获取数据失败！",L"错误");
			}
			else
			{
				GetMonthPay(root);
			}
		}
		break;

	}
	return TRUE;
}

CMonthCheckDlg::CMonthCheckDlg(CWnd* pParent /*=NULL*/)
: CPayAccountDlg(pParent)
{

}

CMonthCheckDlg::~CMonthCheckDlg()
{
}

void CMonthCheckDlg::SendToGetStaff()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_SAMPLE_STAFF;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CMonthCheckDlg::GetStaff(Json::Value root)
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
				m_vet.push_back(stu);
			}
		}
	}
}

void CMonthCheckDlg::GetMonthPay(Json::Value root)
{
	Json::FastWriter writer;  
	string strData = writer.write(root);

	vector<MONTH_PAY_STAFF> vStaffs;
	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json::Value ones1 = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		int nsize1 = ones1.size();
		for (int i=0;i<nsize1;i++)//职工循环
		{
			Json::Value one1 = ones1[i];
			MONTH_PAY_STAFF staff;
			staff.strStaffID = one1[MPAYMSG[EM_GET_MPAY_STAFFID]].asCString();
			staff.strStaffName = one1[MPAYMSG[EM_GET_MPAY_STAFFNAME]].asCString();
			if (one1.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
			{
				Json::Value ones2 = one1[CMD_RetType[EM_CMD_RETYPE_VALUE]];
				int nsize2 = ones2.size();
				for (int j=0;j<nsize2;j++)//日期循环
				{
					Json::Value one2 = ones2[j];
					MONTHPAY_DAY day;
					day.ndex = one2[MPAYMSG[EM_GET_MPAY_DEX]].asInt();
					if (one2.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
					{
						Json::Value ones3 = one2[CMD_RetType[EM_CMD_RETYPE_VALUE]];
						int nsize3 = ones3.size();
						for (int k=0;k<nsize3;k++)//日做工循环
						{
							Json::Value one3 = ones3[k];
							DAYPAY day_pay;
							day_pay.type = (DAYPAY_TYPE)one3[MPAYMSG[EM_GET_MPAY_TYPE]].asInt();
							day_pay.money = one3[MPAYMSG[EM_GET_MPAY_MONEY]].asCString();
							day.v_daypay.push_back(day_pay);
						}
					}
					//按ndex从小到大排列
					if(staff.vDays.size() == 0)
						staff.vDays.push_back(day);
					else
					{
						bool bInset=false;
						list <MONTHPAY_DAY>::iterator it;
						for ( it = staff.vDays.begin( ) ; it != staff.vDays.end( ) ; it++ )
						{
							if (day.ndex<(*it).ndex)
							{
								bInset = true;
								staff.vDays.insert(it,day);
								break;
							}
						}
						if(!bInset)
							staff.vDays.push_back(day);
					}
				}
			}
			vStaffs.push_back(staff);
		}
	}
	SetListValue(vStaffs);
}

void CMonthCheckDlg::SendToGetMonthPay()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_MPAY;
	
	CString strYear, strMonth,strDate,strTmp;
	m_comboYear.GetLBText(m_comboYear.GetCurSel(), strYear);
	m_comboMonth.GetLBText(m_comboMonth.GetCurSel(), strMonth);

	int nYear = _ttoi(strYear);
	int nMonth = _ttoi(strMonth);
	int nDays = g_Globle.GetDays(nYear, nMonth);

	int nSize = m_vet.size();
	if (nSize>0 && nDays>0)
	{
		for (int i = 0; i < nSize;i++)
		{
			Json::Value one1;
			one1[MPAYMSG[EM_GET_MPAY_STAFFID]]=T2A(m_vet[i].strStaffID);
			one1[MPAYMSG[EM_GET_MPAY_STAFFNAME]]=T2A(m_vet[i].strname);
			for (int j = 0; j < nDays;j++)
			{
				strDate = strYear + "//" + strMonth;
				strDate.Format(L"%s/%s/%02d", strYear, strMonth, j + 1);

				Json::Value one2;
				one2[MPAYMSG[EM_GET_MPAY_DATE]]=T2A(strDate);
				one2[MPAYMSG[EM_GET_MPAY_DEX]]=j+2;
				one1[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one2);
			}
			root[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one1);
		}
	}
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"发送网络请求失败，请检查网络是否正常！",L"提示");
	}
	else
	{
		m_comboMonth.EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_UPDATE)->EnableWindow(FALSE);
		m_listCtrl.ShowWindow(SW_HIDE);
		m_staw.ShowWindow(SW_SHOW);
	}
}

void CMonthCheckDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(MonthCheckCallback,this);

	//初始化控件位置
	CRect rectThis;
	GetClientRect(&rectThis);
	CRect rectList;
	m_listCtrl.GetClientRect(&rectList);
	rectList.top = rectThis.top + 50;
	rectList.left = rectThis.left+10;
	rectList.right = rectThis.right-10;
	rectList.bottom = rectThis.bottom -40;
	m_listCtrl.MoveWindow(rectList, TRUE);

	CRect rectmsg;
	m_total.GetClientRect(&rectmsg);
	int width = rectmsg.Width();
	int height = rectmsg.Height();
	rectmsg.left = rectList.right - width;
	rectmsg.top = rectList.bottom+1;
	rectmsg.right = rectList.right;
	rectmsg.bottom = rectmsg.top+height;
	m_total.MoveWindow(rectmsg, TRUE);

	CRect rectw;
	m_total.GetClientRect(&rectw);
	width = rectw.Width();
	height = rectw.Height();
	rectw.left = rectThis.right/2-width/2;
	rectw.top = rectThis.bottom/2-height/2;
	rectw.right = rectw.left+width;
	rectw.bottom = rectw.top+height;
	m_staw.MoveWindow(rectw, TRUE);

	SendToGetStaff();
}

void CMonthCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_COMBO_YEAR, m_comboYear);
	DDX_Control(pDX, IDC_COMBO_MONTH, m_comboMonth);
	DDX_Control(pDX, IDC_TOTAL, m_total);
	DDX_Control(pDX, IDC_STAW, m_staw);
}


BEGIN_MESSAGE_MAP(CMonthCheckDlg, CPayAccountDlg)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_YEAR, &CMonthCheckDlg::OnCbnSelchangeComboYear)
	ON_CBN_SELCHANGE(IDC_COMBO_MONTH, &CMonthCheckDlg::OnCbnSelchangeComboMonth)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CMonthCheckDlg::OnBnClickedBtnUpdate)
	ON_MESSAGE(WM_MONTHCHECK_CALL, &CMonthCheckDlg::OnCallBack)
END_MESSAGE_MAP()


// CMonthCheckDlg 消息处理程序


void CMonthCheckDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CPayAccountDlg::OnPaint()
	//UpdateDlg();
}


BOOL CMonthCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化字体大小
	m_font.CreatePointFont(180, _T("宋体"));
	m_total.SetFont(&m_font);

	//初始化年份控件
	CTime tm = CTime::GetCurrentTime();
	int nYear = tm.GetYear();
	for (int i = 0; i < 8; i++)
	{
		CString str;
		str.Format(L"%d", nYear - (8 - i-1));
		m_comboYear.InsertString(i, str);
		if (i == 7)
		{
			m_comboYear.SetCurSel(i);
		}
	}
	//初始化月份控件
	for (int i = 0; i < 12; i++)
	{
		CString str;
		str.Format(L"%02d", i + 1);
		m_comboMonth.AddString(str);
		if (i + 1 == tm.GetMonth())
		{
			m_comboMonth.SetCurSel(i);
		}
	}

	SetListFontSize();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CMonthCheckDlg::SetListFontSize()
{
	LOGFONT logfont;
	CFont *pfont1 = m_listCtrl.GetFont();  
	pfont1->GetLogFont(&logfont);  
	logfont.lfHeight = logfont.lfHeight * 1.2; 
	logfont.lfWidth = logfont.lfWidth * 1.2;
	static CFont font1;  
	font1.CreateFontIndirect(&logfont);  
	m_listCtrl.SetFont(&font1);  
	font1.Detach();
}

void CMonthCheckDlg::SetListValue(vector<MONTH_PAY_STAFF> vStaffs)
{
	CString strYear, strMonth,strDate,strTmp;
	m_comboYear.GetLBText(m_comboYear.GetCurSel(), strYear);
	m_comboMonth.GetLBText(m_comboMonth.GetCurSel(), strMonth);

	int nYear = _ttoi(strYear);
	int nMonth = _ttoi(strMonth);
	int nDays = g_Globle.GetDays(nYear, nMonth);

	m_listCtrl.DeleteAllItems();
	while (m_listCtrl.DeleteColumn(0))
	{
	}

	LV_COLUMN lvcolumn;
	CRect rect;
	m_listCtrl.GetWindowRect(&rect);
	for (int i = 0; i < nDays+2; i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		if (i == 0)
		{
			lvcolumn.pszText =  L"姓名";
		}
		else if (i == 1)
		{
			lvcolumn.pszText =  L"总额";
		}
		else
		{
			TCHAR a[10] = {0};
			_itot(i-1, a, 10);
			lvcolumn.pszText = a;

		}
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		lvcolumn.cx = 65;
		m_listCtrl.InsertColumn(i, &lvcolumn);
	}
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrl.SetBkColor(RGB(102, 205, 170));
	m_listCtrl.SetTextBkColor(RGB(102, 205, 170));

	CImageList imagelist;
	imagelist.Create(1, 20, ILC_COLOR, 1, 1);
	m_listCtrl.SetImageList(&imagelist, LVSIL_SMALL);

	double fTotal = 0;
	int nSize = vStaffs.size();
	for (int i = 0; i < nSize;i++)//职工循环
	{
		double allMoney = 0;
		m_listCtrl.InsertItem(i, vStaffs[i].strStaffName);
		for (auto it = vStaffs[i].vDays.begin();it!=vStaffs[i].vDays.end();it++)//天数循环
		{
			double money = 0;
			for (int n = 0; n < it->v_daypay.size();n++)
			{
				if (it->v_daypay[n].type == DAYPAY_TYPE_DEL)
					money -= _ttof(it->v_daypay[n].money);
				else
					money += _ttof(it->v_daypay[n].money);
			}
			allMoney += money;
			strTmp.Format(L"%.2lf", money);
			if (money == 0)
				m_listCtrl.SetItemText(i, it->ndex, L"");
			else
				m_listCtrl.SetItemText(i, it->ndex, strTmp);
		}
		/*
		for (int j = 0; j < vStaffs[i].vDays.size();j++)//天数循环
		{
			double money = 0;
			for (int n = 0; n < vStaffs[i].vDays[j].v_daypay.size();n++)
			{
				if (vStaffs[i].vDays[j].v_daypay[n].type == DAYPAY_TYPE_DEL)
					money -= _ttof(vStaffs[i].vDays[j].v_daypay[n].money);
				else
				    money += _ttof(vStaffs[i].vDays[j].v_daypay[n].money);
			}
			allMoney += money;
			strTmp.Format(L"%.2lf", money);
			if (money == 0)
				m_listCtrl.SetItemText(i, vStaffs[i].vDays[j].ndex, L"");
			else
			    m_listCtrl.SetItemText(i, vStaffs[i].vDays[j].ndex, strTmp);
		}
		*/
		fTotal+=allMoney;
		strTmp.Format(L"%.2lf", allMoney);
		m_listCtrl.SetItemText(i, 1, strTmp);
	}
	CString tmp;
	tmp.Format(L"总支出：%.02f",fTotal);
	SetDlgItemText(IDC_TOTAL,tmp);

	m_comboMonth.EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_UPDATE)->EnableWindow(TRUE);
	m_staw.ShowWindow(SW_HIDE);
	m_listCtrl.ShowWindow(SW_SHOW);
}

void CMonthCheckDlg::OnCbnSelchangeComboYear()
{
	// TODO:  在此添加控件通知处理程序代码
	SendToGetMonthPay();
}


void CMonthCheckDlg::OnCbnSelchangeComboMonth()
{
	// TODO:  在此添加控件通知处理程序代码
	SendToGetMonthPay();
}


void CMonthCheckDlg::OnBnClickedBtnUpdate()
{
	// TODO:  在此添加控件通知处理程序代码
	SendToGetStaff();
}
