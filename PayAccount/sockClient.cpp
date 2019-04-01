#include "stdafx.h"
#include "sockClient.h"
#include "..\common\globle.h"

SockClient g_SockClient;
SOCKET g_CliSock= INVALID_SOCKET;

DWORD WINAPI RecvThread(LPVOID lpParam);
DWORD WINAPI HeardThread(LPVOID lpParam);

#define  SHOW_MSGBOX 0

SockClient::SockClient()
{
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	m_RecvHandle=INVALID_HANDLE_VALUE;
	m_HeardHandle=INVALID_HANDLE_VALUE;
	m_bConnect = FALSE;
	m_pCallBack = NULL;
	m_pHand = NULL;
	m_TryTime=0;
}

SockClient::~SockClient()
{
	m_bConnect = FALSE;
	if (m_HeardHandle!=INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_HeardHandle,INFINITE);
		m_HeardHandle=INVALID_HANDLE_VALUE;
	}
	if (m_RecvHandle!=INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_RecvHandle,INFINITE);
		m_RecvHandle=INVALID_HANDLE_VALUE;
	}

	CWnd::DestroyWindow();
}

void SockClient::createWin()
{
	CWnd::Create(NULL,L"SockClient",WS_CHILD,CRect(0,0,0,0),::AfxGetMainWnd(),1234);
}

BEGIN_MESSAGE_MAP(SockClient,CWnd)
	ON_MESSAGE(WM_SOCKMSG, &SockClient::OnSockMsg)
END_MESSAGE_MAP()

LRESULT SockClient::OnSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case SOCK_ERROR_INITSOCKET:
		{
			MessageBox(L"初始化套接字失败！",L"错误");
			break;
		}
	case SOCK_ERROR_CREATESOCK:
		{
			MessageBox(L"连接服务器失败：create socket error!",L"错误");
			break;
		}
	case SOCK_ERROR_CONNECT:
		{
			MessageBox(L"连接服务器失败：socket connect error!",L"错误");
			break;
		}
	case SOCK_ERROR_SERCLOSE:
		{
			Log("Server have closed the connection!");
			MessageBox(L"Server have closed the connection!",L"");
			break;
		}
	case SOCK_ERROR_RECVERROR:
		{
			Log("Receive Server data Failed!");
			MessageBox(L"Receive Server data Failed!",L"");
			break;
		}
	case SOCK_ERROR_NETERROR:
		{
			int err = wParam;
			CString str;
			str.Format(L"send error:%d",err);
#if SHOW_MSGBOX
			int ret = MessageBox(L"网络异常,单击‘重试’尝试重新连接。",str,MB_RETRYCANCEL);
			if(ret==IDRETRY)
			{
				Start(TRUE);
			}
#else
			if (m_TryTime <= 2)
			{
				Start();
			}
			else
			{
				int ret = MessageBox(L"网络异常,单击‘重试’尝试重新连接。",str,MB_RETRYCANCEL);
				if(ret==IDRETRY)
				{
					Start(TRUE);
				}
			}
#endif
			break;
		}
	case SOCK_ERROR_OUTMAXBUFF:
		{
			Log("报文超过最大接收长度");
			MessageBox(L"报文超过最大接收长度!",L"错误");
			Start();
			break;
		}
	case SOCK_ERROR_OUTMAX_RECVLEN:
		{
			Log("实际接收长度超出定义范围");
			MessageBox(L"实际接收长度超出定义范围!",L"错误");
			Start();
			break;
		}
	case SOCK_ERROR_START:
		{
			Log("报文头错误");
#if SHOW_MSGBOX
			MessageBox(L"报文头错误！",L"错误");
#endif
			Start();
			break;
		}
	case SOCK_ERROR_END:
		{
			Log("报文尾错误");
#if SHOW_MSGBOX
			MessageBox(L"报文尾错误！",L"错误");
#endif
			Start();
			break;
		}
	case SOCK_ERROR_UNKNOW:
		{
			Log("未知的异常网络事件");
			MessageBox(L"未知的异常网络事件！",L"错误");
			break;
		}
	}
	return TRUE;
}

void SockClient::SetCallback(FUN_CALLBACK callback,void* param)
{
	m_pCallBack = callback;
	m_pHand = param;
}

