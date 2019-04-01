// StaffWriteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "StaffWriteDlg.h"
#include "afxdialogex.h"
#include "..\common\common.h"
#include "..\common\status.h"

// CStaffWriteDlg 对话框

IMPLEMENT_DYNAMIC(CStaffWriteDlg, CDialog)

void StaffWriteCallback(void* p,string strData)
{
	USES_CONVERSION;
	CStaffWriteDlg* pThis=(CStaffWriteDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}

	Json::Reader r;
	Json::Value root;
	r.parse(strData,root);

	EM_SOCK_CMD cmd = (EM_SOCK_CMD)root[CONNECT_CMD].asInt();
	EM_CMD_RET ret = (EM_CMD_RET)root[CMD_RetType[EM_CMD_RETYPE_RESULT]].asInt();

	switch (cmd)
	{
	case SOCK_CMD_GET_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				pThis->MessageBox(L"获取图书信息失败！",L"错误");
			else
				pThis->GetBook(root);
		}
		break;
	case SOCK_CMD_GET_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				pThis->MessageBox(L"获取项目信息失败！",L"错误");
			else
				pThis->GetProject(root);
		}
		break;
	case SOCK_CMD_GET_PROGRESS:
		{
			Json::FastWriter writer;  
			string temp = writer.write(root);

			if (ret == NET_CMD_FAIL)
				pThis->MessageBox(L"获取进度信息失败！",L"错误");
			else
				pThis->GetProgress(root);
		}
		break;
	case SOCK_CMD_GET_STAFFWRITE:
		{
			if (ret == NET_CMD_FAIL)
				pThis->MessageBox(L"获取单个进度信息失败！",L"错误");
			else
				pThis->GetStaffWrite(root);
		}
		break;
	case SOCK_CMD_SAVE_STAFFWRITE:
		{
			if (ret == NET_CMD_FAIL)
				pThis->MessageBox(L"保存失败！",L"错误");
			else
			{
				pThis->MessageBox(L"保存成功！",L"");
				pThis->UpdateDlg();
			}
		}
		break;
	default:
		break;
	}
}

CStaffWriteDlg::CStaffWriteDlg(CWnd* pParent /*=NULL*/)
	: CPayAccountDlg(pParent)
{
	m_number.m_type = EDIT_TYPE_FLOAT;
	m_Handle = INVALID_HANDLE_VALUE;
	m_bExit = FALSE;
}

CStaffWriteDlg::~CStaffWriteDlg()
{
	m_bExit = TRUE;
	if (m_Handle!=INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_Handle,INFINITE);
		m_Handle=INVALID_HANDLE_VALUE;
	}
}

void CStaffWriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BOOK, m_combo_book);
	DDX_Control(pDX, IDC_COMBO_PRO, m_combo_pro);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_number);
	DDX_Control(pDX, IDOK, m_btnsave);
}


BEGIN_MESSAGE_MAP(CStaffWriteDlg, CPayAccountDlg)
	ON_BN_CLICKED(IDOK, &CStaffWriteDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CStaffWriteDlg::OnBnClickedBtnUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO_BOOK, &CStaffWriteDlg::OnCbnSelchangeComboBook)
	ON_CBN_SELCHANGE(IDC_COMBO_PRO, &CStaffWriteDlg::OnCbnSelchangeComboPro)
END_MESSAGE_MAP()


// CStaffWriteDlg 消息处理程序

void CStaffWriteDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(StaffWriteCallback,this);
	//发送网络请求
	SendToGetBook();
	
	CRect rectThis;
	GetClientRect(&rectThis);
	CRect rectList;
	m_ListCtrl.GetClientRect(&rectList);
	CRect rcBtn;
	m_btnsave.GetClientRect(&rcBtn);

	rectList.top = 140;
	rectList.left = rectThis.left+10;
	rectList.right = rectThis.right-10;
	rectList.bottom = rectList.top+80;
	m_ListCtrl.MoveWindow(rectList, TRUE);
}

