#pragma once
#include <afxwin.h>
#include "..\common\common.h"
#include <string>

using std::string;
#define WM_SOCKMSG   WM_USER+3

enum SOCK_ERROR_MSG
{
	SOCK_ERROR_INITSOCKET,
	SOCK_ERROR_CREATESOCK,
	SOCK_ERROR_CONNECT,
	SOCK_ERROR_SERCLOSE,
	SOCK_ERROR_RECVERROR,
	SOCK_ERROR_NETERROR,//网络异常
	SOCK_ERROR_OUTMAXBUFF,//报文长度超出定义范围
	SOCK_ERROR_OUTMAX_RECVLEN,//实际接收长度超出定义范围
	SOCK_ERROR_START,//报文头错误
	SOCK_ERROR_END,//报文尾错误
	SOCK_ERROR_UNKNOW,//未知错误
	SOCK_ERROR_MAX
};

typedef void (*FUN_CALLBACK)(void* p,string strData);

class SockClient : public CWnd
{
public:
	SockClient();
	~SockClient();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSockMsg(WPARAM wParam, LPARAM lParam);
	
public:
	bool ConnectSer(BOOL bShowMsgBox);
	bool Start(BOOL bShowMsgBox=FALSE);
	void createWin();
	void SetCallback(FUN_CALLBACK callback,void* param);
	int SendTo(string strData,long len = 0);
	void DoRun(string strData);
public:
	HANDLE           m_mutex;
	HANDLE           m_RecvHandle;
	HANDLE           m_HeardHandle;
	BOOL             m_bConnect;//是否连接
	FUN_CALLBACK     m_pCallBack;
	void*            m_pHand;
	int              m_TryTime;//重连次数
};
extern SockClient g_SockClient;
