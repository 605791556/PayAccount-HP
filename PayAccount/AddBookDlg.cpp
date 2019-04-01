// AddBookDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "AddBookDlg.h"


// CAddBookDlg 对话框

IMPLEMENT_DYNAMIC(CAddBookDlg, CDialog)

void AddBookCallback(void* p,string strData)
{
	USES_CONVERSION;
	CAddBookDlg* pThis=(CAddBookDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_ADDBOOK_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CAddBookDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_ADD_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"添加失败！",L"错误");
			else
			{
				MessageBox(L"添加成功",L"提示");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_MDF_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"修改失败！",L"错误");
			else
			{
				MessageBox(L"修改成功",L"提示");
				OnOK();
			}
		}
		break;
	case SOCK_CMD_JUDGE_BOOK:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"判断图书是否存在时发生错误！",L"提示");
			else
			{
				bool bHad = root[CMD_JUDGEBOOK[EM_JUDGE_BOOK_BHAD]].asBool();
				if(bHad)
				{
					if(IDYES == MessageBox(L"该图书名称已存在，是否继续添加？",L"提示",MB_YESNO))
					{
						SendToAddOrMdfBook();
					}
				}
				else
				{
					SendToAddOrMdfBook();	
				}
			}
		}
		break;
	}
	return TRUE;
}

CAddBookDlg::CAddBookDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAddBookDlg::IDD, pParent)
{
	m_ndex = -1;
	m_dlg=NULL;
	m_bAdd=true;
	m_edit_yz.m_type = EDIT_TYPE_FLOAT;
	m_edit_ls.m_type = EDIT_TYPE_FLOAT;
}

CAddBookDlg::CAddBookDlg(CBookMngDlg* dlg)
{
	m_dlg=dlg;
	m_bAdd=false;
}

CAddBookDlg::~CAddBookDlg()
{
}

void CAddBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_ComboZy);
	DDX_Control(pDX, IDC_XD_DATE, m_date);
	DDX_Control(pDX, IDC_COMBO3, m_comboRk);
	DDX_Control(pDX, IDC_YZ, m_edit_yz);
	DDX_Control(pDX, IDC_LS, m_edit_ls);
}


BEGIN_MESSAGE_MAP(CAddBookDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddBookDlg::OnBnClickedOk)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_XD_DATE, &CAddBookDlg::OnDtnDatetimechangeXdDate)
	ON_MESSAGE(WM_ADDBOOK_CALL, &CAddBookDlg::OnCallBack)
END_MESSAGE_MAP()

BOOL CAddBookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//初始化combobox
	m_ComboZy.InsertString(0, L"请选择");
	for (int i = 0; i < ZHEYEPAY_TYPE_MAX-1;i++)
	{
		m_ComboZy.InsertString(i+1, ZyType[i]);
	}
	m_ComboZy.SetCurSel(0);

	for (int i=0;i<BOOK_RK_MAX;i++)
	{
		m_comboRk.InsertString(i,rkType[i]);
	}
	m_comboRk.SetCurSel(0);

	if (m_dlg)
	{
		SetModifyInit();
	}

	//设置回调
	g_Globle.SetCallback(AddBookCallback,this);
	return TRUE;
}

void CAddBookDlg::SetNotifyWnd(CBookMngDlg* dlg)
{
	m_dlg=dlg;
	m_bAdd=false;
}

