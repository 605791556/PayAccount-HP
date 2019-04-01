#pragma once


// CSetAdressDlg 对话框

class CSetAdressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetAdressDlg)

public:
	CSetAdressDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetAdressDlg();

// 对话框数据
	enum { IDD = IDD_SET_ADRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
