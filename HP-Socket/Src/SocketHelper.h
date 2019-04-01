/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 4.1.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include <ws2tcpip.h>
#include <mswsock.h>
#include <malloc.h>

#include <atlfile.h>

#include "SocketInterface.h"
#include "../../Common/Src/WaitFor.h"
#include "../../Common/Src/bufferpool.h"
#include "../../Common/Src/RingBuffer.h"

/************************************************************************
���ƣ�ȫ�ֳ���
��������������Ĺ���ȫ�ֳ���
************************************************************************/

#define HP_VERSION_MAJOR	4
#define HP_VERSION_MINOR	1
#define HP_VERSION_REVISE	2
#define HP_VERSION_BUILD	3

/* IOCP ������߳��� */
extern const DWORD MAX_WORKER_THREAD_COUNT;
/* IOCP Socket ��������Сֵ */
extern const DWORD MIN_SOCKET_BUFFER_SIZE;
/* С�ļ�����ֽ��� */
extern const DWORD MAX_SMALL_FILE_SIZE;
/* �������ʱ�� */
extern const DWORD MAX_CONNECTION_PERIOD;

/* Server/Agent Ĭ����������� */
extern const DWORD DEFAULT_MAX_CONNECTION_COUNT;
/* Server/Agent Ĭ�� IOCP �����߳��� */
extern const DWORD DEFAULT_WORKER_THREAD_COUNT;
/* Server/Agent Ĭ�� Socket �����������ʱ�� */
extern const DWORD DEFAULT_FREE_SOCKETOBJ_LOCK_TIME;
/* Server/Agent Ĭ�� Socket ����ش�С */
extern const DWORD DEFAULT_FREE_SOCKETOBJ_POOL;
/* Server/Agent Ĭ�� Socket ����ػ��շ�ֵ */
extern const DWORD DEFAULT_FREE_SOCKETOBJ_HOLD;
/* Server/Agent Ĭ���ڴ�黺��ش�С */
extern const DWORD DEFAULT_FREE_BUFFEROBJ_POOL;
/* Server/Agent Ĭ���ڴ�黺��ػ��շ�ֵ */
extern const DWORD DEFAULT_FREE_BUFFEROBJ_HOLD;
/* Client Ĭ���ڴ�黺��ش�С */
extern const DWORD DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE;
/* Client Ĭ���ڴ�黺��ػ��շ�ֵ */
extern const DWORD DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD;
/* Agent Ĭ�ϰ󶨵�ַ */
extern LPCTSTR DEFAULT_BIND_ADDRESS;

/* TCP Ĭ��ͨ�����ݻ�������С */
extern const DWORD DEFAULT_TCP_SOCKET_BUFFER_SIZE;
/* TCP Ĭ����������� */
extern const DWORD DEFALUT_TCP_KEEPALIVE_TIME;
/* TCP Ĭ������ȷ�ϰ������ */
extern const DWORD DEFALUT_TCP_KEEPALIVE_INTERVAL;
/* TCP Server Ĭ�� Listen ���д�С */
extern const DWORD DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE;
/* TCP Server Ĭ��ԤͶ�� Accept ���� */
extern const DWORD DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT;

/* UDP Ĭ�����ݱ�����󳤶� */
extern const DWORD DEFAULT_UDP_MAX_DATAGRAM_SIZE;
/* UDP Ĭ�� Receive ԤͶ������ */
extern const DWORD DEFAULT_UDP_POST_RECEIVE_COUNT;
/* UDP Ĭ�ϼ������Դ��� */
extern const DWORD DEFAULT_UDP_DETECT_ATTEMPTS;
/* UDP Ĭ�ϼ������ͼ�� */
extern const DWORD DEFAULT_UDP_DETECT_INTERVAL;

/* TCP Pack ������λ�� */
extern const DWORD TCP_PACK_LENGTH_BITS;
/* TCP Pack ���������� */
extern const DWORD TCP_PACK_LENGTH_MASK;
/* TCP Pack ����󳤶�Ӳ���� */
extern const DWORD TCP_PACK_MAX_SIZE_LIMIT;
/* TCP Pack ��Ĭ����󳤶� */
extern const DWORD TCP_PACK_DEFAULT_MAX_SIZE;
/* TCP Pack ��ͷ��ʶֵӲ���� */
extern const USHORT TCP_PACK_HEADER_FLAG_LIMIT;
/* TCP Pack ��ͷĬ�ϱ�ʶֵ */
extern const USHORT TCP_PACK_DEFAULT_HEADER_FLAG;

