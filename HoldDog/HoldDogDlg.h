
// HoldDogDlg.h : 头文件
//

#pragma once
#include "HPSocket.h"
#include "helper.h"
#include <string>

using std::string;

#define WM_INITDLG   WM_USER+1
#define WM_CALLBACK WM_USER+2

// CHoldDogDlg 对话框
class CHoldDogDlg : public CDialogEx, public CTcpPullClientListener
{
// 构造
public:
	CHoldDogDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CHoldDogDlg();
// 对话框数据
	enum { IDD = IDD_HOLDDOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	string m_LocalIP;//本地IP
	int    m_TcpPort;//本机TCP/IP服务端口

	TPkgInfo m_pkgInfo;
	CTcpPullClientPtr m_Client;
	int m_NoResponseTime;//服务无响应次数
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
