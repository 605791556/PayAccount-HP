// ProjectMngDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PayAccount.h"
#include "ProjectMngDlg.h"
#include "afxdialogex.h"
#include "PayAccountDlg.h"
#include "AddProDlg.h"

extern CPayAccountDlg* g_PayAccountDlg;

// CProjectMngDlg �Ի���

IMPLEMENT_DYNAMIC(CProjectMngDlg, CDialogEx)

void ProMngCallback(void* p,string strData)
{
	USES_CONVERSION;
	CProjectMngDlg* pThis=(CProjectMngDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_PROMNG_CALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CProjectMngDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
				MessageBox(L"��ȡ��Ŀ��Ϣʧ�ܣ�",L"����");
			else
			{
				GetProject(root);
			}
		}
		break;
	case SOCK_CMD_DEL_PROJECT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"ɾ��ʧ�ܣ�",L"����");
			else
			{
				SendToGetProject();
				MessageBox(L"ɾ���ɹ���",L"��ʾ");
			}
		}
		break;
	case SOCK_CMD_SAVE_PRONDEX:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"��������˳��ʧ�ܣ�",L"����");
			else
			{
				SendToGetProject();
				MessageBox(L"��������˳��ɹ���",L"��ʾ");
			}
		}
		break;
	}
	return TRUE;
}

CProjectMngDlg::CProjectMngDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProjectMngDlg::IDD, pParent)
{

}

CProjectMngDlg::~CProjectMngDlg()
{
}

void CProjectMngDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_EditIndex);
}


BEGIN_MESSAGE_MAP(CProjectMngDlg, CDialogEx)
	ON_COMMAND(ID_MU_DEL, DelProject)
	ON_COMMAND(ID_MU_MDF, MdfProject)
	ON_BN_CLICKED(IDC_BTN_ADD, &CProjectMngDlg::OnBnClickedBtnAdd)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &CProjectMngDlg::OnNMRClickList2)
	ON_MESSAGE(WM_PROMNG_CALL, &CProjectMngDlg::OnCallBack)
	ON_BN_CLICKED(IDC_SAVENDEX, &CProjectMngDlg::OnBnClickedSavendex)
	ON_EN_CHANGE(IDC_EDIT1, &CProjectMngDlg::OnEnChangeEdit1)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, &CProjectMngDlg::OnNMClickList2)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CProjectMngDlg::OnEnKillfocusEdit1)
END_MESSAGE_MAP()


// CProjectMngDlg ��Ϣ�������

BOOL CProjectMngDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_ListCtrl.SetNotifyWnd(this);
	m_ListCtrl.InitListCtrl();
	m_EditIndex.SetParent(&m_ListCtrl);

	//���ûص�
	g_Globle.SetCallback(ProMngCallback,this);
	SendToGetProject();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CProjectMngDlg::SendToGetProject()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_GET_PROJECT;
	root[CMD_GETPRO[EM_GETPRO_BWRITE]]=PRO_STAFF_TYPE_MAX;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"������������ʧ�ܣ����������Ƿ�������",L"��ʾ");
	}
}

