#pragma once
#include "afxwin.h"
#include "StaffMngDlg.h"
#include "..\common\EditSet.h"


#define  WM_ADDSTAFF_CALL WM_USER+601

// CAddStaffDlg �Ի���

class CAddStaffDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddStaffDlg)

public:
	CAddStaffDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CAddStaffDlg(CStaffMngDlg* dlg);
	virtual ~CAddStaffDlg();

// �Ի�������
	enum { IDD = IDD_ADD_STAFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCallBack(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	bool m_bAdd;
	int m_ndex;//��Ϊ�޸�ʱ������m_vet������s
	CString strOldIdcard;//�޸�ְ������ְ���޸�ǰ�����֤��
	CComboBox m_ComBoSex;
	CStaffMngDlg* m_dlg;
	CEditSet m_edit_number;
	CEditSet m_edit_number_x;
	CEditSet m_edit_daypay;

	afx_msg void OnBnClickedOk();
	void SetModifyInit();
	void SetNotifyWnd(CStaffMngDlg* dlg);
	void SendToJudgeStaff(CString strIdcard);
	void SendToAddStaff();
	void SendToMdfStaff(CString strName,CString strSex,int age,CString strStaffID, CString strIdcard,CString strTel,STAFF_TYPE type,int sort,double fDaypay);
	CComboBox m_comboType;
};
