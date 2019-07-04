
// PayServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayServer.h"
#include "PayServerDlg.h"
#include "afxdialogex.h"
#include "..\common\common.h"
#include <tlhelp32.h>
#include <shlwapi.h>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"HPSocket_U.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
CPayServerDlg* g_PaySerDlg=NULL;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPayServerDlg 对话框
LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionPointers);
CPayServerDlg::CPayServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPayServerDlg::IDD, pParent), m_Server(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	m_hSetStart = INVALID_HANDLE_VALUE;
	m_bExit = false;
	SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
}

CPayServerDlg::~CPayServerDlg()
{
	
}

void CPayServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_LIST3, m_listCtrl);
	DDX_Control(pDX, IDC_STA_IP, m_staIP);
	DDX_Control(pDX, IDC_CK_STARTRUN, m_ckRun);
	DDX_Control(pDX, IDC_STA, m_NowSta);
}

BEGIN_MESSAGE_MAP(CPayServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SHOWTASK, &CPayServerDlg::OnShowTask)
	ON_COMMAND(ID_S_SHOW, OnShow)
	ON_COMMAND(ID_S_CLOSE, OnClose)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_HIDE, &CPayServerDlg::OnBnClickedBtnHide)
	ON_BN_CLICKED(IDC_CK_STARTRUN, &CPayServerDlg::OnBnClickedCkStartrun)
END_MESSAGE_MAP()

DWORD WINAPI StartSerThread(LPVOID lparam)
{
	USES_CONVERSION;
	CPayServerDlg* pData = (CPayServerDlg*)lparam;
	int bOk = false;
	while (!bOk && !pData->m_bExit)
	{
		bOk = pData->m_Server->Start(A2W(g_Globle.m_LocalIP.c_str()), g_Globle.m_TcpPort);
		if(bOk)
		{
			pData->RunDog();
			::LogServerStart(A2W(g_Globle.m_LocalIP.c_str()), g_Globle.m_TcpPort);
			break;
		}
		else
		{
			::LogServerStartFail(pData->m_Server->GetLastError(), pData->m_Server->GetLastErrorDesc());
			Sleep(2000);
		}
	}
	return 0;
}

LONG WINAPI MyUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionPointers)
{
	SYSTEMTIME t;
	::GetLocalTime(&t);

	CString strDump,strPath;
	int nPos;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	nPos = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Left(nPos);
	strDump.Format(L"%s\\dump\\%02dH%02dM%02dS%02dMS.dmp",strPath,t.wHour,t.wMinute,t.wSecond,t.wMilliseconds);

	USES_CONVERSION;
	HANDLE hFile = CreateFileA(T2A(strDump),GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 
	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = GetCurrentThreadId(); //把需要的信息添进去
	eInfo.ExceptionPointers = pExceptionPointers;
	eInfo.ClientPointers = FALSE;

	// 调用, 生成Dump. 98不支持
	// Dump的类型是小型的, 节省空间. 可以参考MSDN生成更详细的Dump.
	MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MiniDumpNormal,
		&eInfo,
		NULL,
		NULL);
	CloseHandle(hFile);
	return 0;

}
// CPayServerDlg 消息处理程序
BOOL CPayServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	USES_CONVERSION;
	//如果后台有看门狗程序，则先关闭
	KillDog();
	::SetMainWnd(this);
	::SetInfoList(&m_ListBox);
	g_PaySerDlg=this;

	InitListCtrl();
	//初始化socket
	AddString(L"正在初始化网络程序...");
	if (g_Globle.InitGloble())
	{
		CString str;
		str.Format(L"IP：%s    端口：%d",A2W(g_Globle.m_LocalIP.c_str()),g_Globle.m_TcpPort);
		m_staIP.SetWindowTextW(str);

		if(m_Server->Start(A2W(g_Globle.m_LocalIP.c_str()), g_Globle.m_TcpPort))
		{
			RunDog();
			::LogServerStart(A2W(g_Globle.m_LocalIP.c_str()), g_Globle.m_TcpPort);
		}
		else
		{
			::LogServerStartFail(m_Server->GetLastError(), m_Server->GetLastErrorDesc());
			m_hSetStart = CreateThread(NULL,0,StartSerThread,this,0,NULL);
		}
		//开始定时刷新客户端列表
		SetTimer(1,2000,NULL);
	}
	InitChechBox();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPayServerDlg::InitChechBox()
{
	HKEY hKey;
	CString lpRun = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
	long lRet = RegOpenKeyEx(HKEY_CURRENT_USER, lpRun, 0, KEY_ALL_ACCESS, &hKey);
	if (lRet != ERROR_SUCCESS)
		MessageBox(_T("打开启动项失败"));

	TCHAR pFileName[MAX_PATH] = {};
	GetModuleFileName(NULL, pFileName, MAX_PATH);
	//判断是否已经设置开机启动
	TCHAR PowerBoot[MAX_PATH] = {};
	DWORD nLongth = MAX_PATH;
	long result = RegGetValue(hKey, NULL, _T("PayServer"), RRF_RT_REG_SZ, 0, PowerBoot, &nLongth);
	if (result == ERROR_SUCCESS)
	{
		m_ckRun.SetCheck(TRUE);
	}
	RegCloseKey(hKey); 
}

