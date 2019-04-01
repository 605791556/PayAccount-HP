// PayAccountDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PayAccount.h"
#include "PayAccountDlg.h"
#include "StaffMngDlg.h"
#include "BookMngDlg.h"
#include "SetPayDlg.h"
#include "LockDlg.h"
#include "MdfPwdDlg.h"
#include "DetailDlg.h"
#include "DayCheckDlg.h"
#include "MonthCheckDlg.h"
#include "UserMngDlg.h"
#include "ProjectMngDlg.h"
#include "ProcessDlg.h"
#include "StaffWriteDlg.h"
#include "WorkCalDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
CPayAccountDlg* g_PayAccountDlg=NULL;
extern HWND g_PayAcountDLgHwnd;
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPayAccountDlg �Ի���




CPayAccountDlg::CPayAccountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPayAccountDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pStaffWriteDlg = NULL;
	m_ProcessDlg = NULL;
	m_DetailDlg = NULL;
	m_DPayDlg = NULL;
	m_MPayDlg = NULL;
	m_WorkCalDlg = NULL;
	m_CloseType = Type_Close;
	m_netState = NET_STATE_OK;
	m_bExit = FALSE;
}

void CPayAccountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
	DDX_Control(pDX, IDC_TOP, m_top);
	DDX_Control(pDX, IDC_STA_NET, m_staNet);
	DDX_Control(pDX, IDC_STA_NET1, m_sta_net1);
}

BEGIN_MESSAGE_MAP(CPayAccountDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_STAFF, &CPayAccountDlg::OnStaff)
	ON_COMMAND(IDM_BOOK, &CPayAccountDlg::OnBook)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CPayAccountDlg::OnTcnSelchangeTab1)
	ON_MESSAGE(WM_INITDLG, &CPayAccountDlg::OnInitDlg)
	ON_MESSAGE(WM_VDATAUPDATE, &CPayAccountDlg::OnBookUpdate)
	ON_MESSAGE(WM_NET_STATE, &CPayAccountDlg::OnNetState)
	ON_MESSAGE(WM_MAINCALL, &CPayAccountDlg::OnCallBack)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPayAccountDlg ��Ϣ�������

BOOL CPayAccountDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	g_PayAccountDlg = this;
	g_PayAcountDLgHwnd = GetSafeHwnd();
	InitMenu();

	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0);
	MoveWindow(0,0,rt.right,rt.bottom,TRUE);
	

	CString strTop;
	strTop.Format(L"��ӭ��¼��%s��%s��\r\n%s  ", g_Globle.m_strUserName, TypeName[g_Globle.m_nType],g_Globle.m_strName);
	SetDlgItemText(IDC_TOP, strTop);

	PostMessage(WM_INITDLG);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPayAccountDlg::InitMenu()
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);  //IDR_MENU1Ϊ�˵���ID��
	if (g_Globle.m_nType == TYPE_COMMON)
	{
		menu.EnableMenuItem(IDM_STAFF, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem(IDM_BOOK, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem(ID_PROJECT_MNG, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem(ID_SETPAY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem(ID_INIT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem(ID_USER, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.EnableMenuItem(ID_MDFPWD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	SetMenu(&menu);
}

void PayAccountCallback(void* p,string strData)
{
	USES_CONVERSION;
	CPayAccountDlg* pThis=(CPayAccountDlg*) p;

	if ( pThis==NULL || !IsWindow(pThis->m_hWnd))
	{
		return;
	}
	else
	{
		string* pStrData = new string;
		*pStrData = strData;
		pThis->PostMessageW(WM_MAINCALL,NULL,(LPARAM)pStrData);
	}
}

LRESULT CPayAccountDlg::OnCallBack(WPARAM wParam, LPARAM lParam)
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
	case SOCK_CMD_INIT:
		{
			if (ret == NET_CMD_FAIL)
				MessageBox(L"��ʼ������ʧ�ܣ�",L"����");
			else
				MessageBox(L"��ʼ���ɹ���",L"");
		}
		break;
	}
	return TRUE;
}

LRESULT CPayAccountDlg::OnInitDlg(WPARAM wParam, LPARAM lParam)
{
	//g_SockClient.createWin();
	InitTabControl();
	return TRUE;
}

LRESULT CPayAccountDlg::OnBookUpdate(WPARAM wParam, LPARAM lParam)
{
	//֪ͨ����ͳ�ƺ��պ��㣬��ϸҳ��ˢ������
	m_ProcessDlg->OnBnClickedBtnUpdate();
	m_DetailDlg->OnBnClickedBtnUpdate();
	return TRUE;
}

LRESULT CPayAccountDlg::OnNetState(WPARAM wParam, LPARAM lParam)
{
	NET_STATE new_state = (NET_STATE)wParam;
	if (new_state != m_netState)
	{
		Invalidate();
	}
	m_netState = new_state;
	if(m_netState == NET_STATE_CLOSE)
		m_staNet.SetWindowTextW(L"�Ͽ�");
	else if(m_netState == NET_STATE_OK)
		m_staNet.SetWindowTextW(L"����");
	return TRUE;
}

void CPayAccountDlg::InitTabControl()
{
	CRect rectThis;
	GetClientRect(&rectThis);

	if (g_Globle.m_nType == TYPE_COMMON)
	{
		m_pStaffWriteDlg = new CStaffWriteDlg;

		m_TabCtrl.InsertItem(0,  L"���˲���");

		m_pStaffWriteDlg->Create(IDD_STAFF_WRITE, &m_TabCtrl);

		CRect rc;
		m_TabCtrl.GetClientRect(rc);

		rc.top = rectThis.top + 30;
		rc.left = rectThis.left + 5;
		rc.right = rectThis.right - 20;
		rc.bottom = rectThis.bottom - 50;
		m_TabCtrl.MoveWindow(rc, TRUE);

		rc.top += 0;
		rc.bottom -= 50;
		rc.left += 10;
		rc.right -= 20;

		m_pStaffWriteDlg->MoveWindow(&rc, TRUE);
		m_TabDlgs[0] = m_pStaffWriteDlg;
		m_TabDlgs[0]->ShowWindow(SW_SHOW);
		m_TabSelIndex = 0;
		m_TabDlgs[0]->UpdateDlg();
	}
	else
	{
		//m_pStaffWriteDlg = new CStaffWriteDlg;
		m_ProcessDlg = new CProcessDlg;
		m_DetailDlg = new CDetailDlg;
		m_WorkCalDlg = new CWorkCalDlg;
		//m_DPayDlg = new CDayCheckDlg;
		m_MPayDlg = new CMonthCheckDlg;

		//m_TabCtrl.InsertItem(0,  L"���˲���");
		m_TabCtrl.InsertItem(0,  L"����");
		m_TabCtrl.InsertItem(1,  L"��ϸ");
		m_TabCtrl.InsertItem(2,  L"����ͳ��");
		m_TabCtrl.InsertItem(3,  L"�º���");

		//m_pStaffWriteDlg->Create(IDD_STAFF_WRITE, &m_TabCtrl);
		m_ProcessDlg->Create(IDD_PROCESS, &m_TabCtrl);
		m_DetailDlg->Create(IDD_DETAIL, &m_TabCtrl);
		m_WorkCalDlg->Create(IDD_WORKCAL, &m_TabCtrl);
		//m_DPayDlg->Create(IDD_DPAY, &m_TabCtrl);
		m_MPayDlg->Create(IDD_MPAY, &m_TabCtrl);

		CRect rc;
		m_TabCtrl.GetClientRect(rc);

		rc.top = rectThis.top + 30;
		rc.left = rectThis.left + 5;
		rc.right = rectThis.right - 20;
		rc.bottom = rectThis.bottom - 30;
		m_TabCtrl.MoveWindow(rc, TRUE);

		rc.top += 0;
		rc.bottom -= 50;
		rc.left += 10;
		rc.right -= 20;

		m_ProcessDlg->MoveWindow(&rc, TRUE);
		m_DetailDlg->MoveWindow(&rc, TRUE);
		m_WorkCalDlg->MoveWindow(&rc, TRUE);
		//m_DPayDlg->MoveWindow(&rc, TRUE);
		m_MPayDlg->MoveWindow(&rc, TRUE);

		m_TabDlgs[0] = m_ProcessDlg;
		m_TabDlgs[1] = m_DetailDlg;
		m_TabDlgs[2] = m_WorkCalDlg;
		//m_TabDlgs[2] = m_DPayDlg;
		m_TabDlgs[3] = m_MPayDlg;

		m_TabDlgs[0]->ShowWindow(SW_SHOW);
		m_TabDlgs[1]->ShowWindow(SW_HIDE);
		m_TabDlgs[2]->ShowWindow(SW_HIDE);
		m_TabDlgs[3]->ShowWindow(SW_HIDE);
		//m_TabDlgs[4]->ShowWindow(SW_HIDE);
		m_TabSelIndex = 0;
		m_TabDlgs[0]->UpdateDlg();
	}
	CRect rect1;
	m_sta_net1.GetClientRect(&rect1);
	int width = rect1.Width();
	int height = rect1.Height();
	rect1.left = rectThis.left+10;
	rect1.top = rectThis.bottom-height-5;
	rect1.right = rect1.left+width;
	rect1.bottom = rect1.top+height;
	m_sta_net1.MoveWindow(rect1, TRUE);

	CRect rect2;
	m_staNet.GetClientRect(&rect2);
	width = rect2.Width();
	height = rect2.Height();
	rect2.left = rect1.right+10;
	rect2.top = rect1.top;
	rect2.right = rect2.left+width;
	rect2.bottom = rect2.top+height;
	m_staNet.MoveWindow(rect2, TRUE);
}

void CPayAccountDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPayAccountDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPayAccountDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//Ա������
void CPayAccountDlg::OnStaff()
{
	CStaffMngDlg dlg;
	dlg.DoModal();
}

//��������
void CPayAccountDlg::OnBook()
{
	// TODO: �ڴ���������������
	CBookMngDlg dlg;
	dlg.DoModal();
}

void CPayAccountDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_TabDlgs[m_TabSelIndex]->ShowWindow(SW_HIDE);
	m_TabSelIndex = m_TabCtrl.GetCurSel();
	m_TabDlgs[m_TabSelIndex]->ShowWindow(SW_SHOW);
	m_TabDlgs[m_TabSelIndex]->UpdateDlg();
	*pResult = 0;
}

BOOL CPayAccountDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO:  �ڴ����ר�ô����/����û���
	UINT uMsg = LOWORD(wParam);
	switch (uMsg)
	{
	case ID_INIT:
		{
			if (g_Globle.m_nType == TYPE_COMMON)
			{
				MessageBox(L"�˺���Ȩ��",  L"��ʾ");
			}
			else if (MessageBox(L"��ʼ��������������������ݣ�ȷ�ϼ����˲�����", TEXT("����"), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
			{
				CLockDlg dlg;
				dlg.SetType(false);
				if (dlg.DoModal() == IDOK)
				{
					//���ûص�
					g_Globle.SetCallback(PayAccountCallback,this);

					Json::Value root;
					root[CONNECT_CMD]=SOCK_CMD_INIT;
					Json::FastWriter writer;  
					string temp = writer.write(root);
					g_Globle.SendTo(temp);
				}
			}
			break;
		}
	case ID_LOCK:
		{
			CLockDlg dlg;
			dlg.DoModal();
			m_TabDlgs[m_TabSelIndex]->UpdateDlg();
			break;
		}
	case ID_LOGOFF://ע��
		{
			g_Globle.m_strUserName = L"";
			g_Globle.m_strUserPwd = L"";
			m_CloseType = Type_LogOff;
			OnCancel();
			break;
		}
	case ID_QUIT:
		{
			if (MessageBox(L"ȷ���˳���", TEXT("��ʾ"), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
				OnCancel();
			break;
		}
	case ID_MDFPWD:
		{
			if (g_Globle.m_nType == TYPE_COMMON)
			{
				MessageBox(L"�˺���Ȩ��",L"��ʾ");
			}
			else
			{
				CMdfPwdDlg dlg;
				dlg.DoModal();
			}
			break;
		}
	case ID_USER://�˺Ź���
		{
			CUserMngDlg dlg;
			dlg.DoModal();
			break;
		}
	case ID_PROJECT_MNG://��Ŀ����
		{
			CProjectMngDlg dlg;
			dlg.DoModal();
			break;
		}
	case ID_SETPAY://��������
		{
			CSetPayDlg dlg;
			dlg.DoModal();					 
			break;
		}
	case ID_LOG:
		{
			USES_CONVERSION;
			ShellExecute(NULL,L"open",A2T(g_Globle.m_LogRootPath.c_str()), NULL, NULL, SW_SHOWNORMAL);
			break;
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}


void CPayAccountDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialog::OnClose();
}


void CPayAccountDlg::OnDestroy()
{
	CDialog::OnDestroy();
	m_bExit = TRUE;
	// TODO:  �ڴ˴������Ϣ����������
	if (m_pStaffWriteDlg)
	{
		delete m_pStaffWriteDlg;
		m_pStaffWriteDlg = NULL;
	}
	if (m_MPayDlg)
	{
		delete m_MPayDlg;
		m_MPayDlg = NULL;
	}
	if (m_DPayDlg)
	{
		delete m_DPayDlg;
		m_DPayDlg = NULL;
	}
	if (m_WorkCalDlg)
	{
		delete m_WorkCalDlg;
		m_WorkCalDlg = NULL;
	}
	if (m_ProcessDlg)
	{
		delete m_ProcessDlg;
		m_ProcessDlg = NULL;
	}
	if (m_DetailDlg)
	{
		delete m_DetailDlg;
		m_DetailDlg = NULL;
	}
}

void CPayAccountDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	CRect rectThis;
	GetClientRect(&rectThis);
	if(m_top)
	{
		CRect rt;
		m_top.GetClientRect(&rt);
		int w = rt.Width();
		rt.right = rectThis.right;
		rt.left = rt.right-w;
		m_top.MoveWindow(rt,TRUE);
	}
}



HBRUSH CPayAccountDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID()==IDC_STA_NET)
	{
		if(m_netState == NET_STATE_CLOSE)
			pDC->SetTextColor(RGB(255, 0, 0));
		else if(m_netState == NET_STATE_OK)
			pDC->SetTextColor(RGB(0, 255, 0));
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
