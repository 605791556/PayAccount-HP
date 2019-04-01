// BookMngDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "BookMngDlg.h"
#include "AddBookDlg.h"
#include "PayAccountDlg.h"


extern CPayAccountDlg* g_PayAccountDlg;

// CBookMngDlg 对话框

IMPLEMENT_DYNAMIC(CBookMngDlg, CDialog)

void BookMngCallback(void* p,string strData)
{
	USES_CONVERSION;
	CBookMngDlg* pThis=(CBookMngDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_BOOKMNG_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CBookMngDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_GET_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取图书信息失败！",L"错误");
			else
			{
				GetBook(root);
			}
		}
		break;
	case SOCK_CMD_DEL_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"删除失败！",L"错误");
			else
			{
				OnBnClickedButton1();
				MessageBox(L"删除成功！");
			}
		}
		break;
	}
	return TRUE;
}

CBookMngDlg::CBookMngDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBookMngDlg::IDD, pParent)
{
	m_nPage=1;
	m_dex=1;
	m_date_Type=EM_DATE_TYPE_1WEEK;
	m_rkType=BOOK_RK_MAX;
}

CBookMngDlg::~CBookMngDlg()
{
}

void CBookMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_comboRk);
	DDX_Control(pDX, IDC_STA_ALL, m_sta_all);
	DDX_Control(pDX, IDC_STA_GO, m_sta_go);
	DDX_Control(pDX, IDC_EDIT_GO, m_edit_go);
	DDX_Control(pDX, IDC_BTN_GO, m_btn_go);
	DDX_Control(pDX, IDC_BTN_LAST, m_BtnLast);
	DDX_Control(pDX, IDC_BTN_NEXT, m_BtnNext);
	DDX_Control(pDX, IDC_COMBO_DATE, m_comboDate);
}


BEGIN_MESSAGE_MAP(CBookMngDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CBookMngDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CBookMngDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CBookMngDlg::OnNMRClickList1)
	ON_COMMAND(ID_MU_DEL, DelBook)
	ON_COMMAND(ID_MU_MDF, MdfBook)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_GO, &CBookMngDlg::OnBnClickedBtnGo)
	ON_BN_CLICKED(IDC_BTN_LAST, &CBookMngDlg::OnBnClickedBtnLast)
	ON_BN_CLICKED(IDC_BTN_NEXT, &CBookMngDlg::OnBnClickedBtnNext)
	ON_MESSAGE(WM_BOOKMNG_CALL, &CBookMngDlg::OnCallBack)
	ON_CBN_SELCHANGE(IDC_COMBO_DATE, &CBookMngDlg::OnCbnSelchangeComboDate)
END_MESSAGE_MAP()


// CBookMngDlg 消息处理程序
BOOL CBookMngDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化combobox
	m_comboRk.InsertString(0,L"全部");
	for (int i=0;i<BOOK_RK_MAX;i++)
	{
		m_comboRk.InsertString(i+1,rkType[i]);
	}
	m_comboRk.SetCurSel(0);

	for (int i=0;i<EM_DATE_TYPE_MAX;i++)
	{
		m_comboDate.InsertString(i,DateName[i]);
	}
	m_comboDate.SetCurSel(1);


	TCHAR rgtsz[15][10] = { _T("序号"), _T("下单日期"),_T("书名"),_T("出版社"), _T("版次"), _T("成品尺寸"),  _T("开本"),_T("印张"), _T("印数"), _T("包册"),_T("令数"), _T("印张类型"), _T("折页类型"),_T("入库"),_T("备注") };

	LV_COLUMN lvcolumn;
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	for (int i=0;i<15;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i==0)
		{
			lvcolumn.cx = 50;
		}
		else if (i == 1)
			lvcolumn.cx = 150;
		else if (i == 2 || i == 3)
			lvcolumn.cx = 200;
		else if (i==4 || i==5)
			lvcolumn.cx = 100;
		else if (i == 14)
			lvcolumn.cx = 150;
		else
		{
			lvcolumn.cx = 80;
		}
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(102 ,205 ,170));
	m_ListCtrl.SetTextBkColor(RGB(102 ,205, 170)); 

	CRect rt;
	m_ListCtrl.GetClientRect(&rt);
	CImageList imagelist;
	imagelist.Create(1, 25, ILC_COLOR, 1, 1);
	m_ListCtrl.SetImageList(&imagelist, LVSIL_SMALL);
	SetListFontSize();

	//设置回调
	g_Globle.SetCallback(BookMngCallback,this);
	SendToGetBook(L"",m_rkType,m_date_Type,(m_dex-1)*20,20);
	return TRUE;
}