/************************************************************************
���ƣ�Windows Socket �����ʼ����
�������Զ����غ�ж�� Windows Socket ���
************************************************************************/
class CInitSocket
{
public:
	CInitSocket(LPWSADATA lpWSAData = nullptr, BYTE minorVersion = 2, BYTE majorVersion = 2)
	{
		LPWSADATA lpTemp = lpWSAData;
		if(!lpTemp)
			lpTemp	= (LPWSADATA)_alloca(sizeof(WSADATA));

		m_iResult	= ::WSAStartup(MAKEWORD(majorVersion, minorVersion), lpTemp);
	}

	~CInitSocket()
	{
		if(IsValid())
			::WSACleanup();
	}

	int		GetResult()	{return m_iResult;}
	BOOL	IsValid()	{return m_iResult == 0;}

private:
	int		m_iResult;
};

/* Server ����� Agent ����ڲ�ʹ�õ��¼����������� */

// �����ѹر�
#define HR_CLOSED	0xFF

/* �ر����ӱ�ʶ */
enum EnSocketCloseFlag
{
	SCF_NONE	= 0,	// �������¼�
	SCF_CLOSE	= 1,	// ���� �����ر� OnClose �¼�
	SCF_ERROR	= 2		// ���� �쳣�ر� OnClose �¼�
};

/* ���ݻ����������ṹ */
template<class T> struct TBufferObjBase
{
	WSAOVERLAPPED		ov;
	EnSocketOperation	operation;
	WSABUF				buff;

	int					capacity;
	CPrivateHeap&		heap;

	T* next;
	T* last;

	static T* Construct(CPrivateHeap& heap, DWORD dwCapacity)
	{
		T* pBufferObj = (T*)heap.Alloc(sizeof(T) + dwCapacity);
		ASSERT(pBufferObj);

		ZeroMemory(pBufferObj, sizeof(T));
		
		pBufferObj->TBufferObjBase::TBufferObjBase(heap, dwCapacity);
		pBufferObj->buff.buf = ((char*)pBufferObj) + sizeof(T);

		return pBufferObj;
	}

	static void Destruct(T* pBufferObj)
	{
		ASSERT(pBufferObj);
		pBufferObj->heap.Free(pBufferObj);
	}

	TBufferObjBase(CPrivateHeap& hp, DWORD dwCapacity)
	: heap(hp)
	, capacity((int)dwCapacity)
	{
		ASSERT(capacity > 0);
	}

	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0);

		int cat = min(Remain(), length);

		if(cat > 0)
		{
			memcpy(buff.buf + buff.len, pData, cat);
			buff.len += cat;
		}

		return cat;
	}

	void Reset()	{buff.len = 0;}
	int Remain()	{return capacity - buff.len;}
	BOOL IsFull()	{return buff.len == capacity;}
};

/* ���ݻ������ṹ */
struct TBufferObj : public TBufferObjBase<TBufferObj>
{
	SOCKET client;
};

/* UDP ���ݻ������ṹ */
struct TUdpBufferObj : public TBufferObjBase<TUdpBufferObj>
{
	SOCKADDR_IN	remoteAddr;
	int			addrLen;
};

/* ���ݻ���������ģ�� */
template<class T> struct TBufferObjListT : public TSimpleList<T>
{
public:
	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0);

		int remain = length;

		while(remain > 0)
		{
			T* pItem = Back();

			if(pItem == nullptr || pItem->IsFull())
				pItem = PushBack(bfPool.PickFreeItem());

			int cat  = pItem->Cat(pData, remain);

			pData	+= cat;
			remain	-= cat;
		}

		return length;
	}

	T* PushTail(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0 && length <= (int)bfPool.GetItemCapacity());

		T* pItem = PushBack(bfPool.PickFreeItem());
		pItem->Cat(pData, length);

		return pItem;
	}

	void Release()
	{
		bfPool.PutFreeItem(*this);
	}

public:
	TBufferObjListT(CNodePoolT<T>& pool) : bfPool(pool)
	{
	}

private:
	CNodePoolT<T>& bfPool;
};

/* ���ݻ���������� */
typedef CNodePoolT<TBufferObj>			CBufferObjPool;
/* UDP ���ݻ���������� */
typedef CNodePoolT<TUdpBufferObj>		CUdpBufferObjPool;
/* ���ݻ���������ģ�� */
typedef TBufferObjListT<TBufferObj>		TBufferObjList;
/* UDP ���ݻ���������ģ�� */
typedef TBufferObjListT<TUdpBufferObj>	TUdpBufferObjList;