//保存
void CStaffWriteDlg::OnBnClickedOk()
{
	int ndex = m_combo_book.GetCurSel();
	CString strBookID = *((CString*)m_combo_book.GetItemData(ndex));
	ndex = m_combo_pro.GetCurSel();
	PROJECT_STU pro_stu = *(PROJECT_STU*)m_combo_pro.GetItemData(ndex);
	CString strNumber;
	GetDlgItemText(IDC_EDIT1,strNumber);
	if (strNumber.IsEmpty())
	{
		MessageBox(L"请输入完成数量。",L"提示");
		return;
	}

	double number = _ttof(strNumber);
	if (pro_stu.pn_type == PRO_NUM_TYPE_YS)
	{
		CString strYs = m_ListCtrl.GetItemText(0,0);
		int nYs = _ttoi(strYs);
		int maxYs = nYs + (nYs/100);
		if (number > maxYs)
		{
			MessageBox(L"超出运行最大范围值，请重新输入！",L"提示");
			return;
		}
	}
	else if (pro_stu.pn_type == PRO_NUM_TYPE_LS)
	{
		CString strLs = m_ListCtrl.GetItemText(0,0);
		double fls = _ttof(strLs);
		double maxLs = fls + (fls/100);
		if (number > maxLs)
		{
			MessageBox(L"超出运行最大范围值，请重新输入！",L"提示");
			return;
		}
	}

	SendToSaveStaffWrite(strBookID,pro_stu.nID,number);
}

DWORD WINAPI UpdateThread(LPVOID lpParam)
{
	CStaffWriteDlg* pThis = (CStaffWriteDlg*)lpParam;
	int waitMinSec = 2*60*1000;//2分钟刷新一次
	int nNUm = waitMinSec/200;
	for (int i=0;i<nNUm && !pThis->m_bExit;i++)
	{
		Sleep(200);
		if (i == nNUm - 1)
		{
			pThis->UpdateDlg();
			i = 0;
		}
	}
	return 0;
}

BOOL CStaffWriteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_font.CreateFont(20, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD, // nWeight 
		TRUE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("仿宋"));

	SetListFontSize();
	if (g_Globle.m_nType == TYPE_COMMON)
	{
		m_Handle = CreateThread(NULL,0,UpdateThread,this,0,NULL);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CStaffWriteDlg::SetListFontSize()
{
	GetDlgItem(IDC_STA1)->SetFont(&m_font);
	GetDlgItem(IDC_STA2)->SetFont(&m_font);
	GetDlgItem(IDC_STA3)->SetFont(&m_font);
	GetDlgItem(IDC_EDIT1)->SetFont(&m_font);
	GetDlgItem(IDOK)->SetFont(&m_font);
	GetDlgItem(IDC_BTN_UPDATE)->SetFont(&m_font);
	m_combo_book.SetFont(&m_font);
	m_combo_pro.SetFont(&m_font);

	LOGFONT logfont;
	CFont *pfont1 = m_ListCtrl.GetFont();  
	pfont1->GetLogFont(&logfont);  
	logfont.lfHeight = logfont.lfHeight * 1.8; 
	logfont.lfWidth = logfont.lfWidth * 1.8;
	static CFont font1;  
	font1.CreateFontIndirect(&logfont);  
	m_ListCtrl.SetFont(&font1);  
	font1.Detach();
}

void CStaffWriteDlg::GetBook(Json::Value root)
{
	m_vet.clear();
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
		m_vet.push_back(stu);
	}
	SetComboBookValue();
	SendToGetPro();
}