void CAddBookDlg::SetModifyInit()
{
	//序号
	CString strDex=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,0);
	m_ndex = m_dlg->m_nItem;
	//下单日期
	CString strDate=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,1);
	m_date.SetFormat(strDate);
	//书名
	CString strname=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,2);
	SetDlgItemText(IDC_NAME,strname);
	//出版社
	CString strCbs=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,3);
	SetDlgItemText(IDC_CBS,strCbs);

	//版次
	int bc1,bc2;
    CString strBc=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,4);
	int nFind = strBc.Find(L"--");
	if (nFind>=0)
	{
		CString str1 = strBc.Mid(0,nFind);
		CString str2 = strBc.Mid(nFind+2,strBc.GetLength()-nFind);
		bc1 = _ttoi(str1);
		bc2 = _ttoi(str2);
	}
	SetDlgItemInt(IDC_BC1,bc1);
	SetDlgItemInt(IDC_BC2,bc2);

	//成品尺寸
	int size1,size2;
	CString strSize=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,5);
	nFind = strSize.Find(L"*");
	if (nFind>=0)
	{
		CString str1 = strSize.Mid(0,nFind);
		CString str2 = strSize.Mid(nFind+1,strSize.GetLength());
		size1 = _ttoi(str1);
		size2 = _ttoi(str2);
	}
	SetDlgItemInt(IDC_SIZE1,size1);
	SetDlgItemInt(IDC_SIZE2,size2);

	//开本
	CString strKb=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,6);
	SetDlgItemInt(IDC_KB,_ttoi(strKb));

	//印张
	CString strYz=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,7);
	SetDlgItemText(IDC_YZ,strYz);

	//印数
	CString strYs=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,8);
	SetDlgItemInt(IDC_YS,_ttoi(strYs));
	//包册
	CString strBaoCe=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,9);
	SetDlgItemInt(IDC_BC,_ttoi(strBaoCe));
	//令数
	CString strLs=m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem,10);
	CString str;
	str.Format(L"%.02f",_ttof(strLs));
	SetDlgItemText(IDC_LS,str);

	CString strType = m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem, 11);
	if (strType == "全开")
		((CButton*)GetDlgItem(IDC_RADIO_QUAN))->SetCheck(TRUE);
	else if (strType == "对开")
		((CButton*)GetDlgItem(IDC_RADIO_DUI))->SetCheck(TRUE);

	CString strZyType = m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem, 12);
	for (int i = 0; i < ZHEYEPAY_TYPE_MAX - 1;i++)
	{
		if (ZyType[i] == strZyType)
		{
			m_ComboZy.SetCurSel(i + 1);
			break;
		}
	}

	CString strRkType = m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem, 13);
	for (int i = 0; i < BOOK_RK_MAX;i++)
	{
		if (rkType[i] == strRkType)
		{
			m_comboRk.SetCurSel(i);
			break;
		}
	}

	CString strMsg = m_dlg->m_ListCtrl.GetItemText(m_dlg->m_nItem, 14);
	SetDlgItemText(IDC_EDIT_MSG,strMsg);

	CButton *pBtn = (CButton *)GetDlgItem(IDOK);
	pBtn->SetWindowText(L"保存");
}

// CAddBookDlg 消息处理程序

void CAddBookDlg::SendToJudgeBook(CString strName)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_JUDGE_BOOK;
	root[CMD_JUDGEBOOK[EM_JUDGE_BOOK_NAME]]=T2A(strName);
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

void CAddBookDlg::SendToAddOrMdfBook(EM_SOCK_CMD type)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=type;
	root[CMD_BOOKMSG[BOOKMSG_ID]]=T2A(m_bookstu.strBookID);
	root[CMD_BOOKMSG[BOOKMSG_NAME]]=T2A(m_bookstu.strname);
	root[CMD_BOOKMSG[BOOKMSG_CBS]]=T2A(m_bookstu.strCbs);;
	root[CMD_BOOKMSG[BOOKMSG_DATE]]=T2A(m_bookstu.strDate);
	root[CMD_BOOKMSG[BOOKMSG_BC1]]=m_bookstu.nBc1;
	root[CMD_BOOKMSG[BOOKMSG_BC2]]=m_bookstu.nBc2;
	root[CMD_BOOKMSG[BOOKMSG_SIZE1]]=m_bookstu.nSize1;
	root[CMD_BOOKMSG[BOOKMSG_SIZE2]]=m_bookstu.nSize2;
	root[CMD_BOOKMSG[BOOKMSG_KB]]=m_bookstu.nKb;
	root[CMD_BOOKMSG[BOOKMSG_YZ]]=m_bookstu.fYz;
	root[CMD_BOOKMSG[BOOKMSG_YS]]=m_bookstu.nYs;
	root[CMD_BOOKMSG[BOOKMSG_BC]]=m_bookstu.nBc;
	root[CMD_BOOKMSG[BOOKMSG_LS]]=m_bookstu.fLs;
	root[CMD_BOOKMSG[BOOKMSG_B_TYPE]]=m_bookstu.type;
	root[CMD_BOOKMSG[BOOKMSG_ZY_TYPE]]=m_bookstu.zyType;
	root[CMD_BOOKMSG[BOOKMSG_RK_TYPE]]=m_bookstu.rkType;
	root[CMD_BOOKMSG[BOOKMSG_MSG]]=T2A(m_bookstu.strMsg);;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	g_Globle.SendTo(temp);
}