/* ���ݻ������ṹ���� */
typedef CRingPool<TBufferObj>		TBufferObjPtrList;

/* Udp ���ݻ������ṹ���� */
typedef CRingPool<TUdpBufferObj>	TUdpBufferObjPtrList;

/* Socket �����������ṹ */
struct TSocketObjBase
{
	CONNID		connID;
	SOCKADDR_IN	remoteAddr;
	PVOID		extra;
	PVOID		reserved;
	PVOID		reserved2;
	BOOL		valid;

	union
	{
		DWORD	freeTime;
		DWORD	connTime;
	};

	DWORD		activeTime;

	CCriSec		csSend;

	volatile BOOL smooth;
	volatile long pending;
	volatile long sndCount;

	CReentrantSpinGuard csRecv;

	static BOOL IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static BOOL IsValid(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr && pSocketObj->valid;}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	static BOOL IsSmooth(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); return pSocketObj->valid && pSocketObj->smooth;}

	static BOOL IsPending(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); return pSocketObj->valid && pSocketObj->pending > 0;}

	static void Release(TSocketObjBase* pSocketObj)
	{
		ASSERT(IsExist(pSocketObj));

		pSocketObj->freeTime = ::TimeGetTime();
	}

	int Pending() {return pending;}

	void Reset(CONNID dwConnID)
	{
		connID	 = dwConnID;
		valid	 = TRUE;
		smooth	 = TRUE;
		pending	 = 0;
		sndCount = 0;
		extra	 = nullptr;
		reserved = nullptr;
		reserved2= nullptr;
	}
};

/* ���ݻ������ṹ */
struct TSocketObj : public TSocketObjBase
{
	SOCKET			socket;
	CStringA		host;
	TBufferObjList	sndBuff;
	
	TSocketObj(CBufferObjPool& bfPool)
	: sndBuff(bfPool)
	{

	}

	static void Release(TSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);
		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID, SOCKET soClient)
	{
		__super::Reset(dwConnID);
		
		host.Empty();

		socket = soClient;
	}
};

/* UDP ���ݻ������ṹ */
struct TUdpSocketObj : public TSocketObjBase
{
	TUdpBufferObjList	sndBuff;
	volatile DWORD		detectFails;

	TUdpSocketObj(CUdpBufferObjPool& bfPool)
	: sndBuff(bfPool)
	{

	}

	static void Release(TUdpSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);
		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);
		detectFails = 0;
	}
};

/* ��Ч TSocketObj ���� */
typedef CRingCache2<TSocketObj, CONNID, true>		TSocketObjPtrPool;
/* ʧЧ TSocketObj ���� */
typedef CRingPool<TSocketObj>						TSocketObjPtrList;
/* ʧЧ TSocketObj �������սṹ���� */
typedef CCASQueue<TSocketObj>						TSocketObjPtrQueue;

/* ��Ч TUdpSocketObj ���� */
typedef CRingCache2<TUdpSocketObj, CONNID, true>	TUdpSocketObjPtrPool;
/* ʧЧ TUdpSocketObj ���� */
typedef CRingPool<TUdpSocketObj>					TUdpSocketObjPtrList;
/* ʧЧ TUdpSocketObj �������սṹ���� */
typedef CCASQueue<TUdpSocketObj>					TUdpSocketObjPtrQueue;

/* SOCKADDR_IN �Ƚ��� */
struct sockaddr_func
{
	struct hash
	{
		size_t operator() (const SOCKADDR_IN* pA) const
		{
			return ((pA->sin_family << 16) | ntohs(pA->sin_port)) ^ pA->sin_addr.s_addr;
		}
	};

	struct equal_to
	{
		bool operator () (const SOCKADDR_IN* pA, const SOCKADDR_IN* pB) const
		{
			return memcmp(pA, pB, offsetof(SOCKADDR_IN, sin_zero)) == 0;
		}
	};

};

/* ��ַ-���� ID ��ϣ�� */
typedef unordered_map<SOCKADDR_IN*, CONNID, sockaddr_func::hash, sockaddr_func::equal_to>
										TSockAddrMap;
/* ��ַ-���� ID ��ϣ������� */
typedef TSockAddrMap::iterator			TSockAddrMapI;
/* ��ַ-���� ID ��ϣ�� const ������ */
typedef TSockAddrMap::const_iterator	TSockAddrMapCI;

/* IClient ����ر������� */
struct TClientCloseContext
{
	BOOL bFireOnClose;
	EnSocketOperation enOperation;
	int iErrorCode;