void CProjectMngDlg::GetProject(Json::Value root)
{
	m_vet.clear();
	m_ListCtrl.DeleteAllItems();
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
		//��ndex��С��������
		if(m_vet.size() == 0)
			m_vet.push_back(stu);
		else
		{
			bool bInset=false;
			vector <PROJECT_STU>::iterator it;
			for ( it = m_vet.begin( ) ; it != m_vet.end( ) ; it++ )
			{
				if (stu.ndex<(*it).ndex)
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
	SetListValue();
}

void CProjectMngDlg::SendToDelProject(int nProID)
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_DEL_PROJECT;
	root[CMD_DELPROJECT[EM_DEL_PROJECT_ID]]=nProID;
	Json::FastWriter writer;  
	string temp = writer.write(root);
	if(g_Globle.SendTo(temp) != 0)
	{
		MessageBox(L"������������ʧ�ܣ����������Ƿ�������",L"��ʾ");
	}
}

void CProjectMngDlg::SendToSaveProNdex()
{
	USES_CONVERSION;
	Json::Value root;
	root[CONNECT_CMD]=SOCK_CMD_SAVE_PRONDEX;

	int nSize = m_ListCtrl.GetItemCount();
	if (nSize>0)
	{
		for (int i = 0; i < nSize; i++)
		{
			Json::Value one;
			CString strdex=m_ListCtrl.GetItemText(i,4);
			int ndex = _ttoi(strdex);

			PROJECT_STU* stu = (PROJECT_STU*)m_ListCtrl.GetItemData(i);
			stu->ndex = ndex;
			one[CMD_PROMSG[EM_PROMSG_ID]] = stu->nID;
			one[CMD_PROMSG[EM_PROMSG_NDEX]] = stu->ndex;
			root[CMD_RetType[EM_CMD_RETYPE_VALUE]].append(one);
		}

		Json::FastWriter writer;  
		string temp = writer.write(root);
		if(g_Globle.SendTo(temp) != 0)
		{
			MessageBox(L"������������ʧ�ܣ����������Ƿ�������",L"��ʾ");
		}
	}
}

void CProjectMngDlg::SetListValue()
{
	for (int i=0;i<m_vet.size();i++)
	{
		PROJECT_STU stu = m_vet[i];
		CString str;
		str.Format(L"%d",i+1);
		m_ListCtrl.InsertItem(i,str);
		m_ListCtrl.SetItemText(i,1,stu.strName);
		if(stu.pn_type!=PRO_NUM_TYPE_MAX)
			m_ListCtrl.SetItemText(i,2,proNumType[stu.pn_type]);
		if(stu.ps_type!=PRO_STAFF_TYPE_MAX)
		    m_ListCtrl.SetItemText(i,3,proStaffType[stu.ps_type]);

		str.Format(L"%d",stu.ndex);
		m_ListCtrl.SetItemText(i,4,str);
		m_ListCtrl.SetItemData(i,(DWORD_PTR)&m_vet[i]);
	}
}

void CProjectMngDlg::OnBnClickedBtnAdd()
{
	CAddProDlg dlg;
	int ret = dlg.DoModal();
	g_Globle.SetCallback(ProMngCallback,this);
	if (ret==IDOK)
	{
		SendToGetProject();
	}
}


void CProjectMngDlg::OnNMRClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

void CProjectMngDlg::DelProject()
{
	PROJECT_STU stu = *((PROJECT_STU*)m_ListCtrl.GetItemData(m_nItem));
	if (stu.nID>=1 && stu.nID<=8)
	{
		MessageBox(L"�����ֹɾ����",L"��ʾ");
		return;
	}

	CString str;
	str.Format(L"�ò�������ɾ�������Ŀ������صļ�¼�����������ȣ���ϸ�������ȣ���ȷ��ɾ����Ŀ '%s'��",stu.strName);

	if (MessageBox(str,L"��ʾ",MB_OKCANCEL)==IDOK)
	{
		SendToDelProject(stu.nID);
	}
}

void CProjectMngDlg::MdfProject()
{
	CAddProDlg dlg;
	dlg.SetNotifyWnd(this);
	int ret = dlg.DoModal();
	g_Globle.SetCallback(ProMngCallback,this);
	if (ret==IDOK)
	{
		SendToGetProject();
	}
}


void CProjectMngDlg::OnBnClickedSavendex()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendToSaveProNdex();
}


void CProjectMngDlg::OnEnChangeEdit1()
{
	
}


void CProjectMngDlg::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	POINT PT;
	GetCursorPos(&PT);
	m_ListCtrl.ScreenToClient(&PT);
	LVHITTESTINFO hitInfo;
	hitInfo.pt=PT;

	m_ListCtrl.SubItemHitTest(&hitInfo);
	if(hitInfo.flags & LVHT_ONITEMLABEL)//�ж��Ƿ񵥻����ı���
	{
		CRect rect;
		m_ListCtrl.GetSubItemRect(hitInfo.iItem,hitInfo.iSubItem,LVIR_BOUNDS,rect);
		if (hitInfo.iSubItem==4)
		{
			CString mes=m_ListCtrl.GetItemText(hitInfo.iItem,hitInfo.iSubItem);
			// rect.InflateRect(0,0,0,0);//������Ͽ�ĸ߶�ʹ��������������ı�
			m_col=hitInfo.iSubItem;
			m_row=hitInfo.iItem;

			rect.left = rect.left+2;
			rect.top = rect.top+2;
			rect.right = rect.right-2;
			rect.bottom = rect.bottom-2;

			m_EditIndex.MoveWindow(&rect,TRUE);
			m_EditIndex.ShowWindow(SW_NORMAL);
			m_EditIndex.SetWindowText(mes);
			m_EditIndex.BringWindowToTop();
			m_EditIndex.SetFocus();//ʹ��Ͽ�۽�
			m_EditIndex.SetSel(0,-1);
		}
		*pResult = 0;
	}
}

void CProjectMngDlg::SetSubItemValue()
{
	int nSize = m_ListCtrl.GetItemCount();
	if (nSize>0 && m_row<nSize)
	{
		POINT pt;
		GetCursorPos(&pt);
		m_ListCtrl.ScreenToClient(&pt);
		CRect rect;
		m_ListCtrl.GetSubItemRect(m_row,m_col,LVIR_BOUNDS,rect);
		if(!rect.PtInRect(pt))//���������һ���ڵ��ı�������
		{
			CString text;
			m_EditIndex.GetWindowText(text);
			m_ListCtrl.SetItemText(m_row,m_col,text);
			m_EditIndex.ShowWindow(SW_HIDE);//����Ͽ�����
		}
	}
}

void CProjectMngDlg::OnEnKillfocusEdit1()
{
	SetSubItemValue();
}
