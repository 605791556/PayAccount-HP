// ProgressDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "DetailDlg.h"
#include "afxdialogex.h"


// CDetailDlg 对话框
IMPLEMENT_DYNAMIC(CDetailDlg, CDialog)

void DetailCallback(void* p,string strData)
{
	USES_CONVERSION;
	CDetailDlg* pThis=(CDetailDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_DETAIL_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CDetailDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
				InitListCtrl();
				SendToGetDetails();
			}
		}
		break;
	case SOCK_CMD_GET_DETAILS:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"获取明细失败！",L"错误");
			else
			{
				GetDetails(root);
			}
		}
		break;
	}
	return TRUE;
}

CDetailDlg::CDetailDlg(CWnd* pParent /*=NULL*/)
: CPayAccountDlg(pParent)
{

}

CDetailDlg::~CDetailDlg()
{
}

BOOL CDetailDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CDetailDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_comboRk.ResetContent();
	m_comboRk.InsertString(0,L"全部");
	for (int i=0;i<BOOK_RK_MAX;i++)
	{
		m_comboRk.InsertString(i+1,rkType[i]);
	}
	m_comboRk.SetCurSel(1);

	SetListFontSize();
	return TRUE;
}

void CDetailDlg::UpdateDlg()
{
	//设置回调
	g_Globle.SetCallback(DetailCallback,this);
	SendToGetBook();

	CRect rectThis;
	GetClientRect(&rectThis);
	CRect rectList;
	m_ListCtrl.GetClientRect(&rectList);
	rectList.top = rectThis.top + 55;
	rectList.left = rectThis.left+10;
	rectList.right = rectThis.right-10;
	rectList.bottom = rectThis.bottom -40;
	m_ListCtrl.MoveWindow(rectList, TRUE);
}

void CDetailDlg::SendToGetBook()
{
	BOOK_RK em_rk;
	int ndex = m_comboRk.GetCurSel();
	if (ndex == 0)
		em_rk = BOOK_RK_MAX;
	else
		em_rk = BOOK_RK(ndex -1);

	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_SAMPLE_BOOK;
	root[CMD_GETBOOK[GETBOOK_RKTYPE]] = em_rk;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDetailDlg::SendToGetProject()
{
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PROJECT;
	root[CMD_GETPRO[EM_GETPRO_BWRITE]] = PRO_STAFF_TYPE_MAX;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CDetailDlg::SendToGetDetails()
{
	USES_CONVERSION;
	int ndex = m_combooks.GetCurSel();
	int nSize = m_vPro.size();
	if (ndex>=0 && nSize>0)
	{
		CString strBookID;
		strBookID = *((CString*)m_combooks.GetItemData(ndex));

		Json::Value root;
		root[CONNECT_CMD]=SOCK_CMD_GET_DETAILS;
		root[CMD_GETDETAILS[EM_GET_DETAILS_BOOKID]] = T2A(strBookID);
		for (int i=0;i<nSize;i++)
		{
			Json::Value one;
			one[CMD_GETDETAILS[EM_GET_DETAILS_PROID]] = m_vPro[i].nID;
			one[CMD_GETDETAILS[EM_GET_DETAILS_NDEX]] = i;
			root[CMD_GETDETAILS[EM_GET_DETAILS_PROIDS]].append(one);
		}
		Json::FastWriter writer;  
		string temp = writer.write(root);
		g_Globle.SendTo(temp);
	}
}
void CDetailDlg::GetBook(Json::Value root)
{
	m_vet.clear();
	m_combooks.ResetContent();
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
		m_vet.push_back(stu);
	}
	nCount = m_vet.size();
	int nFindex = 0;
	for (int i = 0; i < nCount; i++)
	{
		m_combooks.InsertString(i,m_vet[i].strname);
		m_combooks.SetItemData(i,(DWORD_PTR)&m_vet[i].strBookID);
		if(m_LastBookID == m_vet[i].strBookID)
			nFindex = i;
	}
	if (nCount>0)
	{
		m_combooks.SetCurSel(nFindex);
	}
}

void CDetailDlg::GetProject(Json::Value root)
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
}

void CDetailDlg::GetDetails(Json::Value root)
{
	int nNum = m_vPro.size();
	int* n = new int[nNum];
	for (int i=0;i<nNum;i++)
		n[i] = 0;

	CString strDate;
	strDate = root[CMD_BOOKMSG[BOOKMSG_DATE]].asCString();
	int nYs = root[CMD_BOOKMSG[BOOKMSG_YS]].asInt();
	double fLs = root[CMD_BOOKMSG[BOOKMSG_LS]].asDouble();

	vector<PRO_DETAIL> vts;
	if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
	{
		Json::Value root1 = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
		if (root1.isArray())
		{
			int nSize1 = root1.size();
			if (nSize1 == nNum)
			{
				for (int i=0;i<nNum;i++)//项目遍历
				{
					PRO_DETAIL proDtl;
					Json::Value one1 = root1[i];
					proDtl.nProID = one1[CMD_DETAILMSG[EM_DETAIL_MSG_PROID]].asInt();
					proDtl.ndex = one1[CMD_DETAILMSG[EM_DETAIL_MSG_NDEX]].asInt();
					if (one1.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
					{
						Json::Value root2 = one1[CMD_RetType[EM_CMD_RETYPE_VALUE]];
						if (root2.isArray())
						{
							int nSize2 = root2.size();
							for (int j=0;j<nSize2;j++)//明细遍历
							{
								STU_DETAIL stu;
								Json::Value one2 = root2[j];
								stu.strName = one2[CMD_DETAILMSG[EM_DETAIL_MSG_NAME]].asCString();
								stu.stridCard = one2[CMD_DETAILMSG[EM_DETAIL_MSG_IDCARD]].asCString();
								stu.number = one2[CMD_DETAILMSG[EM_DETAIL_MSG_NUMBER]].asInt();
								proDtl.vDetails.push_back(stu);
							}
							//对vDetails容器处理，如有相同姓名的明细则合并
							for (int j=0;j<proDtl.vDetails.size();j++)
							{
								if (proDtl.vDetails.size() >= 2 && j<proDtl.vDetails.size()-1)
								{
									vector<STU_DETAIL>::iterator it ;
									vector<STU_DETAIL>::iterator src = proDtl.vDetails.begin()+j;
									for(it=proDtl.vDetails.begin()+j+1;it!=proDtl.vDetails.end();)
									{
										if(src->stridCard == it->stridCard)
										{
											src->number+=it->number;
											it=proDtl.vDetails.erase(it);  
										}
										else
											++it;
									}
								}
								else
									break;
							}
							n[i] = proDtl.vDetails.size();
						}
					}
					vts.push_back(proDtl);
				}
			}	
		}
	}
	SetListValue(strDate,nYs,fLs, nNum,n,vts);
}

void CDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combooks);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO4, m_comboRk);
}


