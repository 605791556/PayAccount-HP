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

BOOL CMonthCheckDlg::PreTranslateMessage(MSG* pMsg)
{ 
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{	
		return TRUE;
	}
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
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
	m_vStaffs.clear();
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

				MONTH_PAY_STAFF staff;
				staff.strStaffName = js[i][CMD_STAFFMSG[EM_STAFF_MSG_NAME]].asCString();
				staff.strStaffID = js[i][CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]].asCString();
				m_vStaffs.push_back(staff);
			}
		}
	}
}

int findDay(CString strDate)
{
	int nfind = strDate.ReverseFind('/');
	CString str = strDate.Mid(nfind+1,2);
	return _ttoi(str);
}

void CMonthCheckDlg::GetMonthPay(Json::Value root)
{
	InitListCtrl();
	for (auto it = m_vStaffs.begin();it!=m_vStaffs.end();it++)
		it->resetValue();

	Json::FastWriter writer;  
	string strData = writer.write(root);

	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json::Value js = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		int size = js.size();
		for (int i=0;i<size;i++)
		{
			Json::Value one = js[i];
			
			MONTH_PAY_STAFF staff;
			staff.strStaffID = one[MPAYMSG[EM_GET_MPAY_STAFFID]].asCString();
			staff.strStaffName = one[MPAYMSG[EM_GET_MPAY_STAFFNAME]].asCString();

			MONTHPAY_DAY day;
			day.strDate = one[MPAYMSG[EM_GET_MPAY_DATE]].asCString();
			day.ndex = findDay(day.strDate)+1;

			DAYPAY day_pay;
			day_pay.type = (DAYPAY_TYPE)one[MPAYMSG[EM_GET_MPAY_TYPE]].asInt();
			day_pay.money = one[MPAYMSG[EM_GET_MPAY_MONEY]].asCString();

			for (int j = 0;j<m_vStaffs.size();j++)
			{
				if (m_vStaffs[j].strStaffID == staff.strStaffID)
				{
					CString strDate;
					strDate = one[MPAYMSG[EM_GET_MPAY_DATE]].asCString();

					if (m_vStaffs[j].vDays.size() == 0)
					{
						if (day_pay.type == DAYPAY_TYPE_DEL)
						{
							day.d_money -= _ttof(day_pay.money);
							m_vStaffs[j].m_money -= _ttof(day_pay.money);
						}
						else
						{
							day.d_money += _ttof(day_pay.money);
							m_vStaffs[j].m_money += _ttof(day_pay.money);
						}

						m_vStaffs[j].vDays.push_back(day);
					}
					else
					{
						bool bInsert = false;
						for (auto it = m_vStaffs[j].vDays.begin();it != m_vStaffs[j].vDays.end();it++)
						{
							if (it->strDate == strDate)//该日期已经插入
							{
								bInsert = true;
								if (day_pay.type == DAYPAY_TYPE_DEL)
								{
									it->d_money -= _ttof(day_pay.money);
									m_vStaffs[j].m_money -= _ttof(day_pay.money);
								}
								else
								{
									it->d_money += _ttof(day_pay.money);
									m_vStaffs[j].m_money += _ttof(day_pay.money);
								}
								break;
							}
						}
						if (bInsert == false)//该日期还没插入
						{
							if (day_pay.type == DAYPAY_TYPE_DEL)
							{
								day.d_money -= _ttof(day_pay.money);
								m_vStaffs[j].m_money -= _ttof(day_pay.money);
							}
							else
							{
								day.d_money += _ttof(day_pay.money);
								m_vStaffs[j].m_money += _ttof(day_pay.money);
							}

							m_vStaffs[j].vDays.push_back(day);
						}
					}
					break;
				}
			}
		}
	}

	double fTotal = 0;
	for (int i=0;i<m_vStaffs.size();i++)
		fTotal+=m_vStaffs[i].m_money;

	CString tmp;
	tmp.Format(L"总支出：%.02f",fTotal);
	SetDlgItemText(IDC_TOTAL,tmp);

	m_comboMonth.EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_UPDATE)->EnableWindow(TRUE);
	m_staw.ShowWindow(SW_HIDE);
	m_listCtrl.ShowWindow(SW_SHOW);

	m_listCtrl.SetItemCount(m_vStaffs.size());
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

	strDate.Format(L"%s/%s", strYear, strMonth);
	root[MPAYMSG[EM_GET_MPAY_DATE]]=T2A(strDate);

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
	DDX_Control(pDX, IDC_EDIT_KEYWORD, m_keywordCtrl);
}


