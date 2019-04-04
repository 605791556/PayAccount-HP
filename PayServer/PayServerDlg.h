
// PayServerDlg.h : 头文件
//

#pragma once
#include <string>
#include "afxwin.h"
#include "afxcmn.h"
#include "HPSocket.h"
#include "helper.h"

using std::string;

#define WM_SHOWTASK (WM_USER + 1)

enum SOCK_MSG
{
	SOCK_MSG_OK,
	SOCK_ERROR_CREATESOCK,
	SOCK_ERROR_BIND,
	SOCK_ERROR_CREATEEVENT,
	SOCK_MSG_CLOSE,
	SOCK_MSG_ONE_LINK,
	SOCK_MSG_ONE_CLOSE,
	SOCK_MSG_TIMEOUT,
	SOCK_MSG_MAX
};


// CPayServerDlg 对话框
class CPayServerDlg : public CDialogEx, public CTcpPullServerListener
{
// 构造
public:
	CPayServerDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPayServerDlg();
// 对话框数据
	enum { IDD = IDD_PAYSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	LRESULT OnShowTask(WPARAM wParam, LPARAM lParam) ;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnShow();
	afx_msg void OnClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnSockMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, SOCKET soClient);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);

private:
	static TPkgInfo* FindPkgInfo(ITcpServer* pSender, CONNID dwConnID);
	static void RemovePkgInfo(ITcpServer* pSender, CONNID dwConnID);

public:
	void DoRun(string strData,Json::Value& js,TPkgInfo* pInfo = NULL);
	int SendTo(CONNID clientID,Json::Value js);
	void AddString(CString str);
	void InitListCtrl();
	void InitChechBox();
	void Hide();
	void KillDog();
	BOOL RunDog();

public:
	CTcpPullServerPtr m_Server;

public:
	NOTIFYICONDATA m_nid;
	afx_msg void OnDestroy();
	CListBox m_ListBox;
	CListCtrl m_listCtrl;
	CStatic m_staIP;
	HANDLE m_mutex;
	HANDLE m_hSetStart;
	BOOL   m_bExit;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnHide();
	CButton m_ckRun;
	afx_msg void OnBnClickedCkStartrun();
	CStatic m_NowSta;
};