bool SockClient::ConnectSer(BOOL bShowMsgBox)
{
	//初始化socket
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
	{
		if(bShowMsgBox)
			PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_INITSOCKET);
		return false;
	}

	g_CliSock = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if (g_CliSock==INVALID_SOCKET)
	{
		if(bShowMsgBox)
		    PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_CREATESOCK);
		return false; 
	}

	HOSTENT *host_entry;
	SOCKADDR_IN   addrClient;
	int nFind = -1;
	int nNum = 0;
	string strName = g_Globle.m_LocalIP;
	nFind=strName.find(".");
	while (nFind>=0)
	{
		nNum++;
		strName = strName.substr(nFind+1);
		nFind=strName.find(".");
	}
	if(nNum == 3)
		addrClient.sin_addr.S_un.S_addr = inet_addr(g_Globle.m_LocalIP.c_str());
	else
	{
		host_entry=gethostbyname(g_Globle.m_LocalIP.c_str());
		char cip[20]={0};
		if(host_entry!=0)
		{
			sprintf(cip,"%d.%d.%d.%d",
				(host_entry->h_addr_list[0][0]&0x00ff),
				(host_entry->h_addr_list[0][1]&0x00ff),
				(host_entry->h_addr_list[0][2]&0x00ff),
				(host_entry->h_addr_list[0][3]&0x00ff));
		}
		addrClient.sin_addr.S_un.S_addr = inet_addr(cip);
	}
	
	addrClient.sin_family = AF_INET;
	addrClient.sin_port = htons(g_Globle.m_TcpPort); 

	if(SOCKET_ERROR == connect(g_CliSock, (SOCKADDR*)&addrClient, sizeof(SOCKADDR)))
	{
		if(bShowMsgBox)
		    PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_CONNECT);
		closesocket(g_CliSock);
		WSACleanup();
		return false;
	}
	return true;
}

bool SockClient::Start(BOOL bShowMsgBox)
{
	if (m_bConnect)
	{
		return true;
	}
	else
	{
		if (m_HeardHandle != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(m_HeardHandle,INFINITE);
			m_HeardHandle = INVALID_HANDLE_VALUE;
		}
		if (m_RecvHandle != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(m_RecvHandle,INFINITE);
			m_RecvHandle = INVALID_HANDLE_VALUE;
		}
	}

	if (ConnectSer(bShowMsgBox))
	{
		//启动心跳和接收线程
		m_TryTime = 0;
		m_bConnect = TRUE;
		m_HeardHandle = CreateThread(NULL,0,HeardThread,this,0,NULL);
		m_RecvHandle = CreateThread(NULL,0,RecvThread,this,0,NULL);
		return true;
	}
	else
	{
		Log("ConnectSer error");
#if SHOW_MSGBOX
		int ret = MessageBox(L"网络异常,单击‘重试’尝试重新连接。",L"Connect error",MB_RETRYCANCEL);
		if(ret==IDRETRY)
		{
			return Start(TRUE);
		}
		else
			return false;
#else
		if (!bShowMsgBox && m_TryTime<=2)
		{
			Sleep(100);
			m_TryTime++;
			return Start();
		}
		else
		{
			int ret = MessageBox(L"网络异常,单击‘重试’尝试重新连接。",L"Connect error",MB_RETRYCANCEL);
			if(ret==IDRETRY)
			{
				return Start(TRUE);
			}
			else
				return false;
		}
#endif
	}
}

//发送心跳
DWORD WINAPI HeardThread(LPVOID lpParam)
{
	USES_CONVERSION;
	SockClient* pThis = (SockClient*)lpParam;
	if (g_CliSock != INVALID_SOCKET)
	{
		for (int i=0;i<20 && pThis->m_bConnect;i++)
		{
			Sleep(100);
			if (i == 19)
			{
				Json::Value root;
				root[CONNECT_CMD]=SOCK_CMD_HEART;
				root[HEARTMSG[EM_HEART_USER]]=T2A(g_Globle.m_strUserName);
				root[HEARTMSG[EM_HEART_NAME]]=T2A(g_Globle.m_strName);
				Json::FastWriter writer;  
				string temp = writer.write(root);
				int ret = pThis->SendTo(temp);
				if(ret == 0)
					i=0;
				else
				{
					pThis->m_bConnect = FALSE;
					pThis->PostMessageW(WM_SOCKMSG,ret,SOCK_ERROR_NETERROR);
					break;
				}
				
			}
		}
	}
	return 0;
}

int SockClient::SendTo(string strData,long len)
{
	int  Ret = 0;
	WaitForSingleObject(m_mutex, INFINITE);
	do 
	{
		string tr = strData.substr(strData.length()-1,1);
		if (tr=="\n")
		{
			strData = strData.substr(0,strData.length()-1);
		}

		long AllLen = 0;
		char* sendBuf = g_Globle.CombineSendData(strData,AllLen);
		if(sendBuf == NULL)
		{
			MessageBox(L"sendBuf NULL!",L"错误");
			break;
		}
		else if (AllLen > MAXBUFFLEN)
		{
			MessageBox(L"请求失败，原因：发送数据长度已超出最大定义长度！",L"错误");
			break;
		}
		else
		{
			byte start = sendBuf[0];
			if (start != MSG_BEGN)
			{
				MessageBox(L"报文头错误,终止发送请求！");
				break;
			}
		}

		DWORD NumberOfBytesSent = 0;
		DWORD dwBytesSent = 0;
		WSABUF Buffers;

		if (g_CliSock != INVALID_SOCKET)
		{
			do
			{
				Buffers.buf = sendBuf;
				Buffers.len = AllLen;//strlen(szResponse)遇到0x00会中断
				Ret = WSASend(g_CliSock,&Buffers,1,&NumberOfBytesSent,0,0,NULL);  
				if(SOCKET_ERROR != Ret)
					dwBytesSent += NumberOfBytesSent;
				else
				{
					Ret = WSAGetLastError();
					Log("send error : %d",Ret);
					break;
				}
			}
			while((dwBytesSent < AllLen) && SOCKET_ERROR != Ret);
		}
		else
			Ret = -2;
		delete[] sendBuf;
	} while (0);
	
	ReleaseMutex(m_mutex); 
	return Ret;
}