void CBookMngDlg::SendToGetBook(CString strKeyWord,BOOK_RK rkType,EM_DATE_TYPE date_Type,int nStart,int nNum)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_BOOK;
	root[CMD_GETBOOK[GETBOOK_KEYWORD]] = T2A(strKeyWord); 
	root[CMD_GETBOOK[GETBOOK_RKTYPE]] = rkType;
	root[CMD_GETBOOK[GETBOOK_DATE]] = date_Type;
	root[CMD_GETBOOK[GETBOOK_NSTART]] = nStart;
	root[CMD_GETBOOK[GETBOOK_NUMBER]] = nNum;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CBookMngDlg::SendToDelBook(CString strBookID)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_DEL_BOOK;
	root[CMD_DELBOOK[GETBOOK_KEYWORD]] = T2A(strBookID); 
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CBookMngDlg::GetBook(Json::Value root)
{
	m_nCount = 0;
	m_vet.clear();
	m_ListCtrl.DeleteAllItems();

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
		//按时间排序
		if(m_vet.size() == 0)
			m_vet.push_back(stu);
		else
		{
			bool bInset=false;
			vector <BOOK_STU>::iterator it;
			for ( it = m_vet.begin( ) ; it != m_vet.end( ) ; it++ )
			{
				if (JudgeDateStringSize(stu.strDate,it->strDate)<=0)
				{
					bInset = true;
					m_vet.insert(it,stu);
					break;
				}
			}
			if(!bInset)
				m_vet.push_back(stu);
		}
	}
	m_nCount = root[CMD_BOOKMSG[BOOKMSG_XCOUNT]].asInt();
	SetListCtrlValue();
}

void CBookMngDlg::SetListFontSize()
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