void CPayServerDlg::InitListCtrl()
{
	TCHAR rgtsz[4][10] = {_T("账号"), _T("端口") , _T("名称"),_T("连接时长")};
	LV_COLUMN lvcolumn;
	CRect rect;
	m_listCtrl.GetWindowRect(&rect);
	for (int i=0;i<4;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if(i==0)
			lvcolumn.cx = 100;
		else if(i==1)
			lvcolumn.cx = 70;
		else
			lvcolumn.cx = ((rect.Width()-170)/2) -2;
		m_listCtrl.InsertColumn(i, &lvcolumn);
	}
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrl.SetBkColor(RGB(211 ,211 ,211));
	m_listCtrl.SetTextBkColor(RGB(102 ,205, 170));
}

void CPayServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_MINIMIZE)
	{
		Hide();
		return;
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CPayServerDlg::OnShow()
{
	this->ShowWindow(SW_SHOWNORMAL);
	this->SetForegroundWindow();
	Shell_NotifyIcon(NIM_DELETE, &m_nid); // 托盘图标不显示
}

void CPayServerDlg::OnClose()
{
	PostMessage(WM_CLOSE);
}

void CPayServerDlg::Hide()
{
	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uID = IDR_MAINFRAME;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_SHOWTASK;
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	lstrcpy(m_nid.szTip, _T("payServer"));
	Shell_NotifyIcon(NIM_ADD, &m_nid); 
	ShowWindow(SW_HIDE);
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPayServerDlg::KillDog()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	PROCESSENTRY32 pe32;
	HANDLE hProcess;
	char *kill[]={"HoldDog.exe","HoldDog"};
	static int times=0;

	pe32.dwSize = sizeof( PROCESSENTRY32 );
	Process32First( hProcessSnap, &pe32);

	do
	{
		TCHAR data[512]=TEXT("NULL");
		_tprintf(TEXT("ID: %5d\tname: %s\n"),pe32.th32ProcessID,pe32.szExeFile);

		{
			char ch[512]={0};
			WideCharToMultiByte( CP_ACP, 0, pe32.szExeFile, wcslen(pe32.szExeFile),ch,512 , NULL, NULL );
			int k=sizeof(kill)/sizeof(char*);
			for(int i=0;i<sizeof(kill)/sizeof(char*);i++)
			{
				if(strstr(ch,kill[i]))
				{
					hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, FALSE, pe32.th32ProcessID );
					TerminateProcess( hProcess, 0 );
				}
			}
		}
	} while( Process32Next( hProcessSnap, &pe32 ) );
	CloseHandle (hProcessSnap); 
}

BOOL CPayServerDlg::RunDog()
{
	CString str,strPath;
	int nPos;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	nPos = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Left(nPos);


	CString dogPathName = strPath+L"\\HoldDog.exe";
	if (!PathFileExists(dogPathName))
	{
		str.Format(L"看门狗程序启动失败：%s",dogPathName);
		AddString(str);
		return FALSE;
	}
	else
	{
		ShellExecute(NULL,L"open",dogPathName,NULL,strPath,SW_SHOWNORMAL);
		AddString(L"看门狗程序启动成功");
	}
	return TRUE;
}

void CPayServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPayServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CPayServerDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDR_MAINFRAME)
		return 1;
	if (WM_LBUTTONDBLCLK == lParam)
	{
		this->ShowWindow(SW_SHOWNORMAL);
		this->SetForegroundWindow();
		Shell_NotifyIcon(NIM_DELETE, &m_nid); // 托盘图标不显示
	}
	if (lParam == WM_RBUTTONDOWN)
	{
		//右击弹出托盘菜单
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu *pPopUp = menu.GetSubMenu(0);
		CPoint pt;
		GetCursorPos(&pt);
		SetForegroundWindow();
		pPopUp->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
		PostMessage(WM_NULL, 0, 0);
	}
	return 0;
}

