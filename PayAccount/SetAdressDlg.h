#pragma once


// CSetAdressDlg �Ի���

class CSetAdressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetAdressDlg)

public:
	CSetAdressDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetAdressDlg();

// �Ի�������
	enum { IDD = IDD_SET_ADRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