	TClientCloseContext(BOOL bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK)
	{
		Reset(bFire, enOp, iCode);
	}

	void Reset(BOOL bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK)
	{
		bFireOnClose = bFire;
		enOperation	 = enOp;
		iErrorCode	 = iCode;
	}

};

/*****************************************************************************************************/
/******************************************** ������������ ********************************************/
/*****************************************************************************************************/

// ��ȡ HPSocket �汾�ţ�4 ���ֽڷֱ�Ϊ�����汾�ţ��Ӱ汾�ţ������汾�ţ�������ţ�
DWORD GetHPSocketVersion();

/* ��ȡ���������ı� */
LPCTSTR GetSocketErrorDesc(EnSocketError enCode);
/* IPv4 �ַ�����ַת��Ϊ���� */
ULONG GetIPv4InAddr(LPCTSTR lpszAddress);
/* ����ַ����Ƿ���� IP ��ַ��ʽ */
BOOL IsIPAddress(LPCTSTR lpszAddress);
/* ͨ����������ȡ IP ��ַ */
BOOL GetIPAddress(LPCTSTR lpszHost, __out LPTSTR lpszIP, __inout int& iIPLenth);
/* ͨ����������ȡ���ŵ� IP ��ַ */
BOOL GetOptimalIPByHostName(LPCTSTR lpszHost, __out IN_ADDR& addr);
/* ��ȡ IN_ADDR �ṹ�� IP ��ַ */
BOOL IN_ADDR_2_IP(const IN_ADDR& addr, __out LPTSTR lpszAddress, __inout int& iAddressLen);
/* �� SOCKADDR_IN �ṹת��Ϊ��ַ���� */
BOOL sockaddr_IN_2_A(const SOCKADDR_IN& addr, __out ADDRESS_FAMILY& usFamily, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);
/* �ѵ�ַ����ת��Ϊ SOCKADDR_IN �ṹ */
BOOL sockaddr_A_2_IN(ADDRESS_FAMILY usFamily, LPCTSTR lpszAddress, USHORT usPort, __out SOCKADDR_IN& addr);
/* ��ȡ Socket �ı��ػ�Զ�̵�ַ��Ϣ */
BOOL GetSocketAddress(SOCKET socket, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort, BOOL bLocal = TRUE);
/* ��ȡ Socket �ı��ص�ַ��Ϣ */
BOOL GetSocketLocalAddress(SOCKET socket, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);
/* ��ȡ Socket ��Զ�̵�ַ��Ϣ */
BOOL GetSocketRemoteAddress(SOCKET socket, __out LPTSTR lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);

/* 64 λ�����ֽ���ת�����ֽ��� */
ULONGLONG NToH64(ULONGLONG value);
/* 64 λ�����ֽ���ת�����ֽ��� */
ULONGLONG HToN64(ULONGLONG value);

/* ��ȡ Socket ��ĳ����չ������ָ�� */
PVOID GetExtensionFuncPtr					(SOCKET sock, GUID guid);
/* ��ȡ AcceptEx ��չ����ָ�� */
LPFN_ACCEPTEX Get_AcceptEx_FuncPtr			(SOCKET sock);
/* ��ȡ GetAcceptExSockaddrs ��չ����ָ�� */
LPFN_GETACCEPTEXSOCKADDRS Get_GetAcceptExSockaddrs_FuncPtr(SOCKET sock);
/* ��ȡ ConnectEx ��չ����ָ�� */
LPFN_CONNECTEX Get_ConnectEx_FuncPtr		(SOCKET sock);
/* ��ȡ TransmitFile ��չ����ָ�� */
LPFN_TRANSMITFILE Get_TransmitFile_FuncPtr	(SOCKET sock);
/* ��ȡ DisconnectEx ��չ����ָ�� */
LPFN_DISCONNECTEX Get_DisconnectEx_FuncPtr	(SOCKET sock);

HRESULT ReadSmallFile(LPCTSTR lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, DWORD dwMaxFileSize = MAX_SMALL_FILE_SIZE);
HRESULT MakeSmallFilePackage(LPCTSTR lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, WSABUF szBuf[3], const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);

/************************************************************************
���ƣ�IOCP ָ��Ͷ�ݰ�������
�������� IOCP ָ��Ͷ��
************************************************************************/

/* IOCP ���� */
enum EnIocpCommand
{
	IOCP_CMD_EXIT		= 0x00000000,	// �˳�����
	IOCP_CMD_ACCEPT		= 0xFFFFFFF1,	// ��������
	IOCP_CMD_DISCONNECT	= 0xFFFFFFF2,	// �Ͽ�����
	IOCP_CMD_SEND		= 0xFFFFFFF3	// ��������
};