void CPayServerDlg::AddString(CString str)
{

	CString str2,strTime;
	SYSTEMTIME st;
	GetLocalTime(&st);
	strTime.Format(L"%4d/%02d/%02d %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	str2.Format(L"%s  %s",strTime, str);
	m_ListBox.AddString(str2);
}

void CPayServerDlg::DoRun(string strData,Json::Value& js,TPkgInfo* pInfo)
{
	try
	{
		bool bRet = false;
		USES_CONVERSION;
		Json::Reader r;
		Json::Value root;
		r.parse(strData,root);

		Json::FastWriter writer;  
		string temp = writer.write(root);

		EM_SOCK_CMD cmd = (EM_SOCK_CMD)root[CONNECT_CMD].asInt();
		js[CONNECT_CMD]=cmd;
		switch (cmd)
		{
		case SOCK_CMD_HEART:
			{
			}
			break;
		case SOCK_CMD_GET_USER:
			{
				bRet=theApp.m_dbData->GetUsers(js);
			}
			break;
		case SOCK_CMD_JUDGE_USER:
			{
				CString strName = A2T(root[CMD_JUDGEUSER[EM_JUDGE_USER_NAME]].asCString());
				bRet=theApp.m_dbData->_JudgeUser(strName,js);
			}
			break;
		case SOCK_CMD_ADD_USER:
			{
				CString strName = A2T(root[CMD_ADDUSER[EM_ADD_USER_NAME]].asCString());
				CString strPwd = A2T(root[CMD_ADDUSER[EM_ADD_USER_PWD]].asCString());
				USER_TYPE type = (USER_TYPE)root[CMD_ADDUSER[EM_ADD_USER_TYPE]].asInt();
				bRet=theApp.m_dbData->AddUser(strName,strPwd,type);
			}
			break;
		case SOCK_CMD_DEL_USER:
			{
				CString strName = A2T(root[CMD_DELUSER[EM_JUDGE_USER_NAME]].asCString());
				bRet=theApp.m_dbData->DelUser(strName);
			}
			break;
		case SOCK_CMD_INIT:
			{
				bRet=theApp.m_dbData->InitData();
			}
			break;
		case SOCK_CMD_CHECKPWD:
			{
				CString strName = A2T(root[CMD_CHECKPWD[EM_CHECKPWD_NAME]].asCString());
				CString strPass = A2T(root[CMD_CHECKPWD[EM_CHECKPWD_PWD]].asCString());
				bRet=theApp.m_dbData->CheckPwd(strName,strPass);
			}
			break;
		case SOCK_CMD_MDFPWD:
			{
				CString strName = A2T(root[CMD_MDFPWD[EM_MDFPWD_NAME]].asCString());
				CString strPass = A2T(root[CMD_MDFPWD[EM_MDFPWD_PWD]].asCString());
				bRet=theApp.m_dbData->ModifyPwd(strName,strPass);
			}
			break;
		case SOCK_CMD_LOGIN:
			{
				CString strUser = A2T(root[CMD_LOGIN[EM_LOGIN_NAME]].asCString());
				CString strPass = A2T(root[CMD_LOGIN[EM_LOGIN_PASS]].asCString());
				int show_pass = root[CMD_LOGIN[EM_LOGIN_SHOWPASS]].asInt();
				//判断该账号是否已经登录
				bool bLogind = false;
				DWORD nCount = m_Server->GetConnectionCount();
				CONNID* pIDs = new CONNID[nCount];
				if(m_Server->GetAllConnectionIDs(pIDs,nCount))
				{
					for (int i=0;i<nCount;i++)
					{
						TPkgInfo* pInfo = FindPkgInfo(m_Server,pIDs[i]);
						CString strUserd = A2T(pInfo->user.c_str());
						if (strUserd == strUser)
						{
							//已经登录
							bLogind = true;
							js[CMD_LOGINMSG[EM_LOGINMSG_BOK]] = false;
							js[CMD_LOGINMSG[EM_LOGINMSG_LOGIND]] = bLogind;
							bRet = true;
							break;
						}
					}
				}
				delete[] pIDs;
				if(!bLogind)
				{
					if (pInfo)
					{
						WaitForSingleObject(m_mutex, INFINITE); 
						pInfo->user = T2A(strUser);
						ReleaseMutex(m_mutex);
					}
					bRet=theApp.m_dbData->Login(strUser,strPass,show_pass,js);
				}
			}
			break;
		case SOCK_CMD_GET_BOOK:
			{
				CString strKeyWord = A2T(root[CMD_GETBOOK[GETBOOK_KEYWORD]].asCString());
				BOOK_RK em_rk = (BOOK_RK)root[CMD_GETBOOK[GETBOOK_RKTYPE]].asInt();
				EM_DATE_TYPE em_date = (EM_DATE_TYPE)root[CMD_GETBOOK[GETBOOK_DATE]].asInt();
				int n = root[CMD_GETBOOK[GETBOOK_NSTART]].asInt();
				int number = root[CMD_GETBOOK[GETBOOK_NUMBER]].asInt();
				bRet=theApp.m_dbData->GetBooks(js,strKeyWord,em_rk,em_date,n,number);
			}
			break;
		case SOCK_CMD_GET_SAMPLE_BOOK:
			{
				BOOK_RK rkType = (BOOK_RK)root[CMD_GETBOOK[GETBOOK_RKTYPE]].asInt();
				bRet=theApp.m_dbData->GetSampleBooks(js,rkType);
			}
			break;
		case SOCK_CMD_DEL_BOOK:
			{
				CString strBookID = A2T(root[CMD_DELBOOK[EM_DEL_BOOK_ID]].asCString());
				bRet=theApp.m_dbData->DelBook(strBookID);
			}
			break;
		case SOCK_CMD_JUDGE_BOOK:
			{
				CString strName = A2T(root[CMD_JUDGEBOOK[EM_JUDGE_BOOK_NAME]].asCString());
				bRet=theApp.m_dbData->_JudgeBook(strName,js);
			}
			break;
		case SOCK_CMD_ADD_BOOK:
		case SOCK_CMD_MDF_BOOK:
			{
				CString strBookID = A2T(root[CMD_BOOKMSG[BOOKMSG_ID]].asCString());
				CString strName = A2T(root[CMD_BOOKMSG[BOOKMSG_NAME]].asCString());
				CString strCbs = A2T(root[CMD_BOOKMSG[BOOKMSG_CBS]].asCString());
				CString strDate = A2T(root[CMD_BOOKMSG[BOOKMSG_DATE]].asCString());
				int nBc1 = root[CMD_BOOKMSG[BOOKMSG_BC1]].asInt();
				int nBc2 = root[CMD_BOOKMSG[BOOKMSG_BC2]].asInt();
				int nSize1 = root[CMD_BOOKMSG[BOOKMSG_SIZE1]].asInt();
				int nSize2 = root[CMD_BOOKMSG[BOOKMSG_SIZE2]].asInt();
				int nKb = root[CMD_BOOKMSG[BOOKMSG_KB]].asInt();
				double fYz = root[CMD_BOOKMSG[BOOKMSG_YZ]].asDouble();
				int nYs = root[CMD_BOOKMSG[BOOKMSG_YS]].asInt();
				int nBc = root[CMD_BOOKMSG[BOOKMSG_BC]].asInt();
				double fLs = root[CMD_BOOKMSG[BOOKMSG_LS]].asDouble();
				int yzType = root[CMD_BOOKMSG[BOOKMSG_B_TYPE]].asInt();
				int zyType = root[CMD_BOOKMSG[BOOKMSG_ZY_TYPE]].asInt();
				int rkType = root[CMD_BOOKMSG[BOOKMSG_RK_TYPE]].asInt();
				CString strMsg = A2T(root[CMD_BOOKMSG[BOOKMSG_MSG]].asCString());

				if(cmd == SOCK_CMD_ADD_BOOK)
					bRet=theApp.m_dbData->AddBook(strBookID, strName,strCbs,strDate,nBc1,nBc2, nSize1,nSize2,nKb, fYz,nYs,nBc,fLs,yzType,zyType,rkType,strMsg);
				else if(cmd == SOCK_CMD_MDF_BOOK)
					bRet=theApp.m_dbData->ModifyBook(strBookID, strName,strCbs,strDate,nBc1,nBc2, nSize1,nSize2,nKb, fYz,nYs,nBc,fLs,yzType,zyType,rkType,strMsg);
			}
			break;
		case SOCK_CMD_GET_PROJECT:
			{
				PRO_STAFF_TYPE type = (PRO_STAFF_TYPE)root[CMD_GETPRO[EM_GETPRO_BWRITE]].asInt();
				bRet=theApp.m_dbData->GetProjectLists(js,type);
			}
			break;
		case SOCK_CMD_DEL_PROJECT:
			{
				int nProID = root[CMD_DELPROJECT[EM_DEL_PROJECT_ID]].asInt();
				bRet=theApp.m_dbData->DelOneProject(nProID);
			}
			break;
		case SOCK_CMD_JUDGE_PROJECT:
			{
				CString strProName = A2T(root[CMD_JUDGEBOOK[EM_JUDGE_PROJECT_NAME]].asCString());
				bRet=theApp.m_dbData->_JudgePro(strProName,js);
			}
			break;
		case SOCK_CMD_ADD_PROJECT:
		case SOCK_CMD_MDF_PROJECT:
			{
				CString strName = A2T(root[CMD_PROMSG[EM_PROMSG_NAME]].asCString());
				PRO_NUM_TYPE numType = (PRO_NUM_TYPE)root[CMD_PROMSG[EM_PROMSG_NUM_TYPE]].asInt();
				PRO_STAFF_TYPE bWrite = (PRO_STAFF_TYPE)root[CMD_PROMSG[EM_PROMSG_BWRITE]].asInt();
				if(cmd == SOCK_CMD_ADD_PROJECT)
					bRet=theApp.m_dbData->AddProject(strName,numType,bWrite);
				else if(cmd == SOCK_CMD_MDF_PROJECT)
				{
					int nID = root[CMD_PROMSG[EM_PROMSG_ID]].asInt();
					bRet=theApp.m_dbData->ModifyProject(nID,strName,numType,bWrite);
				}
			}
			break;
		case SOCK_CMD_SAVE_PRONDEX:
			{
				bRet=theApp.m_dbData->SaveProNdex(root);
			}
			break;
		case SOCK_CMD_GET_PROGRESS:
			{
				CString strBookID = A2T(root[CMD_GETPROGRESS[EM_GETPROGRESS_BOOKID]].asCString());
				bRet=theApp.m_dbData->_GetProgress(js,root,strBookID);
			}
			break;
		case SOCK_CMD_GET_STAFFWRITE:
			{
				CString strBookID = A2T(root[CMD_GETPROGRESS[EM_GETPROGRESS_BOOKID]].asCString());
				int nProID = root[CMD_GETPROGRESS[EM_GETPROGRESS_PROID]].asInt();
				bRet=theApp.m_dbData->GetStaffWrite(js,strBookID,nProID);
			}
			break;
		case SOCK_CMD_SAVE_STAFFWRITE:
			{
				CString strBookID = A2T(root[CMD_GETPROGRESS[EM_GETPROGRESS_BOOKID]].asCString());
				int nProID = root[CMD_GETPROGRESS[EM_GETPROGRESS_PROID]].asInt();
				double number = root[CMD_RetType[EM_CMD_RETYPE_VALUE]].asDouble();
				bRet=theApp.m_dbData->SaveStaffWrite(strBookID,nProID,number);
			}
			break;
		case SOCK_CMD_GET_STAFF:
			{
				CString strKeyWord = A2T(root[CMD_GETSTAFF[EM_GET_STAFF_KEYWORD]].asCString());
				int n = root[CMD_GETSTAFF[EM_GET_STAFF_NSTART]].asInt();
				int number = root[CMD_GETSTAFF[EM_GET_STAFF_NUMBER]].asInt();
				bRet=theApp.m_dbData->GetStaffs(strKeyWord,js,n,number);
			}
			break;
		case SOCK_CMD_GET_SAMPLE_STAFF:
			{
				bRet=theApp.m_dbData->GetSampleStaffs(js);
			}
			break;
		case SOCK_CMD_DEL_STAFF:
			{
				CString strStaffID = A2T(root[CMD_DELSTAFF[EM_DEL_STAFF_ID]].asCString());
				bRet=theApp.m_dbData->DelStaff(strStaffID);
			}
			break;
		case SOCK_CMD_GET_DPAY:
			{
				CString strStaffID = A2T(root[CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]].asCString());
				bRet=theApp.m_dbData->GetDPay(strStaffID,js);
			}
			break;
		case SOCK_CMD_JUDGE_STAFF:
			{
				CString strIdcard = A2T(root[CMD_JUDGESTAFF[EM_JUDGE_STAFF_IDCARD]].asCString());
				bRet=theApp.m_dbData->_JudgeStaff(strIdcard,js);
			}
			break;
		case SOCK_CMD_ADD_STAFF:
		case SOCK_CMD_MDF_STAFF:
			{
				CString strName = A2T(root[CMD_STAFFMSG[EM_STAFF_MSG_NAME]].asCString());
				CString strSex = A2T(root[CMD_STAFFMSG[EM_STAFF_MSG_SEX]].asCString());
				int age = root[CMD_STAFFMSG[EM_STAFF_MSG_AGE]].asInt();
				CString strStaffID = A2T(root[CMD_STAFFMSG[EM_STAFF_MSG_STAFFID]].asCString());
				CString strIdCard = A2T(root[CMD_STAFFMSG[EM_STAFF_MSG_IDCARD]].asCString());
				CString strTel = A2T(root[CMD_STAFFMSG[EM_STAFF_MSG_TEL]].asCString());
				STAFF_TYPE type = (STAFF_TYPE)root[CMD_STAFFMSG[EM_STAFF_MSG_TYPE]].asInt();
				int sort = root[CMD_STAFFMSG[EM_STAFF_MSG_SORT]].asInt();
				double fDaypay = root[CMD_STAFFMSG[EM_STAFF_MSG_DAYPAY]].asDouble();
				if(cmd == SOCK_CMD_ADD_STAFF)
					bRet=theApp.m_dbData->AddStaff(strName,strSex,age,strStaffID,strIdCard,strTel,type,sort,fDaypay);
				else if(cmd == SOCK_CMD_MDF_STAFF)
					bRet=theApp.m_dbData->ModifyStaff(strName,strSex,age,strStaffID,strIdCard,strTel,type,sort,fDaypay);
			}
			break;
		case SOCK_CMD_GET_DAIPAY:
			{
				bRet=theApp.m_dbData->_GetDaiPay(js);
			}
			break;
		case SOCK_CMD_SET_DAIPAY:
			{
				STU_DAI_PAY stu;
				stu.strA_w = A2T(root[CMD_DAIPAY[EM_DAI_PAY_A_W]].asCString());
				stu.strSf_w = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Sf_w]].asCString());
				stu.strTd_w = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Td_w]].asCString());

				stu.strA_2 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_2]].asCString());
				stu.strSf_2 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Sf_2]].asCString());
				stu.strTd_2 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Td_2]].asCString());

				stu.strA_2_5 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_2_5]].asCString());
				stu.strSf_2_5 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Sf_2_5]].asCString());
				stu.strTd_2_5 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Td_2_5]].asCString());

				stu.strA_5_9 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_5_9]].asCString());
				stu.strSf_5_9 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Sf_5_9]].asCString());
				stu.strTd_5_9 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Td_5_9]].asCString());

				stu.strA_10 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_10]].asCString());
				stu.strSf_10 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Sf_10]].asCString());
				stu.strTd_10 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Td_10]].asCString());

				stu.strA_18 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_18]].asCString());
				stu.strSf_18 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Sf_18]].asCString());
				stu.strTd_18 = A2T(root[CMD_DAIPAY[EM_DAI_PAY_Td_18]].asCString());

				bRet=theApp.m_dbData->SaveDaiPay(stu);
			}
			break;
		case SOCK_CMD_SET_ZHEYEPAY:
			{
				STU_ZHEYE_PAY stu;
				stu.strAQ4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_AQ4]].asCString();
				stu.strA4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A4]].asCString();
				stu.strAD3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_AD3]].asCString();
				stu.strA3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A3]].asCString();
				stu.strA2 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_A2]].asCString();

				stu.str_sf_Q4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_Q4]].asCString();
				stu.str_sf_4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_4]].asCString();
				stu.str_sf_D3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_D3]].asCString();
				stu.str_sf_3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_3]].asCString();
				stu.str_sf_2 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_sf_2]].asCString();

				stu.str_td_Q4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_Q4]].asCString();
				stu.str_td_4 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_4]].asCString();
				stu.str_td_D3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_D3]].asCString();
				stu.str_td_3 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_3]].asCString();
				stu.str_td_2 = root[CMD_ZHEYEPAY[EM_ZHEYE_PAY_td_2]].asCString();
				bRet=theApp.m_dbData->SaveZheYePay(stu);
			}
			break;
		case SOCK_CMD_GET_ZHEYEPAY:
			{
				bRet=theApp.m_dbData->_GetZheYePay(js);
			}
			break;
		case SOCK_CMD_GET_DYPAY:
			{
				bRet=theApp.m_dbData->_GetDyPay(js);
			}
			break;
		case SOCK_CMD_SET_DYPAY:
			{
				CString strdown,strup;
				strdown = root[CMD_DYPAY[EM_DY_PAY_DOWN]].asCString();
				strup = root[CMD_DYPAY[EM_DY_PAY_UP]].asCString();
				bRet=theApp.m_dbData->SaveDianyePay(strdown,strup);
			}
			break;
		case SOCK_CMD_GET_OTHERPAY:
			{
				int nProID = root[CMD_OTHERPAY[EM_OTHER_PAY_PROID]].asInt();
				bRet=theApp.m_dbData->_GetOtherPay(js,nProID);
			}
			break;
		case SOCK_CMD_SET_OTHERPAY:
			{
				vector<OTHER_PRO_PAY> vec;
				int nProID = root[CMD_OTHERPAY[EM_OTHER_PAY_PROID]].asInt();
				if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
				{
					if (root[CMD_RetType[EM_CMD_RETYPE_VALUE]].isArray())
					{
						Json::Value vle = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
						for (int i=0;i<vle.size();i++)
						{
							OTHER_PRO_PAY pay;
							pay.strBookID=vle[i][CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_BOOKID]].asCString();
							pay.strPay=vle[i][CMD_OTHERPAYMSG[EM_OTHER_PAY_MSG_PAY]].asCString();
							vec.push_back(pay);
						}
					}
				}
				bRet = theApp.m_dbData->SaveOtherPay(nProID,vec);
			}
			break;
		case SOCK_CMD_SET_OTHERALLBOOKPAY:
			{
				CString strPay;
				int nProID = root[CMD_PROMSG[EM_PROMSG_ID]].asInt();
				strPay = root[CMD_PROMSG[EM_PROMSG_PAY]].asCString();
				bRet=theApp.m_dbData->SaveOtherAllBookPay(nProID,strPay);
			}
			break;
		case SOCK_CMD_GET_DETAILS:
			{
				CString strBookID;
				vector<PROJECT_STU> vec;
				strBookID = root[CMD_GETDETAILS[EM_GET_DETAILS_BOOKID]].asCString();
				if (root.isMember(CMD_GETDETAILS[EM_GET_DETAILS_PROIDS]))
				{
					if (root[CMD_GETDETAILS[EM_GET_DETAILS_PROIDS]].isArray())
					{
						Json::Value vle = root[CMD_GETDETAILS[EM_GET_DETAILS_PROIDS]];
						for (int i=0;i<vle.size();i++)
						{
							PROJECT_STU stu;
							stu.nID=vle[i][CMD_GETDETAILS[EM_GET_DETAILS_PROID]].asInt();
							stu.ndex=vle[i][CMD_GETDETAILS[EM_GET_DETAILS_NDEX]].asInt();
							vec.push_back(stu);
						}
					}
				}
				bRet=theApp.m_dbData->_GetDetails(js,vec,strBookID);
			}
			break;
		case SOCK_CMD_GET_DAYPAY:
			{
				CString strStaffID,strDate;
				strStaffID = root[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]].asCString();
				strDate = root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]].asCString();
				bRet=theApp.m_dbData->_GetDayPay(js,strStaffID,strDate);
			}
			break;
		case SOCK_CMD_GET_DAYPAY_LIST:
			{
				bRet=theApp.m_dbData->_GetDayPayList(js,root);
			}
			break;
		case SOCK_CMD_GET_MPAY:
			{
				DWORD time=0;
				bRet=theApp.m_dbData->_GetMouthPay2(js,root,time);
			}
			break;
		case SOCK_CMD_DEL_DAYPAY:
			{
				CString strStaffID,strDate;
				strStaffID = root[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]].asCString();
				strDate = root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]].asCString();
				bRet=theApp.m_dbData->DeleteDayPay(strStaffID,strDate);
			}
			break;
		case SOCK_CMD_SAVE_DAYPAY:
			{
				CString strStaffID,strDate;
				strStaffID = root[CMD_GETDAYPAY[EM_GET_DAYPAY_STAFFID]].asCString();
				strDate = root[CMD_GETDAYPAY[EM_GET_DAYPAY_DATE]].asCString();
				vector<DAYPAY> vec;
				if (root.isMember(CMD_RetType[EM_CMD_RETYPE_VALUE]))
				{
					if (root[CMD_RetType[EM_CMD_RETYPE_VALUE]].isArray())
					{
						Json::Value vle = root[CMD_RetType[EM_CMD_RETYPE_VALUE]];
						for (int i=0;i<vle.size();i++)
						{
							DAYPAY stu;
							stu.type=(DAYPAY_TYPE)vle[i][DAYPAYMSG[EM_DAYPAY_MSG_TYPE]].asInt();
							if (stu.type == DAYPAY_TYPE_DEL)
							{
								stu.strMsg = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_DELMSG]].asCString();
							}
							else if (stu.type == DAYPAY_TYPE_DAY)
							{
								stu.strPayDay = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_PAYDAY]].asCString();
								stu.strDays = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_DAYS]].asCString();
							}
							else if (stu.type == DAYPAY_TYPE_JIJIAN)
							{
								stu.proID = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_PROID]].asInt();
								stu.strBookID = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_BOOKID]].asCString();
								stu.pay = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_PAY]].asCString();
								stu.number = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_NUMBER]].asDouble();
								stu.strProName = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_PRONAME]].asCString();
								stu.strBookName = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_BOOKNAME]].asCString();
							}
							stu.money = vle[i][DAYPAYMSG[EM_DAYPAY_MSG_MONEY]].asCString();
							vec.push_back(stu);
						}
						bRet=theApp.m_dbData->AddDayPay(strStaffID,vec,strDate);
					}
				}
			}
			break;
		case SOCK_CMD_GET_PAY:
			{
				int nProID,nItem;
				CString strStaffID,strBookID;
				strStaffID = root[GETPAYMSG[EM_GET_PAY_STAFFID]].asCString();
				strBookID = root[GETPAYMSG[EM_GET_PAY_BOOKID]].asCString();
				nProID = root[GETPAYMSG[EM_GET_PAY_PROID]].asInt();
				nItem = root[GETPAYMSG[EM_GET_PAY_NITEM]].asInt();
				bRet=theApp.m_dbData->_GetOnePay(js,strStaffID,nProID,strBookID);
				js[GETPAYMSG[EM_GET_PAY_NITEM]] = nItem;
			}
			break;
		default:
			{
			}
		}
		if (bRet)
			js[CMD_RetType[EM_CMD_RETYPE_RESULT]]=NET_CMD_SUC;
		else
			js[CMD_RetType[EM_CMD_RETYPE_RESULT]]=NET_CMD_FAIL;
	}
	catch (...)
	{
		js[CMD_RetType[EM_CMD_RETYPE_RESULT]]=NET_CMD_FAIL;
	}

}

void CPayServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	m_bExit = TRUE;
	if (m_hSetStart != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_hSetStart,INFINITE);
		m_hSetStart = INVALID_HANDLE_VALUE;
	}
	KillTimer(1);
	CloseHandle(m_mutex);
	m_Server->Stop();
	KillDog();
}


void CPayServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString str;
	USES_CONVERSION;
	m_listCtrl.DeleteAllItems();
	WaitForSingleObject(m_mutex, INFINITE); 
	DWORD nCount = m_Server->GetConnectionCount();
	if (nCount > 0)
	{
		CONNID* pIDs = new CONNID[nCount];
		BOOL bRet = m_Server->GetAllConnectionIDs(pIDs,nCount);
		if( bRet)
		{
			for (int i=0;i<nCount;i++)
			{
				TPkgInfo* pInfo = FindPkgInfo(m_Server,pIDs[i]);
				if (pInfo)
				{
					CString strUser = A2T(pInfo->user.c_str());
					m_listCtrl.InsertItem(i,strUser);
					str.Format(L"%d",pInfo->nPort);
					m_listCtrl.SetItemText(i,1,str);
					if (pInfo->em_LinkType == LINK_TYPE_DOG)
					{
						m_listCtrl.SetItemText(i,2,L"HoldDog");
					}	
					DWORD ms_time=0;
					m_Server->GetConnectPeriod(pIDs[i],ms_time);
					int all_sec = ms_time/1000;
					int sec = all_sec%60;
					int all_min = all_sec/60;
					int min = all_min%60;
					int all_h = all_min/60;
					int day = all_h/24;
					int h = all_h%24;
					str.Format(L"%d天 %02d:%02d:%02d",day,h,min,sec);
					m_listCtrl.SetItemText(i,3,str);
				}
			}
		}
		delete[] pIDs;
	}
    str.Format(L"当前连接数：%d",nCount);
	m_NowSta.SetWindowTextW(str);

	ReleaseMutex(m_mutex); 
	CDialogEx::OnTimer(nIDEvent);
}


