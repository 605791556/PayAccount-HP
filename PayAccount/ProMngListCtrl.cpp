// MyListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "PayAccount.h"
#include "ProMngListCtrl.h"
#include "ProjectMngDlg.h"


// CProMngListCtrl

IMPLEMENT_DYNAMIC(CProMngListCtrl, CListCtrl)

CProMngListCtrl::CProMngListCtrl()
{
	pThis = NULL;
}

CProMngListCtrl::~CProMngListCtrl()
{
}

BEGIN_MESSAGE_MAP(CProMngListCtrl, CListCtrl)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CProMngListCtrl::InitListCtrl()
{
	TCHAR rgtsz[5][10] = { _T("序号"), _T("项目") , _T("做工范围"), _T("工人填入"), _T("排列顺序")};

	LV_COLUMN lvcolumn;
	CRect rect;
	GetWindowRect(&rect);
	for (int i=0;i<5;i++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT
			| LVCF_WIDTH | LVCF_ORDER;
		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.pszText = rgtsz[i];
		lvcolumn.iSubItem = i;
		lvcolumn.iOrder = i;
		if (i==0)
		{
			lvcolumn.cx = 50;
		}
		else if (i==1)
		{
			lvcolumn.cx = 100;
		}
		else
		{
			lvcolumn.cx = (rect.Width()-150)/3-8;
		}
		InsertColumn(i, &lvcolumn);
	}
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	SetBkColor(RGB(102 ,205 ,170));
	SetTextBkColor(RGB(102 ,205, 170));

	CImageList imagelist;
	imagelist.Create(1, 25, ILC_COLOR, 1, 1);
	SetImageList(&imagelist, LVSIL_SMALL);
}

void CProMngListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	pThis->m_EditIndex.ShowWindow(SW_HIDE);
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}
void CProMngListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	pThis->m_EditIndex.ShowWindow(SW_HIDE);
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}