void CStaffWriteDlg::GetProject(Json::Value root)
{
	m_vPro.clear();
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
		if(m_vPro.size() == 0)
			m_vPro.push_back(stu);
		else
		{
			bool bInset=false;
			vector <PROJECT_STU>::iterator it;
			for ( it = m_vPro.begin( ) ; it != m_vPro.end( ) ; it++ )
			{
				if (stu.ndex<(*it).ndex)
				{
					bInset = true;
					m_vPro.insert(it,stu);
					break;
				}
			}
			if(!bInset)
				m_vPro.push_back(stu);
		}
	}
	SetComboProValue();
	InitListCtrl();
	SendToGetStaffWrite();
}

void CStaffWriteDlg::SetComboBookValue()
{
	m_combo_book.ResetContent();
	int nCount = m_vet.size();
	int nFindex = 0;
	for (int i = 0; i < nCount; i++)
	{
		m_combo_book.InsertString(i,m_vet[i].strname);
		m_combo_book.SetItemData(i,(DWORD_PTR)&m_vet[i].strBookID);
		if(m_LastBookID == m_vet[i].strBookID)
			nFindex = i;
	}
	if (nCount>0)
	{
		m_combo_book.SetCurSel(nFindex);
	}
}

void CStaffWriteDlg::SetComboProValue()
{
	m_combo_pro.ResetContent();
	int nCount = m_vPro.size();
	int nFindex = 0;
	for (int i = 0; i < nCount; i++)
	{
		m_combo_pro.InsertString(i,m_vPro[i].strName);
		m_combo_pro.SetItemData(i,(DWORD_PTR)&m_vPro[i]);
		if(m_LastProID == m_vPro[i].nID)
			nFindex = i;
	}
	if (nCount>0)
	{
		m_combo_pro.SetCurSel(nFindex);
	}
}

void CStaffWriteDlg::GetProgress(Json::Value root)
{
	CString str;
	int ys = root[CMD_PROGRESSMSG[EM_PROGRESSMSG_YS]].asInt();
	double fls = root[CMD_PROGRESSMSG[EM_PROGRESSMSG_YS]].asDouble();

	str.Format(L"%d",ys);
	m_ListCtrl.InsertItem(0, str);//印数
	str.Format(L"%.02f",fls);
	m_ListCtrl.SetItemText(0,1,str);//令数

	if(root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json::Value jss=root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		if (jss.isArray())
		{
			for (int i=0;i<jss.size();i++)
			{
				double number=0;
				int ndex = jss[i][CMD_PROGRESSMSG[EM_PROGRESSMSG_NDEX]].asInt();
				number = jss[i][CMD_PROGRESSMSG[EM_PROGRESSMSG_NUMBER]].asDouble();

				str.Format(L"%.02f",number);
				m_ListCtrl.SetItemText(0,ndex,str);
			}
		}
	}
}

void CStaffWriteDlg::GetStaffWrite(Json::Value root)
{
	if(root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		double number = root[CMD_RetType[EM_CMD_RETYPE_VALUE]].asDouble();
		CString str;
		str.Format(L"%.02f",number);
		SetDlgItemText(IDC_EDIT1,str);
	}
	else
		SetDlgItemText(IDC_EDIT1,L"0");

	InitListCtrl();
}

