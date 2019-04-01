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
	SOCK_ERROR_NETERROR,//�����쳣
	SOCK_ERROR_OUTMAXBUFF,//���ĳ��ȳ������巶Χ
	SOCK_ERROR_OUTMAX_RECVLEN,//ʵ�ʽ��ճ��ȳ������巶Χ
	SOCK_ERROR_START,//����ͷ����
	SOCK_ERROR_END,//����β����
	SOCK_ERROR_UNKNOW,//δ֪����
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
	BOOL             m_bConnect;//�Ƿ�����
	FUN_CALLBACK     m_pCallBack;
	void*            m_pHand;
	int              m_TryTime;//��������
};
extern SockClient g_SockClient;
