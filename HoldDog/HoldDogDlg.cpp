
// HoldDogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HoldDog.h"
#include "HoldDogDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>
#include <tlhelp32.h>
#include <shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"HPSocket_U.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CHoldDogDlg 对话框




CHoldDogDlg::CHoldDogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHoldDogDlg::IDD, pParent),m_Client(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_NoResponseTime = 0;
}

CHoldDogDlg::~CHoldDogDlg()
{

}

void CHoldDogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHoldDogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_INITDLG, &CHoldDogDlg::OnInitDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CHoldDogDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CHoldDogDlg 消息处理程序

BOOL CHoldDogDlg::OnInitDialog()
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
	PostMessage(WM_INITDLG);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHoldDogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHoldDogDlg::OnPaint()
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
HCURSOR CHoldDogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CHoldDogDlg::GetWorkDir() 
{  
	TCHAR pFileName[MAX_PATH]; 
	int nPos = GetCurrentDirectory( MAX_PATH, pFileName); 

	CString csFullPath(pFileName);  
	if( nPos < 0 ) 
		return CString(""); 
	else 
		return csFullPath; 
}

BOOL CHoldDogDlg::ReadConfig()
{
	USES_CONVERSION;
	fstream _file;
	_file.open(m_ConfigFilePath,ios::in);
	if(!_file)
	{
		return false;
	}
	else
	{
		TCHAR ip[30];
		GetPrivateProfileString(L"SOCKET",L"ip",L"",ip,30,m_ConfigFilePath);
		m_LocalIP=W2A(ip);	
		m_TcpPort=GetPrivateProfileInt(L"SOCKET",L"port",0,m_ConfigFilePath);
	}
	_file.close();
	return true;
}

LRESULT CHoldDogDlg::OnInitDlg(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;
	m_pkgInfo.Reset();
	CString workPath = GetWorkDir();
	m_ConfigFilePath = workPath+L"/config/config.ini";

	if (ReadConfig())
	{
		BOOL bRet = m_Client->Start(A2W(m_LocalIP.c_str()), m_TcpPort, FALSE);
		if(bRet)
		{
			//开始定时发送心跳
			SetTimer(1,2000,NULL);
		}
		else
			OnCancel();
	}
	else
		OnCancel();

	return TRUE;
}

void CHoldDogDlg::DoRun(string strData)
{
	Json::Reader r;
	Json::Value root;
	r.parse(strData,root);
	EM_SOCK_CMD cmd = (EM_SOCK_CMD)root[CONNECT_CMD].asInt();
	switch (cmd)
	{
	case SOCK_CMD_HEART:
		{
			m_NoResponseTime = 0;
		}
		break;
	}

}

EnHandleResult CHoldDogDlg::OnConnect(ITcpClient* pSender, CONNID dwConnID)
{
	return HR_OK;
}

EnHandleResult CHoldDogDlg::OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength)
{
	return HR_OK;
}

EnHandleResult CHoldDogDlg::OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength)
{
	ITcpPullClient* pClient	= ITcpPullClient::FromS(pSender);

	int required = m_pkgInfo.length;
	int remain = iLength;

	while(remain >= required)
	{
		remain -= required;
		CBufferPtr buffer(required);

		EnFetchResult result = pClient->Fetch(buffer, (int)buffer.Size());
		if(result == FR_OK)
		{
			if(m_pkgInfo.is_header)
			{
				TPkgHeader* pHeader = (TPkgHeader*)buffer.Ptr();
				required = pHeader->body_len;
			}
			else
			{
				TPkgBody* pBody = (TPkgBody*)buffer.Ptr();
				DoRun(pBody->desc);

				required = sizeof(TPkgHeader);
			}

			m_pkgInfo.is_header	= !m_pkgInfo.is_header;
			m_pkgInfo.length	= required;
		}
	}
	return HR_OK;
}

EnHandleResult CHoldDogDlg::OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
{
	OnCancel();
	return HR_OK;
}

int CHoldDogDlg::SendTo(string strData)
{
	static DWORD SEQ = 0;//流水号
	LPCSTR desc = strData.c_str();
	int desc_len = (int)strlen(desc) + 1;
	int body_len = offsetof(TPkgBody, desc) + desc_len;

	TPkgHeader header;
	header.seq = ++SEQ;
	header.em_LinkType = LINK_TYPE_DOG;
	header.body_len = body_len;

	TPkgBody* pBody = (TPkgBody*)_alloca(body_len);
	memset(pBody, 0, body_len);
	strcpy(pBody->desc, desc);

	WSABUF bufs[2];
	bufs[0].len = sizeof(TPkgHeader);
	bufs[0].buf = (char*)&header;
	bufs[1].len = body_len;
	bufs[1].buf = (char*)pBody;

	if(m_Client->SendPackets(bufs, 2))
		return 0;
	else
	{
		return ::SYS_GetLastError();
	}
}

void CHoldDogDlg::KillSer()
{
	USES_CONVERSION;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	PROCESSENTRY32 pe32;
	HANDLE hProcess;
	char *kill[]={"PayServer-HP.exe","PayServer-HP"};
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

void CHoldDogDlg::RunSer()
{
	CString strPath;
	int nPos;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	nPos = strPath.ReverseFind(_T('\\'));
	strPath = strPath.Left(nPos);


	CString serPathName = strPath+L"\\PayServer-HP.exe";
	if (!PathFileExists(serPathName))
	{
		CString str;
		str.Format(L"找不到文件：%s",serPathName);
		MessageBox(str);
		return;
	}

	ShellExecute(NULL,L"open",serPathName,NULL,strPath,SW_SHOWNORMAL);
}

void CHoldDogDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_NoResponseTime >=3)
	{
		KillTimer(1);
		//先强制kill服务进程
		KillSer();
		//重启服务器
		RunSer();
		//退出看门狗程序，由服务重新启动
		OnCancel();
	}
	else
	{
		Json::Value root;
		root[CONNECT_CMD]=SOCK_CMD_HEART;
		Json::FastWriter writer;  
		string temp = writer.write(root);
		if(SendTo(temp) != 0)
		{

		}
		m_NoResponseTime++;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CHoldDogDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CHoldDogDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(1);
	EnServiceState state =  m_Client->GetState();
	if (state != SS_STOPPED && state != SS_STOPPING)
	{
		m_Client->Stop();
	}
}


void CHoldDogDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO: 在此处添加消息处理程序代码
	lpwndpos->flags &= ~SWP_SHOWWINDOW;
}
