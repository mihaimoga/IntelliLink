/* Copyright (C) 2014-2026 Stefan-Mihai MOGA
This file is part of IntelliLink application developed by Stefan-Mihai MOGA.
IntelliLink is an alternative Windows version to Online Link Managers!

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// FileView.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliLink.h"
#include "LinkView.h"
#include "MainFrame.h"
#include "LinkPropertiesDlg.h"

// CLinkView

IMPLEMENT_DYNCREATE(CLinkView, CMFCListView)

	CLinkView::CLinkView()
{
	m_bInitialized = false;
	m_bIsVerified = false;
	m_pMainFrame = nullptr;

	GetListCtrl().m_pLinkSnapshot = &m_pLinkSnapshot;
}

CLinkView::~CLinkView()
{
}

BEGIN_MESSAGE_MAP(CLinkView, CMFCListView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, ID_MFCLISTCTRL, OnDblClickEntry)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CLinkView diagnostics

#ifdef _DEBUG
void CLinkView::AssertValid() const
{
	CMFCListView::AssertValid();
}

#ifndef _WIN32_WCE
void CLinkView::Dump(CDumpContext& dc) const
{
	CMFCListView::Dump(dc);
}
#endif
#endif //_DEBUG

// CLinkView message handlers

void CLinkView::OnInitialUpdate()
{
	CMFCListView::OnInitialUpdate();

	if (!m_bInitialized)
	{
		m_bInitialized = true;

		GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle()
			| LVS_EX_DOUBLEBUFFER |LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		const int nSourceURL = theApp.GetInt(_T("SourceURL"), SOURCE_URL_LENGTH);
		const int nTargetURL = theApp.GetInt(_T("TargetURL"), TARGET_URL_LENGTH);
		const int nURLName = theApp.GetInt(_T("URLName"), URL_NAME_LENGTH);
		const int nPageRank = theApp.GetInt(_T("PageRank"), PAGE_RANK_LENGTH);
		const int nStatus = theApp.GetInt(_T("Status"), STATUS_LENGTH);

		GetListCtrl().InsertColumn(0, _T("Source URL"), LVCFMT_LEFT, nSourceURL);
		GetListCtrl().InsertColumn(1, _T("Target URL"), LVCFMT_LEFT, nTargetURL);
		GetListCtrl().InsertColumn(2, _T("URL Name"), LVCFMT_LEFT, nURLName);
		GetListCtrl().InsertColumn(3, _T("Page Rank"), LVCFMT_CENTER, nPageRank);
		GetListCtrl().InsertColumn(4, _T("Status"), LVCFMT_CENTER, nStatus);

		if (m_pLinkSnapshot.LoadConfig())
			RefreshList();
	}
}

void CLinkView::OnDestroy()
{
	CMFCListView::OnDestroy();
}

void CLinkView::OnSize(UINT nType, int cx, int cy)
{
	CMFCListView::OnSize(nType, cx, cy);
	ResizeListCtrl();
}

void CLinkView::OnDblClickEntry(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pResult != nullptr) *pResult = 0;
	if (pItemActivate->iItem != -1)
	{
		DoubleClickEntry(pItemActivate->iItem);
	}
}

BOOL CLinkView::PreTranslateMessage(MSG* pMsg)
{
	return CMFCListView::PreTranslateMessage(pMsg);
}

void CLinkView::ResizeListCtrl()
{
	HDITEM hdItem = { 0 };
	if (GetListCtrl().GetSafeHwnd() != nullptr)
	{
		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		CMFCHeaderCtrl& pHeaderCtrl = GetListCtrl().GetHeaderCtrl();
		hdItem.mask = HDI_WIDTH;
		if (pHeaderCtrl.GetItem(0, &hdItem))
		{
			const int nSourceURL = hdItem.cxy;
			theApp.WriteInt(_T("SourceURL"), nSourceURL);
			hdItem.mask = HDI_WIDTH;
			if (pHeaderCtrl.GetItem(1, &hdItem))
			{
				const int nTargetURL = hdItem.cxy;
				theApp.WriteInt(_T("TargetURL"), nTargetURL);
				hdItem.mask = HDI_WIDTH;
				if (pHeaderCtrl.GetItem(2, &hdItem))
				{
					const int nURLName = hdItem.cxy;
					theApp.WriteInt(_T("URLName"), nURLName);
					hdItem.mask = HDI_WIDTH;
					if (pHeaderCtrl.GetItem(3, &hdItem))
					{
						const int nPageRank = hdItem.cxy;
						theApp.WriteInt(_T("PageRank"), nPageRank);
						hdItem.mask = HDI_WIDTH;
						if (pHeaderCtrl.GetItem(4, &hdItem))
						{
							const int nStatus = hdItem.cxy;
							theApp.WriteInt(_T("Status"), nStatus);
						}
					}
				}
			}
		}
	}
}

void CLinkView::DoubleClickEntry(int nListItem)
{
	ASSERT(GetListCtrl().m_hWnd != nullptr);
	DWORD dwLinkID = (DWORD)GetListCtrl().GetItemData(nListItem);
	CLinkData* pLinkData = m_pLinkSnapshot.SelectLink(dwLinkID);
	if (pLinkData != nullptr)
	{
		CLinkPropertiesDlg dlgLinkProperties(this);
		dlgLinkProperties.m_strSourceURL = pLinkData->GetSourceURL();
		dlgLinkProperties.m_strTargetURL = pLinkData->GetTargetURL();
		dlgLinkProperties.m_strURLName = pLinkData->GetURLName();
		if (dlgLinkProperties.DoModal() == IDOK)
		{
			pLinkData->SetSourceURL(dlgLinkProperties.m_strSourceURL);
			pLinkData->SetTargetURL(dlgLinkProperties.m_strTargetURL);
			pLinkData->SetURLName(dlgLinkProperties.m_strURLName);

			GetListCtrl().SetItemText(nListItem, 0, dlgLinkProperties.m_strSourceURL);
			GetListCtrl().SetItemText(nListItem, 1, dlgLinkProperties.m_strTargetURL);
			GetListCtrl().SetItemText(nListItem, 2, dlgLinkProperties.m_strURLName);
			GetListCtrl().Sort(0, TRUE, FALSE);
		}
	}
}

bool CLinkView::RefreshList()
{
	CString strPageRank;
	CString strListItem;
	m_bIsVerified = true;
	GetListCtrl().SetRedraw(FALSE);
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (nListItem != -1)
	{
		strListItem = GetListCtrl().GetItemText(nListItem, 0);
	}
	VERIFY(GetListCtrl().DeleteAllItems());
	const int nSize = m_pLinkSnapshot.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_pLinkSnapshot.GetAt(nIndex);
		ASSERT(pLinkData != nullptr);
		strPageRank.Format(_T("%d"), pLinkData->GetPageRank());
		nListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), pLinkData->GetSourceURL());
		GetListCtrl().SetItemText(nListItem, 1, pLinkData->GetTargetURL());
		GetListCtrl().SetItemText(nListItem, 2, pLinkData->GetURLName());
		GetListCtrl().SetItemText(nListItem, 3, strPageRank);
		GetListCtrl().SetItemText(nListItem, 4, pLinkData->IsValidLink() ? _T("Verified") : _T("Error"));
		GetListCtrl().SetItemData(nListItem, pLinkData->GetLinkID());
		if (!pLinkData->IsValidLink())
			m_bIsVerified = false;
	}
	GetListCtrl().Sort(0, TRUE, FALSE);
	nListItem = 0;
	if (!strListItem.IsEmpty())
	{
		for (int nIndex = 0; nIndex < GetListCtrl().GetItemCount(); nIndex++)
		{
			if (strListItem.CompareNoCase(GetListCtrl().GetItemText(nIndex, 0)) == 0)
			{
				nListItem = nIndex;
				break;
			}
		}
	}
	GetListCtrl().SetItemState(nListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().UpdateWindow();
	ResizeListCtrl();
	return true;
}

bool CLinkView::InsertLink()
{
	CLinkPropertiesDlg dlgLinkProperties(this);
	if (dlgLinkProperties.DoModal() == IDOK)
	{
		const DWORD dwLinkID = m_pLinkSnapshot.InsertLink(dlgLinkProperties.m_strSourceURL, dlgLinkProperties.m_strTargetURL, dlgLinkProperties.m_strURLName, 0, FALSE);
		const int nListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), dlgLinkProperties.m_strSourceURL);
		GetListCtrl().SetItemText(nListItem, 1, dlgLinkProperties.m_strTargetURL);
		GetListCtrl().SetItemText(nListItem, 2, dlgLinkProperties.m_strURLName);
		GetListCtrl().SetItemText(nListItem, 4, _T("Unknown"));
		GetListCtrl().SetItemData(nListItem, dwLinkID);
		GetListCtrl().Sort(0, TRUE, FALSE);
	}
	return true;
}

bool CLinkView::ModifyLink()
{
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (nListItem != -1)
	{
		DoubleClickEntry(nListItem);
		return true;
	}
	return false;
}

bool CLinkView::DeleteLink()
{
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (nListItem != -1)
	{
		DWORD dwLinkID = (DWORD)GetListCtrl().GetItemData(nListItem);
		if (m_pLinkSnapshot.DeleteLink(dwLinkID))
		{
			GetListCtrl().DeleteItem(nListItem);
			GetListCtrl().Sort(0, TRUE, FALSE);
			return true;
		}
	}
	return false;
}

bool CLinkView::IsSelected()
{
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	return (nListItem != -1);
}

bool CLinkView::IsVerified()
{
	return m_bIsVerified;
}

bool CLinkView::LoadConfig()
{
	return m_pLinkSnapshot.LoadConfig();
}

bool CLinkView::SaveConfig()
{
	return m_pLinkSnapshot.SaveConfig();
}
