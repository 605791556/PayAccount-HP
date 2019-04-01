#include "stdafx.h"
#include "..\common\EditSet.h"


BEGIN_MESSAGE_MAP(CEditSet, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

CEditSet::CEditSet()
{
	m_type = EDIT_TYPE_ALL;
	m_maxLen = 20;
}

CEditSet::~CEditSet()
{
}

void CEditSet::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar == 0x08) || (nChar == 0x10))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	CString str;
	GetWindowText(str);
	int len = str.GetLength();
	if (len>=m_maxLen)
	{
		return;
	}

	switch (m_type)
	{
		case EDIT_TYPE_NUMBER:
		case EDIT_TYPE_NUMBER_X:
		{
			if ((nChar >= '0' && nChar <= '9'))
			{
				CEdit::OnChar(nChar, nRepCnt, nFlags);
			}
			else if (nChar == 'X' && m_type == EDIT_TYPE_NUMBER_X)
			{
				CEdit::OnChar(nChar, nRepCnt, nFlags);
			}
		}
		    break;
		case EDIT_TYPE_FLOAT:
		{
			if (nChar == '.')
			{
				CString str;
				// ��ȡԭ���༭���е��ַ���
				GetWindowText(str);
				//��ԭ�����ַ������Ѿ���һ��С����,�򲻽�С��������,��֤�����ֻ������һ��С����
				if (str!="" && str.Find('.') == -1)
				{
					CEdit::OnChar(nChar, nRepCnt, nFlags);
				}
			}
			//��������
			else if ((nChar >= '0' && nChar <= '9'))
			{
				CEdit::OnChar(nChar, nRepCnt, nFlags);
			}
		}
			break;
		case EDIT_TYPE_NO_CHN:
		{
			int nAscii = nChar;
			if ((nAscii >= 33 && nAscii <= 126))
			{
				CEdit::OnChar(nChar, nRepCnt, nFlags);
			}
		}
			break;
		case EDIT_TYPE_ALL:
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
		    break;
	}
}
