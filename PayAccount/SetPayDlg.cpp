// ProjectMngDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "SetPayDlg.h"
#include "DaiNumberPayDlg.h"
#include "ZheYeDlg.h"
#include "OtherProDlg.h"
#include "DianYePayDlg.h"


// CSetPayDlg 对话框

IMPLEMENT_DYNAMIC(CSetPayDlg, CDialog)

CSetPayDlg::CSetPayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetPayDlg::IDD, pParent)
{
	m_pDaiNumPayDlg = NULL;
	m_pZheYeDlg = NULL;
	m_pOtherProDlg = NULL;
	m_pDianYePayDlg = NULL;
}

CSetPayDlg::~CSetPayDlg()
{
	if (m_pZheYeDlg)
	{
		delete m_pZheYeDlg;
		m_pZheYeDlg = NULL;
	}
	if (m_pDaiNumPayDlg)
	{
		delete m_pDaiNumPayDlg;
		m_pDaiNumPayDlg = NULL;
	}
	if (m_pOtherProDlg)
	{
		delete m_pOtherProDlg;
		m_pOtherProDlg = NULL;
	}
	if (m_pDianYePayDlg)
	{
		delete m_pDianYePayDlg;
		m_pDianYePayDlg = NULL;
	}
}

void CSetPayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}


BEGIN_MESSAGE_MAP(CSetPayDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSetPayDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CSetPayDlg 消息处理程序
BOOL CSetPayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化tabctrl
	InitTabCtrl();

	
	return TRUE;
}

void CSetPayDlg::InitTabCtrl()
{
	m_pDaiNumPayDlg = new CDaiNumberPayDlg;
	m_pZheYeDlg = new CZheYeDlg;
	m_pDianYePayDlg = new CDianYePayDlg;
	m_pOtherProDlg = new COtherProDlg;

	m_tabCtrl.InsertItem(0,  L"胶订");
	m_tabCtrl.InsertItem(1,  L"折页");
	m_tabCtrl.InsertItem(2,  L"点页");
	m_tabCtrl.InsertItem(3,  L"其它");

	m_pDaiNumPayDlg->Create(IDD_DAI_BUMBER, &m_tabCtrl);
	m_pZheYeDlg->Create(IDD_ZHEYE, &m_tabCtrl);
	m_pOtherProDlg->Create(IDD_OTHER_PRO, &m_tabCtrl);
	m_pDianYePayDlg->Create(IDD_DIANYE, &m_tabCtrl);

	CRect rc;
	m_tabCtrl.GetClientRect(rc);
	rc.top += 30;
	rc.bottom -= 2;
	rc.left += 2;
	rc.right -= 5;
	m_pDaiNumPayDlg->MoveWindow(&rc);
	m_pZheYeDlg->MoveWindow(&rc);
	m_pDianYePayDlg->MoveWindow(&rc);
	m_pOtherProDlg->MoveWindow(&rc);

	m_TabDlgs[0] = m_pDaiNumPayDlg;
	m_TabDlgs[1] = m_pZheYeDlg;
	m_TabDlgs[2] = m_pDianYePayDlg;
	m_TabDlgs[3] = m_pOtherProDlg;

	m_TabDlgs[0]->ShowWindow(SW_SHOW);
	m_TabDlgs[1]->ShowWindow(SW_HIDE);
	m_TabDlgs[2]->ShowWindow(SW_HIDE);
	m_TabDlgs[3]->ShowWindow(SW_HIDE);

	m_TabDlgs[0]->UpdateDlg();
	m_TabSelIndex = 0;
}

void CSetPayDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	m_TabDlgs[m_TabSelIndex]->ShowWindow(SW_HIDE);
	m_TabSelIndex = m_tabCtrl.GetCurSel();
	m_TabDlgs[m_TabSelIndex]->ShowWindow(SW_SHOW);
	m_TabDlgs[m_TabSelIndex]->UpdateDlg();
	*pResult = 0;
}

void CSetPayDlg::UpdateDlg()
{

}