BEGIN_MESSAGE_MAP(CMonthCheckDlg, CPayAccountDlg)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_YEAR, &CMonthCheckDlg::OnCbnSelchangeComboYear)
	ON_CBN_SELCHANGE(IDC_COMBO_MONTH, &CMonthCheckDlg::OnCbnSelchangeComboMonth)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CMonthCheckDlg::OnBnClickedBtnUpdate)
	ON_MESSAGE(WM_MONTHCHECK_CALL, &CMonthCheckDlg::OnCallBack)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &CMonthCheckDlg::OnLvnGetdispinfoList1)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LIST1, &CMonthCheckDlg::OnLvnOdfinditemList1)
	ON_EN_CHANGE(IDC_EDIT_KEYWORD, &CMonthCheckDlg::OnEnChangeEditKeyword)
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

void CMonthCheckDlg::InitListCtrl()
{
	USES_CONVERSION;
	CString strYear, strMonth,strDate;
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
			CString strr;
			strr.Format(L"%02d",i-1);
			lvcolumn.pszText = (LPWSTR)(LPCTSTR)strr;
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
	m_keywordCtrl.SetWindowTextW(L"");
	SendToGetStaff();
}


void CMonthCheckDlg::OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strTmp;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	int row= pItem->iItem;//行
	int column = pItem->iSubItem;//列

	if (pItem->mask & LVIF_TEXT)
	{
		if (column == 0)//姓名
		{
			lstrcpy(pItem->pszText,m_vStaffs[row].strStaffName);
		}
		else if (column == 1)//总额
		{
			strTmp.Format(L"%.2lf",  m_vStaffs[row].m_money);
			lstrcpy(pItem->pszText,strTmp);
		}
		else
		{
			double money = 0;
			for (auto it = m_vStaffs[row].vDays.begin();it!= m_vStaffs[row].vDays.end();it++)
			{
				if (it->ndex == column)
				{
					strTmp.Format(L"%.2lf", it->d_money);
					lstrcpy(pItem->pszText,strTmp);
					break;
				}
			}
		}
	}
	*pResult = 0;
}


void CMonthCheckDlg::OnLvnOdfinditemList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVFINDITEM pFindInfo = reinterpret_cast<LPNMLVFINDITEM>(pNMHDR);
	*pResult =-1;

	if( (pFindInfo->lvfi.flags & LVFI_STRING) == 0 )
		return;

	CString strFind = pFindInfo->lvfi.psz;
	if (strFind.IsEmpty()) return;

	//判断是否最后一行
	int nItemCount = m_listCtrl.GetItemCount();
	if(nItemCount == 0) return;
	int currentPos=0;


	//开始查找
	do
	{
		int nFind = m_vStaffs[currentPos].strStaffName.Find(strFind);
		if (nFind == 0)
		{
			*pResult = currentPos;
			m_vItem.push_back(currentPos);
		}
		currentPos++;

	}while(currentPos < nItemCount);
}

void CMonthCheckDlg::OnEnChangeEditKeyword()
{
	m_vItem.clear();
	int nItemCount = m_listCtrl.GetItemCount();
	m_listCtrl.SetItemState(-1,0,-1);

	CString strKeyWord;
	m_keywordCtrl.GetWindowTextW(strKeyWord);

	LVFINDINFO info;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = strKeyWord;
	int n = m_listCtrl.FindItem(&info);
	if (n > 0)
	{
		for (int i=0;i<m_vItem.size();i++)
		{
			m_listCtrl.SetItemState(m_vItem[i],LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);
		}
	}
}