BEGIN_MESSAGE_MAP(CDetailDlg, CPayAccountDlg)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CDetailDlg::OnBnClickedBtnUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDetailDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CDetailDlg::OnCbnSelchangeCombo4)
	ON_MESSAGE(WM_DETAIL_CALL, &CDetailDlg::OnCallBack)
END_MESSAGE_MAP()


void CDetailDlg::InitListCtrl()
{
	TCHAR rgtsz[3][10] = {_T("下单日期"), _T("印数"), _T("令数") };
	m_ListCtrl.DeleteAllItems();
	while (m_ListCtrl.DeleteColumn(0))
	{
	}

	LV_COLUMN lvcolumn;
	CRect rect;
	m_ListCtrl.GetWindowRect(&rect);
	for (int i=0;i<3;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i==0)
		{
			lvcolumn.cx = 100;
		}
		else
		{
			lvcolumn.cx = 80;
		}
		m_ListCtrl.InsertColumn(i, &lvcolumn);
	}

	int nt = m_vPro.size();
	for (int i=0;i<nt;i++)
	{
		PROJECT_STU stu = m_vPro[i];
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = (LPWSTR)(LPCWSTR)stu.strName;
		lvcolumn.iSubItem = 3+i;
		lvcolumn.iOrder = 3+i;
		lvcolumn.cx = 120;
		m_ListCtrl.InsertColumn(3+i, &lvcolumn);
	}
	m_ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListCtrl.SetBkColor(RGB(102 ,205 ,170));
	m_ListCtrl.SetTextBkColor(RGB(102 ,205, 170));

	CRect rt;
	m_ListCtrl.GetClientRect(&rt);
	CImageList imagelist;
	imagelist.Create(1, 25, ILC_COLOR, 1, 1);
	m_ListCtrl.SetImageList(&imagelist, LVSIL_SMALL);
	UpdateData(FALSE);
}

void CDetailDlg::SetListFontSize()
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

void CDetailDlg::SetListValue(CString strDate,int nYs,double fLs,int nNum,int* n,vector<PRO_DETAIL> vts)
{
	m_ListCtrl.DeleteAllItems();
	//基本信息
	CString str;
	m_ListCtrl.InsertItem(0, strDate);//下单日期
	str.Format(L"%d",nYs);
	m_ListCtrl.SetItemText(0,1,str);//印数
	str.Format(L"%.02f",fLs);
	m_ListCtrl.SetItemText(0,2,str);//令数

	int max=0,dex=0;
	for (int i=0;i<nNum;i++)
	{
		if (n[i]>max)
		{
			max = n[i];
			dex=i;
		}
	}
	
	for (int i=0;i<n[dex];i++)
	{
		if (i > 0)
		{
			m_ListCtrl.InsertItem(i, L"");//下单日期
			m_ListCtrl.SetItemText(i,1,L"");//印数
			m_ListCtrl.SetItemText(i,2,L"");//令数
		}
		CString str;
		for (int j=0;j<nNum;j++)
		{
			int nTmp = vts[j].vDetails.size();
			if (i < nTmp)
				str.Format(L"%s：%d",vts[j].vDetails[i].strName,vts[j].vDetails[i].number);
			else
				str=L"";
			m_ListCtrl.SetItemText(i,3+vts[j].ndex,str);
		}
	}
	//最后一行统计
	if (max > 0)
	{
		m_ListCtrl.InsertItem(max, L"");//下单日期
		m_ListCtrl.SetItemText(max,1,L"");//印数
		m_ListCtrl.SetItemText(max,2,L"");//令数
	}

	for (int i = 0;i<nNum;i++)
	{
		CString tmp;
		int number_all = 0;
		int nTmp = vts[i].vDetails.size();
		for (int j =0;j<nTmp;j++)
		{
			number_all+=vts[i].vDetails[j].number;
		}
		tmp.Format(L"合计：%d",number_all);
		m_ListCtrl.SetItemText(max,3+vts[i].ndex,tmp);
	}
	delete[] n;
}

void CDetailDlg::OnBnClickedBtnUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateDlg();
}


void CDetailDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int ndex = m_combooks.GetCurSel();
	if(ndex>=0)
		m_LastBookID = *((CString*)m_combooks.GetItemData(ndex));
	SendToGetDetails();
}


void CDetailDlg::OnCbnSelchangeCombo4()
{
	SendToGetBook();
}