/* IOCP ������� */
enum EnIocpAction
{
	IOCP_ACT_GOON		= 0,	// ����ִ��
	IOCP_ACT_CONTINUE	= 1,	// ����ִ��
	IOCP_ACT_BREAK		= 2		// �ж�ִ��
};

BOOL PostIocpCommand(HANDLE hIOCP, EnIocpCommand enCmd, ULONG_PTR ulParam);
BOOL PostIocpExit(HANDLE hIOCP);
BOOL PostIocpAccept(HANDLE hIOCP);
BOOL PostIocpDisconnect(HANDLE hIOCP, CONNID dwConnID);
BOOL PostIocpSend(HANDLE hIOCP, CONNID dwConnID);

/************************************************************************
���ƣ�setsockopt() ��������
�������򻯳��õ� setsockopt() ����
************************************************************************/

int SSO_SetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int len);
int SSO_GetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int* len);
int SSO_IoctlSocket			(SOCKET sock, long cmd, u_long* arg);
int SSO_WSAIoctl			(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

int SSO_UpdateAcceptContext	(SOCKET soClient, SOCKET soBind);
int SSO_UpdateConnectContext(SOCKET soClient, int iOption);
int SSO_NoDelay				(SOCKET sock, BOOL bNoDelay = TRUE);
int SSO_DontLinger			(SOCKET sock, BOOL bDont = TRUE);
int SSO_Linger				(SOCKET sock, USHORT l_onoff, USHORT l_linger);
int SSO_KeepAlive			(SOCKET sock, BOOL bKeepAlive = TRUE);
int SSO_KeepAliveVals		(SOCKET sock, u_long onoff, u_long time, u_long interval);
int SSO_RecvBuffSize		(SOCKET sock, int size);
int SSO_SendBuffSize		(SOCKET sock, int size);
int SSO_ReuseAddress		(SOCKET sock, BOOL bReuse = TRUE);
int SSO_UDP_ConnReset		(SOCKET sock, BOOL bNewBehavior = TRUE);

/************************************************************************
���ƣ�Socket ��������
������Socket ������װ����
************************************************************************/

/* ��� IOCP ��������ֵ��NO_ERROR �򷵻� TRUE */
#define IOCP_NO_ERROR(result)	(result == NO_ERROR)
/* ��� IOCP ��������ֵ��WSA_IO_PENDING �򷵻� TRUE */
#define IOCP_PENDING(result)	(result == WSA_IO_PENDING)
/* ��� IOCP ��������ֵ��NO_ERROR �� WSA_IO_PENDING �򷵻� TRUE */
#define IOCP_SUCCESS(result)	(IOCP_NO_ERROR(result) || IOCP_PENDING(result))

/* ���� Connection ID */
CONNID GenerateConnectionID	();
/* �ر� Socket */
int ManualCloseSocket		(SOCKET sock, int iShutdownFlag = 0xFF, BOOL bGraceful = TRUE, BOOL bReuseAddress = FALSE);
/* Ͷ�� AccceptEx()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostAccept				(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj);
/* Ͷ�� AccceptEx() */
int PostAcceptNotCheck		(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj);
/* Ͷ�� ConnectEx()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostConnect				(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, SOCKADDR_IN& soAddrIN, TBufferObj* pBufferObj);
/* Ͷ�� ConnectEx() */
int PostConnectNotCheck		(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, SOCKADDR_IN& soAddrIN, TBufferObj* pBufferObj);
/* Ͷ�� WSASend()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostSend				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSASend() */
int PostSendNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSARecv() ������ WSA_IO_PENDING ת��Ϊ NO_ERROR*/
int PostReceive				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSARecv() */
int PostReceiveNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSASendTo() ������ WSA_IO_PENDING ת��Ϊ NO_ERROR*/
int PostSendTo				(SOCKET sock, TUdpBufferObj* pBufferObj);
/* Ͷ�� WSASendTo() */
int PostSendToNotCheck		(SOCKET sock, TUdpBufferObj* pBufferObj);
/* Ͷ�� WSARecvFrom() ������ WSA_IO_PENDING ת��Ϊ NO_ERROR*/
int PostReceiveFrom			(SOCKET sock, TUdpBufferObj* pBufferObj);
/* Ͷ�� WSARecvFrom() */
int PostReceiveFromNotCheck	(SOCKET sock, TUdpBufferObj* pBufferObj);
