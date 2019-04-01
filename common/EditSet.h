#pragma once
#include "afxwin.h"
#include "..\common\globle.h"

class CEditSet :public CEdit
{
public:
	CEditSet();
	~CEditSet();

protected:

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

public:
	EDIT_TYPE m_type;
	int m_maxLen;
};