void CStaffWriteDlg::SendToGetBook()
{
	BOOK_RK em_rk = BOOK_RK_NO;
	int n = 0;

	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_BOOK;
	root[CMD_GETBOOK[GETBOOK_KEYWORD]] = ""; 
	root[CMD_GETBOOK[GETBOOK_RKTYPE]] = em_rk;
	root[CMD_GETBOOK[GETBOOK_NSTART]] = n;
	root[CMD_GETBOOK[GETBOOK_NUMBER]] = 1000;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffWriteDlg::SendToGetPro()
{
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PROJECT;
	root[CMD_GETPRO[EM_GETPRO_BWRITE]] = PRO_STAFF_TYPE_YES;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffWriteDlg::SendToGetProgress(CString strBookID)
{
	USES_CONVERSION;
	Json::Value root;
	int num = m_vPro.size();
	for (int i=0;i<num;i++)
	{
		Json::Value one;
		one[CMD_GETPROGRESS[EM_GETPROGRESS_PROID]] = m_vPro[i].nID;
		one[CMD_GETPROGRESS[EM_GETPROGRESS_INDEX]] = i+2;
		root[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one);
	}
	root[CONNECT_CMD]=SOCK_CMD_GET_PROGRESS;
	root[CMD_GETPROGRESS[EM_GETPROGRESS_BOOKID]] = T2A(strBookID);

	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffWriteDlg::SendToGetStaffWrite()
{
	int ndex = m_combo_book.GetCurSel();
	CString strBookID = *((CString*)m_combo_book.GetItemData(ndex));
	ndex = m_combo_pro.GetCurSel();
	PROJECT_STU pro_stu = *(PROJECT_STU*)m_combo_pro.GetItemData(ndex);

	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_STAFFWRITE;
	root[CMD_GETPROGRESS[EM_GETPROGRESS_BOOKID]] = T2A(strBookID);
	root[CMD_GETPROGRESS[EM_GETPROGRESS_PROID]] = pro_stu.nID;

	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffWriteDlg::SendToSaveStaffWrite(CString strBookID,int proID,double number)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SAVE_STAFFWRITE;
	root[CMD_GETPROGRESS[EM_GETPROGRESS_BOOKID]] = T2A(strBookID);
	root[CMD_GETPROGRESS[EM_GETPROGRESS_PROID]] = proID;
	root[CMD_RetType[EM_CMD_RETYPE_VALUE]] = number;

	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CStaffWriteDlg::InitListCtrl()
{
	TCHAR rgtsz[2][10] = {_T("印数"), _T("令数") };
	m_ListCtrl.DeleteAllItems();
	while (m_ListCtrl.DeleteColumn(0))
	{
	}

	LV_COLUMN lvcolumn;
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	for (int i=0;i<2;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		lvcolumn.cx = 120;
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}

	int nt = m_vPro.size();
	for (int i=0;i<nt;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = (LPWSTR)(LPCWSTR)m_vPro[i].strName;
		lvcolumn.iSubItem = i+2;
		lvcolumn.iOrder = i+2;
		lvcolumn.cx = (rect.Width()-160)/nt-1;
		m_ListCtrl.InsertColumn(i+2, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(102 ,205 ,170));
	m_ListCtrl.SetTextBkColor(RGB(102 ,205, 170));

	CRect rt;
	m_ListCtrl.GetClientRect(&rt);
	CImageList imagelist;
	imagelist.Create(1, 30, ILC_COLOR, 1, 1);
	m_ListCtrl.SetImageList(&imagelist, LVSIL_SMALL);

	SetListCtrlValue();
}

void CStaffWriteDlg::SetListCtrlValue()
{
	m_ListCtrl.DeleteAllItems();
	//基本信息
	CString str,strBookID;
	int ndex = m_combo_book.GetCurSel();
	if (ndex>=0)
	{
		strBookID = *((CString*)m_combo_book.GetItemData(ndex));
		SendToGetProgress(strBookID);
	}
}


void CStaffWriteDlg::OnBnClickedBtnUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateDlg();
}


void CStaffWriteDlg::OnCbnSelchangeComboBook()
{
	// TODO: 在此添加控件通知处理程序代码
	int ndex = m_combo_book.GetCurSel();
	if(ndex>=0)
		m_LastBookID = *((CString*)m_combo_book.GetItemData(ndex));
	SendToGetStaffWrite();
}


void CStaffWriteDlg::OnCbnSelchangeComboPro()
{
	// TODO: 在此添加控件通知处理程序代码
	int ndex = m_combo_pro.GetCurSel();
	if(ndex>=0)
		m_LastProID = (*((PROJECT_STU*)m_combo_pro.GetItemData(ndex))).nID;
	SendToGetStaffWrite();
}
