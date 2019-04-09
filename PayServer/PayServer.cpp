
// PayServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "PayServer.h"
#include "PayServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPayServerApp

BEGIN_MESSAGE_MAP(CPayServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPayServerApp 构造

CPayServerApp::CPayServerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CPayServerApp 对象

CPayServerApp theApp;


// CPayServerApp 初始化

BOOL CPayServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("定制管理系统"));

	HANDLE hMutex=::CreateMutex(NULL,TRUE,L"PayServer-HP2");//第三个参数FirstName这个名称可以任意取，第二个参数要为TRUE表示当前实例占用此互斥对象
	if (hMutex!=NULL)
	{
		if (GetLastError()==ERROR_ALREADY_EXISTS)
		{
			MessageBox(NULL,L"已经有一个服务在运行,如要重新运行，请先关闭服务！",L"提示",MB_OK);
			return 0;
		}
	}

	m_dbData = new CDbData();
	if(!m_dbData->InitSqlite3())
	{
		MessageBox(NULL, L"InitSqlite3 error!", L"error",NULL);
		return FALSE;
	}

	CPayServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