//添加图书
void CAddBookDlg::OnBnClickedOk()
{
	USES_CONVERSION;
	GetDlgItemText(IDC_NAME,m_bookstu.strname);
	GetDlgItemText(IDC_CBS,m_bookstu.strCbs);
	CTime time;
	m_date.GetTime(time);
	m_bookstu.strDate = time.Format(L"%Y/%m/%d");
	m_bookstu.nBc1 = GetDlgItemInt(IDC_BC1);
	m_bookstu.nBc2 = GetDlgItemInt(IDC_BC2);

	m_bookstu.nSize1 = GetDlgItemInt(IDC_SIZE1);
	m_bookstu.nSize2 = GetDlgItemInt(IDC_SIZE2);

	m_bookstu.nKb = GetDlgItemInt(IDC_KB);//开本

	CString strYz;
	GetDlgItemText(IDC_YZ,strYz);//印张
    m_bookstu.fYz = atof(W2A(strYz));
	m_bookstu.nYs = GetDlgItemInt(IDC_YS);
	
	m_bookstu.nBc = GetDlgItemInt(IDC_BC);//包册

	CString strLs;
	GetDlgItemText(IDC_LS,strLs);//令数
	m_bookstu.fLs = atof(W2A(strLs));

	if (m_bookstu.strname == L"" || m_bookstu.nBc1==0 || m_bookstu.nBc2==0 ||m_bookstu.nSize1==0||m_bookstu.nSize2==0||m_bookstu.nKb==0||strYz==L""||m_bookstu.nYs==0|| m_bookstu.nBc==0||strLs==L"")
	{
		MessageBox(L"请将信息填写完整！",L"提示");
		return;
	}
	BOOK_TYPE type = BOOK_TYPE_MAX;
	if (((CButton*)GetDlgItem(IDC_RADIO_QUAN))->GetCheck())
		m_bookstu.type = BOOK_TYPE_QUAN;
	else if (((CButton*)GetDlgItem(IDC_RADIO_DUI))->GetCheck())
		m_bookstu.type = BOOK_TYPE_DUI;

	if (m_bookstu.type == BOOK_TYPE_MAX)
	{
		MessageBox(L"请选择印张类型");
		return;
	}

	//折页类型
	m_bookstu.zyType = (ZHEYEPAY_TYPE)m_ComboZy.GetCurSel();
	if (m_bookstu.zyType == 0)
	{
		MessageBox(L"请选择折页类型");
		return;
	}
	//入库状态
	m_bookstu.rkType = (BOOK_RK)m_comboRk.GetCurSel();
	//备注
	GetDlgItemText(IDC_EDIT_MSG,m_bookstu.strMsg);

	//生成唯一ID
	CTime time_now;
	time_now = CTime::GetCurrentTime();
	m_bookstu.strBookID = time_now.Format(L"%Y%m%d%H%M%S");

	if(m_bAdd)
		SendToJudgeBook(m_bookstu.strname);
	else
	{
		if (m_ndex>=0)
		{
			m_bookstu.strBookID = m_dlg->m_vet[m_ndex].strBookID;
		}
		SendToAddOrMdfBook(SOCK_CMD_MDF_BOOK);
	}
}


void CAddBookDlg::OnDtnDatetimechangeXdDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CTime time;
	m_date.GetTime(time);
	CString strDate = time.Format(L"%Y/%m/%d");
	m_date.SetFormat(strDate);
	*pResult = 0;
}