void CPayServerDlg::OnBnClickedBtnHide()
{
	Hide();
}

//开机启动
void CPayServerDlg::OnBnClickedCkStartrun()
{
	HKEY hKey;
	CString strRegPath = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	//打开启动项
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strRegPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) 
	{
		if (m_ckRun.GetCheck())
		{
			TCHAR szModule[MAX_PATH];
			GetModuleFileName(NULL, szModule, MAX_PATH);//得到本程序自身的全路径  
			//添加一个子Key,并设置值 
			RegSetValueEx(hKey, _T("PayServer"), 0, REG_SZ, (LPBYTE)szModule, (lstrlen(szModule) + 1)*sizeof(TCHAR));
		}
		else
		{
			RegDeleteValue(hKey, _T("PayServer"));
		}
		RegCloseKey(hKey); 
	}
	else
	{
		AfxMessageBox(_T("打开启动项失败"));
	}
}

int CPayServerDlg::SendTo(CONNID clientID, Json::Value js)
{
	Json::FastWriter writer;  
	string strData = writer.write(js);

	static DWORD SEQ = 0;//流水号
	LPCSTR desc = strData.c_str();
	int desc_len = (int)strlen(desc) + 1;
	int body_len = offsetof(TPkgBody, desc) + desc_len;

	DWORD all_len = m_Server->GetSocketBufferSize();
	TPkgHeader header;
	header.seq = ++SEQ;
	header.body_len = body_len;

	TPkgBody* pBody = (TPkgBody*)_alloca(body_len);
	memset(pBody, 0, body_len);
	strcpy(pBody->desc, desc);

	WSABUF bufs[2];
	bufs[0].len = sizeof(TPkgHeader);
	bufs[0].buf = (char*)&header;
	bufs[1].len = body_len;
	bufs[1].buf = (char*)pBody;

	if(!m_Server->SendPackets(clientID,bufs, 2))
	{
		CString str;
		str.Format(L"send error,conid:%d, err:%d",clientID,::SYS_GetLastError());
		AddString(str);	
	}
	return 0;
}

