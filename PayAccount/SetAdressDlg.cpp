// SetAdressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PayAccount.h"
#include "SetAdressDlg.h"
#include "afxdialogex.h"
#include "..\common\globle.h"
#include <iostream>
#include <fstream>

// CSetAdressDlg �Ի���

IMPLEMENT_DYNAMIC(CSetAdressDlg, CDialogEx)

CSetAdressDlg::CSetAdressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetAdressDlg::IDD, pParent)
{

}

CSetAdressDlg::~CSetAdressDlg()
{
}

void CSetAdressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetAdressDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetAdressDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetAdressDlg ��Ϣ�������


BOOL CSetAdressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	USES_CONVERSION;
	SetDlgItemText(IDC_EDIT_ADRESS,A2W(g_Globle.m_LocalIP.c_str()));
	SetDlgItemInt(IDC_EDIT_PORT,g_Globle.m_TcpPort);
	SetDlgItemText(IDC_EDIT_NAME,g_Globle.m_strName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSetAdressDlg::OnBnClickedOk()
{
	CString strPort = L"";
	CString strAdress = L"";
	CString strName = L"";

	GetDlgItemText(IDC_EDIT_ADRESS,strAdress);
	GetDlgItemText(IDC_EDIT_PORT,strPort);
	GetDlgItemText(IDC_EDIT_NAME,strName);
	if (strAdress.IsEmpty() || strPort.IsEmpty())
	{
		MessageBox(L"��Ϣ����Ϊ�գ����������룡",L"��ʾ");
		return;
	}
	else
	{
		USES_CONVERSION;

		fstream _file;
		_file.open(g_Globle.m_ConfigFilePath,ios::in);
		if(!_file)
		{
			CString str;
			str.Format(L"�Ҳ��������ļ�: %s ��",g_Globle.m_ConfigFilePath);
			MessageBox(str,L"error");
			return;
		}
		else
		{
			WritePrivateProfileString(L"SOCKET",L"ip",strAdress,g_Globle.m_ConfigFilePath);
			WritePrivateProfileString(L"SOCKET",L"port",strPort,g_Globle.m_ConfigFilePath);
			WritePrivateProfileString(L"SOCKET",L"name",strName,g_Globle.m_ConfigFilePath);

			g_Globle.m_LocalIP = W2A(strAdress);
			g_Globle.m_TcpPort = _ttoi(strPort);
			g_Globle.m_strName = strName;
		}
		_file.close();
		MessageBox(L"����ɹ������������г���",L"��ʾ");
	}

	CDialogEx::OnOK();
}