void SockClient::DoRun(string strData)
{
	try
	{
		if (m_pCallBack && m_pHand)
		{
			m_pCallBack(m_pHand,strData);
		}
	}
	catch (...)
	{
	}
}

DWORD WINAPI RecvThread(LPVOID lpParam)
{
	SockClient* pThis = (SockClient*)lpParam;

	WSAEVENT Event = WSACreateEvent();
	WSANETWORKEVENTS NetWorkEvent;
	long maxlen = MAXBUFFLEN;
	char* szRequest=new char[maxlen];
	memset(szRequest,0,maxlen);
	DWORD NumberOfBytesRecvd;
	DWORD dwIndex = 0;
	WSABUF Buffers;
	DWORD dwBufferCount = 1;
	DWORD Flags = 0;
	Buffers.buf = szRequest;
	Buffers.len = 5;//5为数据头长度

	int Ret = WSAEventSelect(g_CliSock, Event, FD_READ | FD_WRITE | FD_CLOSE);

	//数据长度
	long dataLen=0;
	long   revLen =0;
	while (pThis->m_bConnect)
	{
		dwIndex = WSAWaitForMultipleEvents(1,&Event,FALSE,1000,FALSE);
		dwIndex = dwIndex - WAIT_OBJECT_0;
		if (dwIndex==WSA_WAIT_TIMEOUT||dwIndex==WSA_WAIT_FAILED)
		{
			continue;
		}

		// 分析什么网络事件产生
		Ret = WSAEnumNetworkEvents(g_CliSock,Event,&NetWorkEvent);
		//其他情况
		if(!NetWorkEvent.lNetworkEvents)
		{
			continue;
		}
		else if (NetWorkEvent.lNetworkEvents & FD_READ)
		{
			NumberOfBytesRecvd=0;
			Ret = WSARecv(g_CliSock,&Buffers,dwBufferCount,&NumberOfBytesRecvd,&Flags,NULL,NULL);
			if (Ret == SOCKET_ERROR)
			{
				revLen = 0;
				Buffers.len = 5;
				memset(szRequest,0,maxlen);
				WSAResetEvent(Event);
				continue;
			}
			else
			{
				revLen+=NumberOfBytesRecvd;

				byte start = szRequest[0];
				if (start != MSG_BEGN)
				{
					pThis->m_bConnect = FALSE;
					pThis->PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_START);
					break;
				}
				else if (revLen<5)
				{
					Buffers.buf+=NumberOfBytesRecvd;
					Buffers.len = 5-revLen;//数据+0x03
					continue;
				}
				else
				{
					char LenBuff[4] = {0};
					memcpy(LenBuff,szRequest+1,4);

					//大端机，进行字节序转换
					if (g_Globle.EndianJudge())
						dataLen = ZZ_SWAP_4BYTE(*(long*)LenBuff);
					else
						dataLen = *(long*)LenBuff;

					if (dataLen+6>maxlen)
					{
						pThis->m_bConnect = FALSE;
						pThis->PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_OUTMAXBUFF);
						break;
					}

					if (revLen < 5+dataLen+1)
					{
						Buffers.buf+=NumberOfBytesRecvd;
						Buffers.len = dataLen+1-(revLen-5);//数据+0x03
						continue;
					}
					else if (revLen == 5+dataLen+1)
					{
						Buffers.buf=Buffers.buf-revLen+NumberOfBytesRecvd;
						Buffers.len=5;
						if (szRequest[revLen-1] != MSG_END)
						{
							pThis->m_bConnect = FALSE;
							pThis->PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_END);
							break;
						}
					}
					else
					{
						pThis->m_bConnect = FALSE;
						pThis->PostMessageW(WM_SOCKMSG,NULL,SOCK_ERROR_OUTMAX_RECVLEN);
						break;
					}
				}
			}
			//此处进行消息处理,根据长度获取到数据buffer
			char* data=new char[dataLen];
			memcpy(data,szRequest+5,dataLen);

			revLen = 0;
			Buffers.len = 5;
			memset(szRequest,0,maxlen);

			pThis->DoRun(data);
			delete[] data;
		}

	}//while

	if (szRequest)
	{
		delete[] szRequest;
	}

	WSACloseEvent(Event);
	closesocket(g_CliSock);
	g_CliSock=INVALID_SOCKET;
	WSACleanup();
	return 0 ;
}