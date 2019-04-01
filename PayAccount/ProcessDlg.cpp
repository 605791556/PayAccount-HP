// WorkCalDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "ProcessDlg.h"
#include "afxdialogex.h"
#include "..\common\common.h"
#include "..\common\status.h"

// CProcessDlg 对话框

IMPLEMENT_DYNAMIC(CProcessDlg, CDialog)

CProcessDlg::CProcessDlg(CWnd* pParent /*=NULL*/)
	: CPayAccountDlg(pParent)
{

}

CProcessDlg::~CProcessDlg()
{
}

void CProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO_RK, m_combo_rk);
	DDX_Control(pDX, IDC_COMBO_BOOK, m_combo_book);
}


BEGIN_MESSAGE_MAP(CProcessDlg, CPayAccountDlg)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CProcessDlg::OnBnClickedBtnUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO_RK, &CProcessDlg::OnCbnSelchangeComboRk)
	ON_CBN_SELCHANGE(IDC_COMBO_BOOK, &CProcessDlg::OnCbnSelchangeComboBook)
END_MESSAGE_MAP()


void ProcessCallback(void* p,string strData)
{
	USES_CONVERSION;
	CProcessDlg* pThis=(CProcessDlg*) p;

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
	case SOCK_CMD_GET_SAMPLE_BOOK:
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
	default:
		break;
	}
}

// CProcessDlg 消息处理程序
BOOL CProcessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_combo_rk.ResetContent();
	m_combo_rk.InsertString(0,L"全部");
	for (int i=0;i<BOOK_RK_MAX;i++)
	{
		m_combo_rk.InsertString(i+1,rkType[i]);
	}
	m_combo_rk.SetCurSel(1);

	SetListFontSize();
	return TRUE;
}

void CProcessDlg::SetListFontSize()
{
	LOGFONT logfont;
	CFont *pfont1 = m_ListCtrl.GetFont();  
	pfont1->GetLogFont(&logfont);  
	logfont.lfHeight = logfont.lfHeight * 1.3; 
	logfont.lfWidth = logfont.lfWidth * 1.3;
	static CFont font1;  
	font1.CreateFontIndirect(&logfont);  
	m_ListCtrl.SetFont(&font1);  
	font1.Detach();
}

void CProcessDlg::GetBook(Json::Value root)
{
	m_vet.clear();
	Json::FastWriter writer;  
	string temp = writer.write(root);

	int nCount = root[CMD_BOOKMSG[BOOKMSG_COUNT]].asInt();
	for (int i=0;i<nCount;i++)
	{
		char str[125]={0};
		BOOK_STU stu;
		Json::Value one = root[itoa(i,str,10)];

		stu.strBookID=one[CMD_BOOKMSG[BOOKMSG_ID]].asCString();
		stu.strname=one[CMD_BOOKMSG[BOOKMSG_NAME]].asCString();
	
		m_vet.push_back(stu);
	}
	nCount = m_vet.size();
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

	//发送获取项目的网络请求
	SendToGetPro();
}

void CProcessDlg::GetProject(Json::Value root)
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
	InitListCtrl();
}

void CProcessDlg::GetProgress(Json::Value root)
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

void CProcessDlg::SendToGetBook()
{
	BOOK_RK em_rk;
	int ndex = m_combo_rk.GetCurSel();
	if (ndex == 0)
		em_rk = BOOK_RK_MAX;
	else
		em_rk = BOOK_RK(ndex -1);

	m_combo_book.ResetContent();
	int n = 0;

	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_SAMPLE_BOOK;
	root[CMD_GETBOOK[GETBOOK_RKTYPE]] = em_rk;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CProcessDlg::SendToGetPro()
{
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PROJECT;
	root[CMD_GETPRO[EM_GETPRO_BWRITE]] = PRO_STAFF_TYPE_YES;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CProcessDlg::SendToGetProgress(CString strBookID)
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

void CProcessDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(ProcessCallback,this);
	//发送网络请求
	SendToGetBook();
	
	CRect rectThis;
	GetClientRect(&rectThis);
	CRect rectList;
	m_ListCtrl.GetClientRect(&rectList);
	rectList.top = rectThis.top + 50;
	rectList.left = rectThis.left+10;
	rectList.right = rectThis.right-10;
	rectList.bottom = rectThis.bottom -40;
	m_ListCtrl.MoveWindow(rectList, TRUE);
}

void CProcessDlg::InitListCtrl()
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
		lvcolumn.cx = 80;
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
		lvcolumn.cx = 120;
		m_ListCtrl.InsertColumn(i+2, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(102 ,205 ,170));
	m_ListCtrl.SetTextBkColor(RGB(102 ,205, 170));

	CRect rt;
	m_ListCtrl.GetClientRect(&rt);
	CImageList imagelist;
	imagelist.Create(1, 25, ILC_COLOR, 1, 1);
	m_ListCtrl.SetImageList(&imagelist, LVSIL_SMALL);

	SetListCtrlValue();
}

void CProcessDlg::SetListCtrlValue()
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

//刷新
void CProcessDlg::OnBnClickedBtnUpdate()
{
	UpdateDlg();
}


void CProcessDlg::OnCbnSelchangeComboRk()
{
	SendToGetBook();
}


void CProcessDlg::OnCbnSelchangeComboBook()
{
	// TODO: 在此添加控件通知处理程序代码
	int ndex = m_combo_book.GetCurSel();
	if(ndex>=0)
		m_LastBookID = *((CString*)m_combo_book.GetItemData(ndex));
	InitListCtrl();
}