void CBookMngDlg::SetListCtrlValue()
{
	int n=1;
	CString str;
	for (int i=0;i<m_vet.size();i++)
	{
		str.Format(L"%d",n+(m_dex-1)*20);
		m_ListCtrl.InsertItem(i,str);
		m_ListCtrl.SetItemText(i,1,m_vet[i].strDate);
		m_ListCtrl.SetItemText(i,2,m_vet[i].strname);
		m_ListCtrl.SetItemText(i,3,m_vet[i].strCbs);
		str.Format(L"%d--%d",m_vet[i].nBc1,m_vet[i].nBc2);
		m_ListCtrl.SetItemText(i,4,str);//版次
		str.Format(L"%d*%d",m_vet[i].nSize1,m_vet[i].nSize2);
		m_ListCtrl.SetItemText(i,5,str);//成品尺寸
		str.Format(L"%d",m_vet[i].nKb);
		m_ListCtrl.SetItemText(i,6,str);//开本
		str.Format(L"%.02f",m_vet[i].fYz);
		m_ListCtrl.SetItemText(i,7,str);//印张
		str.Format(L"%d",m_vet[i].nYs);
		m_ListCtrl.SetItemText(i,8,str);//印数
		str.Format(L"%d",m_vet[i].nBc);
		m_ListCtrl.SetItemText(i,9,str);//包册
		str.Format(L"%.02f",m_vet[i].fLs);
		m_ListCtrl.SetItemText(i,10,str);//令数
		if (m_vet[i].type != BOOK_TYPE_MAX)
		{
			m_ListCtrl.SetItemText(i, 11, BookType[m_vet[i].type]);
		}
		if (m_vet[i].zyType != ZHEYEPAY_TYPE_MAX)
		{
			m_ListCtrl.SetItemText(i, 12, ZyType[m_vet[i].zyType-1]);
		}
		if (m_vet[i].rkType != BOOK_RK_MAX)
		{
			m_ListCtrl.SetItemText(i, 13, rkType[m_vet[i].rkType]);
		}
		m_ListCtrl.SetItemText(i,14,m_vet[i].strMsg);//备注
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

int CBookMngDlg::JudgeDateStringSize(CString strDate1,CString strDate2)
{
	if (strDate1 == strDate2)
		return 0;

	int nYear1,nMonth1,nDay1;
	int nYear2,nMonth2,nDay2;

	int nFind = -1;
	nFind = strDate1.Find('/');
	if (nFind>0)
	{
		CString strYear = strDate1.Mid(0,nFind);
		nYear1 =_ttoi(strYear);
		strDate1 = strDate1.Mid(nFind+1,strDate1.GetLength()-nFind-1);
		nFind = strDate1.Find('/');
		if (nFind>0)
		{
			CString strMonth = strDate1.Mid(0,nFind);
			CString strDay = strDate1.Mid(nFind+1,strDate1.GetLength()-nFind-1);
			nMonth1 =_ttoi(strMonth);
			nDay1 =_ttoi(strDay);
		}
	}

	nFind = -1;
	nFind = strDate2.Find('/');
	if (nFind>0)
	{
		CString strYear = strDate2.Mid(0,nFind);
		nYear2 =_ttoi(strYear);
		strDate2 = strDate2.Mid(nFind+1,strDate2.GetLength()-nFind-1);
		nFind = strDate2.Find('/');
		if (nFind>0)
		{
			CString strMonth = strDate2.Mid(0,nFind);
			CString strDay = strDate2.Mid(nFind+1,strDate2.GetLength()-nFind-1);
			nMonth2 =_ttoi(strMonth);
			nDay2 =_ttoi(strDay);
		}
	}
	long ndays1 = (nYear1*12+nMonth1)*60+nDay1;
	long ndays2 = (nYear2*12+nMonth2)*60+nDay2;
	if (ndays1 < ndays2)
		return -1;
	else if(ndays1 == ndays2)
		return 0;
	else 
		return 1;
}

//添加
void CBookMngDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CAddBookDlg dlg;
	int ret = dlg.DoModal();
	if(ret==IDOK)
	{
		//通知添加了图书,请相关页面更新数据
		g_PayAccountDlg->SendMessageW(WM_VDATAUPDATE);
	}
	g_Globle.SetCallback(BookMngCallback,this);
	OnBnClickedButton1();
}

//查询
void CBookMngDlg::OnBnClickedButton1()
{
	m_dex = 1;
	int ndex = m_comboRk.GetCurSel();
	if (ndex == 0)
		m_rkType = BOOK_RK_MAX;
	else
		m_rkType = BOOK_RK(ndex -1);

	m_date_Type = (EM_DATE_TYPE)m_comboDate.GetCurSel();

	FindBook(m_dex);
}

void CBookMngDlg::FindBook(int pageDex)
{
	m_vet.clear();
	m_ListCtrl.DeleteAllItems();
	CString strKeyWord= L"";
	GetDlgItemText(IDC_KEYWORD,strKeyWord);
	SendToGetBook(strKeyWord,m_rkType,m_date_Type,(pageDex-1)*20,20);
}

void CBookMngDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);
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

void CBookMngDlg::DelBook()
{
	CString strDex=m_ListCtrl.GetItemText(m_nItem,0);
	CString strName = m_ListCtrl.GetItemText(m_nItem, 2);
	int nDex = _ttoi(strDex)-1;
	CString strBookID = m_vet[nDex].strBookID;
	CString tmp;
	tmp.Format(L"该操作将会删除与该书相关的所有记录，包括（进度，明细，做工统计），确认删除 '%s' ？",strName);
	if(MessageBox(tmp,TEXT("警告"),MB_OKCANCEL|MB_ICONEXCLAMATION )==IDOK)
	{
		SendToDelBook(strBookID);
	}
}

void CBookMngDlg::MdfBook()
{
	CAddBookDlg dlg;
	dlg.SetNotifyWnd(this);
	int ret = dlg.DoModal();
	g_Globle.SetCallback(BookMngCallback,this);
	if(ret==IDOK)
	{
		//通知添加了图书,请相关页面更新数据
		//g_PayAccountDlg->SendMessageW(WM_VDATAUPDATE);
		OnBnClickedButton1();
	}
}

void CBookMngDlg::OnSize(UINT nType, int cx, int cy)
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


void CBookMngDlg::OnBnClickedBtnGo()
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
		FindBook(m_dex);
	}
}


void CBookMngDlg::OnBnClickedBtnLast()
{
	if (m_dex>1)
	{
		m_dex--;
		FindBook(m_dex);
	}
}


void CBookMngDlg::OnBnClickedBtnNext()
{
	if (m_dex<m_nPage)
	{
		m_dex++;
		FindBook(m_dex);
	}
}


void CBookMngDlg::OnCbnSelchangeComboDate()
{

}
