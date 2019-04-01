#pragma once

class COtherProDlg;
class COtherProListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(COtherProListCtrl)

public:
	COtherProListCtrl();
	virtual ~COtherProListCtrl();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

public:
	void InitListCtrl();
	void SetNotifyWnd(COtherProDlg* pdata){ pThis = pdata; }

protected:
	DECLARE_MESSAGE_MAP()

public:
	COtherProDlg* pThis;
};


