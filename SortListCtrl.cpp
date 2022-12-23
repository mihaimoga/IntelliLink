/* This file is part of IntelliLink application developed by Mihai MOGA.

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// SortListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliLink.h"
#include "SortListCtrl.h"

// CSortListCtrl

IMPLEMENT_DYNAMIC(CSortListCtrl, CMFCListCtrl)

CSortListCtrl::CSortListCtrl()
{
	m_pLinkSnapshot = NULL;
}

CSortListCtrl::~CSortListCtrl()
{
}

BEGIN_MESSAGE_MAP(CSortListCtrl, CMFCListCtrl)
END_MESSAGE_MAP()

// CSortListCtrl message handlers

int CSortListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	if (m_pLinkSnapshot != NULL)
	{
		CLinkData* pParam1 = (CLinkData*)m_pLinkSnapshot->SelectLink((DWORD)lParam1);
		ASSERT(pParam1);
		CLinkData* pParam2 = (CLinkData*)m_pLinkSnapshot->SelectLink((DWORD)lParam2);
		ASSERT(pParam2);
		switch (iColumn)
		{
			case 0:
			{
				CString strSourceURL1 = pParam1->GetSourceURL();
				CString strSourceURL2 = pParam2->GetSourceURL();
				return strSourceURL1.CompareNoCase(strSourceURL2);
			}
			case 1:
			{
				CString strTargetURL1 = pParam1->GetTargetURL();
				CString strTargetURL2 = pParam2->GetTargetURL();
				return strTargetURL1.CompareNoCase(strTargetURL2);
			}
			case 2:
			{
				CString strURLName1 = pParam1->GetURLName();
				CString strURLName2 = pParam2->GetURLName();
				return strURLName1.CompareNoCase(strURLName2);
			}
			case 3:
			{
				int nPageRank1 = pParam1->GetPageRank();
				int nPageRank2 = pParam2->GetPageRank();
				return (nPageRank2 - nPageRank1);
			}
			case 4:
			{
				BOOL bStatus1 = pParam1->GetStatus();
				BOOL bStatus2 = pParam2->GetStatus();
				return (bStatus2 - bStatus1);
			}
		}
	}
	return 0;
}