TPkgInfo* CPayServerDlg::FindPkgInfo(ITcpServer* pSender, CONNID dwConnID)
{
	PVOID pInfo = nullptr;

	pSender->GetConnectionExtra(dwConnID, &pInfo);

	return (TPkgInfo*)pInfo;
}

void CPayServerDlg::RemovePkgInfo(ITcpServer* pSender, CONNID dwConnID)
{
	TPkgInfo* pInfo = FindPkgInfo(pSender, dwConnID);
	ASSERT(pInfo != nullptr);
	delete pInfo;
}

EnHandleResult CPayServerDlg::OnPrepareListen(ITcpServer* pSender, SOCKET soListen)
{
	TCHAR szAddress[40];
	int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
	USHORT usPort;

	pSender->GetListenAddress(szAddress, iAddressLen, usPort);
	//::PostOnPrepareListen(szAddress, usPort);
	return HR_OK;
}

EnHandleResult CPayServerDlg::OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient)
{
	try
	{
		BOOL bPass = TRUE;
		TCHAR szAddress[40];
		int iAddressLen = sizeof(szAddress) / sizeof(TCHAR);
		USHORT usPort;

		pSender->GetRemoteAddress(dwConnID, szAddress, iAddressLen, usPort);

		if(bPass)
		{
			TPkgInfo* pInfo = new TPkgInfo(true, sizeof(TPkgHeader));
			pInfo->nPort = usPort;
			pSender->SetConnectionExtra(dwConnID, pInfo);
			CString str;
			str.Format(L"%d:%s 连入",dwConnID,szAddress);
			AddString(str);
		}
		return bPass ? HR_OK : HR_ERROR;
	}
	catch (...)
	{
		return HR_ERROR;
	}
}

