#pragma once
#include <vector>
#include "..\common\EditSet.h"
using std::vector;


class CProjectMngDlg;
class CProMngListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CProMngListCtrl)

public:
	CProMngListCtrl();
	virtual ~CProMngListCtrl();
	void InitListCtrl();
	void SetNotifyWnd(CProjectMngDlg* pdata){ pThis = pdata; }
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	DECLARE_MESSAGE_MAP()

public:
	CProjectMngDlg* pThis;
	
};


