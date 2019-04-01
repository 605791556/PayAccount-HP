#pragma once
#include <vector>

using std::vector;

struct DTL_LINE
{
	int nItem;

	DTL_LINE()
	{
	
	}
};

class CDetailDlg;
class CDtlListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CDtlListCtrl)

public:
	CDtlListCtrl();
	virtual ~CDtlListCtrl();

	void SetNotifyWnd(CDetailDlg* pdata){ pThis = pdata; }
	afx_msg LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()

public:
	vector<DTL_LINE> m_ListLine;
	long m_aDex;
	CDetailDlg* pThis;
	
};