EnHandleResult CPayServerDlg::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	return HR_OK;
}

EnHandleResult CPayServerDlg::OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength)
{
	try
	{
		TPkgInfo* pInfo			= FindPkgInfo(pSender, dwConnID);
		ITcpPullServer* pServer	= ITcpPullServer::FromS(pSender);

		if(pInfo != nullptr && pServer != nullptr)
		{
			int required = pInfo->length;
			int remain = iLength;

			while(remain >= required)
			{
				remain -= required;
				CBufferPtr buffer(required);

				EnFetchResult result = pServer->Fetch(dwConnID, buffer, (int)buffer.Size());
				if(result == FR_OK)
				{
					if(pInfo->is_header)
					{
						TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
						if (pHeader->em_LinkType != LINK_TYPE_CLIENT && pHeader->em_LinkType != LINK_TYPE_DOG)
						{
							return HR_ERROR;
						}
						else
						{
							required = pHeader->body_len;
							pInfo->em_LinkType = pHeader->em_LinkType;
						}
					}
					else
					{
						TPkgBody* pBody = (TPkgBody*)(BYTE*)buffer;
						Json::Value js;
						g_PaySerDlg->DoRun(pBody->desc,js,pInfo);
						SendTo(dwConnID,js);

						required = sizeof(TPkgHeader);
					}

					pInfo->is_header = !pInfo->is_header;
					pInfo->length	 = required;
				}
				else
				{
					CString str;
					str.Format(L"fetch faild:%d",result);
					AddString(str);
					return HR_ERROR;
				}
			}
		}
		return HR_OK;
	}
	catch (...)
	{
		return HR_ERROR;
	}
}

EnHandleResult CPayServerDlg::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	iErrorCode == SE_OK ? ::PostOnClose(dwConnID)	:
		::PostOnError(dwConnID, enOperation, iErrorCode);

    RemovePkgInfo(pSender, dwConnID);
	CString str;
	str.Format(L"%d 断开，code：%d",dwConnID,iErrorCode);
	AddString(str);
    return HR_OK;
}

EnHandleResult CPayServerDlg::OnShutdown(ITcpServer* pSender)
{
	::PostOnShutdown();
	return HR_OK;
}
