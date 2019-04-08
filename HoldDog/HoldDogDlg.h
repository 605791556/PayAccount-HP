
// HoldDogDlg.h : ͷ�ļ�
//

#pragma once
#include "HPSocket.h"
#include "helper.h"
#include <string>

using std::string;

#define WM_INITDLG   WM_USER+1
#define WM_CALLBACK WM_USER+2

// CHoldDogDlg �Ի���
class CHoldDogDlg : public CDialogEx, public CTcpPullClientListener
{
// ����
public:
	CHoldDogDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CHoldDogDlg();
// �Ի�������
	enum { IDD = IDD_HOLDDOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnInitDlg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	void KillSer();
	void RunSer();
	int SendTo(string strData);
	CString GetWorkDir();

	void DoRun(string strData);
	BOOL ReadConfig();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();

private:
	virtual EnHandleResult OnConnect(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);

private:
	CString m_ConfigFilePath;
	string m_LocalIP;//����IP
	int    m_TcpPort;//����TCP/IP����˿�

	TPkgInfo m_pkgInfo;
	CTcpPullClientPtr m_Client;
	int m_NoResponseTime;//��������Ӧ����
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
