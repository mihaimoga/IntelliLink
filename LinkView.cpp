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

// Constructor: Initializes member variables and sets up the link snapshot reference
CLinkView::CLinkView()
{
	m_bInitialized = false;
	m_bIsVerified = false;
	m_pMainFrame = nullptr;

	// Associate the link snapshot with the list control
	GetListCtrl().m_pLinkSnapshot = &m_pLinkSnapshot;
}

// Destructor
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

// Called when the view is first initialized
// Sets up the list control columns and loads saved configuration
void CLinkView::OnInitialUpdate()
{
	CMFCListView::OnInitialUpdate();

	// Ensure initialization only happens once
	if (!m_bInitialized)
	{
		m_bInitialized = true;

		// Configure list control extended styles for better appearance and functionality
		GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle()
			| LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		// Retrieve saved column widths from application settings
		const int nSourceURL = theApp.GetInt(_T("SourceURL"), SOURCE_URL_LENGTH);
		const int nTargetURL = theApp.GetInt(_T("TargetURL"), TARGET_URL_LENGTH);
		const int nURLName = theApp.GetInt(_T("URLName"), URL_NAME_LENGTH);
		const int nPageRank = theApp.GetInt(_T("PageRank"), PAGE_RANK_LENGTH);
		const int nStatus = theApp.GetInt(_T("Status"), STATUS_LENGTH);

		// Create columns with saved or default widths
		GetListCtrl().InsertColumn(0, _T("Source URL"), LVCFMT_LEFT, nSourceURL);
		GetListCtrl().InsertColumn(1, _T("Target URL"), LVCFMT_LEFT, nTargetURL);
		GetListCtrl().InsertColumn(2, _T("URL Name"), LVCFMT_LEFT, nURLName);
		GetListCtrl().InsertColumn(3, _T("Page Rank"), LVCFMT_CENTER, nPageRank);
		GetListCtrl().InsertColumn(4, _T("Status"), LVCFMT_CENTER, nStatus);

		// Load link data from configuration and populate the list
		if (m_pLinkSnapshot.LoadConfig())
			RefreshList();
	}
}

// Cleanup when the view is destroyed
void CLinkView::OnDestroy()
{
	CMFCListView::OnDestroy();
}

// Handle window resize events to adjust column widths proportionally
void CLinkView::OnSize(UINT nType, int cx, int cy)
{
	CMFCListView::OnSize(nType, cx, cy);
	ResizeListCtrl();
}

// Handle double-click events on list items to open properties dialog
void CLinkView::OnDblClickEntry(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pResult != nullptr) *pResult = 0;

	// Open properties dialog for the clicked item
	if (pItemActivate->iItem != -1)
	{
		DoubleClickEntry(pItemActivate->iItem);
	}
}

// Pre-process window messages
BOOL CLinkView::PreTranslateMessage(MSG* pMsg)
{
	return CMFCListView::PreTranslateMessage(pMsg);
}

// Resize list control columns proportionally based on client area width
// First three columns get equal width (1/4 each), remaining space split between last two columns
void CLinkView::ResizeListCtrl()
{
	HDITEM hdItem = { 0 };
	if (GetListCtrl().GetSafeHwnd() != nullptr)
	{
		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		CMFCHeaderCtrl& pHeaderCtrl = GetListCtrl().GetHeaderCtrl();

		// Calculate Source URL column width (1/4 of available space)
		const int nSourceURL = (rectClient.Width() - GetSystemMetrics(SM_CXVSCROLL)) / 4;
		theApp.WriteInt(_T("SourceURL"), nSourceURL);
		hdItem.cxy = nSourceURL;
		hdItem.mask = HDI_WIDTH;
		if (pHeaderCtrl.SetItem(0, &hdItem))
		{
			// Target URL column gets same width as Source URL
			const int nTargetURL = nSourceURL;
			theApp.WriteInt(_T("TargetURL"), nTargetURL);
			hdItem.cxy = nTargetURL;
			hdItem.mask = HDI_WIDTH;
			if (pHeaderCtrl.SetItem(1, &hdItem))
			{
				// URL Name column gets same width as Source URL
				const int nURLName = nSourceURL;
				theApp.WriteInt(_T("URLName"), nURLName);
				hdItem.cxy = nURLName;
				hdItem.mask = HDI_WIDTH;
				if (pHeaderCtrl.SetItem(2, &hdItem))
				{
					// Page Rank column gets half of remaining space
					const int nPageRank = ((rectClient.Width() - GetSystemMetrics(SM_CXVSCROLL)) - (nSourceURL + nTargetURL + nURLName)) / 2;
					theApp.WriteInt(_T("PageRank"), nPageRank);
					hdItem.cxy = nPageRank;
					hdItem.mask = HDI_WIDTH;
					if (pHeaderCtrl.SetItem(3, &hdItem))
					{
						// Status column gets the rest of available space
						const int nStatus = (rectClient.Width() - GetSystemMetrics(SM_CXVSCROLL)) - (nSourceURL + nTargetURL + nURLName + nPageRank);
						theApp.WriteInt(_T("Status"), nStatus);
						hdItem.cxy = nStatus;
						hdItem.mask = HDI_WIDTH;
						if (pHeaderCtrl.SetItem(4, &hdItem))
						{
							// Refresh the list control display
							GetListCtrl().Invalidate();
							GetListCtrl().UpdateWindow();
						}
					}
				}
			}
		}
	}
}

// Display and handle the link properties dialog for the specified list item
void CLinkView::DoubleClickEntry(int nListItem)
{
	ASSERT(GetListCtrl().m_hWnd != nullptr);

	// Get link ID from list item data
	DWORD dwLinkID = (DWORD)GetListCtrl().GetItemData(nListItem);
	CLinkData* pLinkData = m_pLinkSnapshot.SelectLink(dwLinkID);

	if (pLinkData != nullptr)
	{
		// Populate dialog with current link data
		CLinkPropertiesDlg dlgLinkProperties(this);
		dlgLinkProperties.m_strSourceURL = pLinkData->GetSourceURL();
		dlgLinkProperties.m_strTargetURL = pLinkData->GetTargetURL();
		dlgLinkProperties.m_strURLName = pLinkData->GetURLName();

		// If user clicks OK, update the link data and list display
		if (dlgLinkProperties.DoModal() == IDOK)
		{
			pLinkData->SetSourceURL(dlgLinkProperties.m_strSourceURL);
			pLinkData->SetTargetURL(dlgLinkProperties.m_strTargetURL);
			pLinkData->SetURLName(dlgLinkProperties.m_strURLName);

			// Update list item text to reflect changes
			GetListCtrl().SetItemText(nListItem, 0, dlgLinkProperties.m_strSourceURL);
			GetListCtrl().SetItemText(nListItem, 1, dlgLinkProperties.m_strTargetURL);
			GetListCtrl().SetItemText(nListItem, 2, dlgLinkProperties.m_strURLName);
			GetListCtrl().Sort(0, TRUE, FALSE);
		}
	}
}

// Refresh the entire list control with data from the link snapshot
// Preserves the currently selected item if possible
bool CLinkView::RefreshList()
{
	CString strPageRank;
	CString strListItem;
	m_bIsVerified = true;

	// Disable redrawing during update for better performance
	GetListCtrl().SetRedraw(FALSE);

	// Remember the currently selected item's text
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (nListItem != -1)
	{
		strListItem = GetListCtrl().GetItemText(nListItem, 0);
	}

	// Clear all existing items
	VERIFY(GetListCtrl().DeleteAllItems());

	// Populate list with all links from the snapshot
	const int nSize = m_pLinkSnapshot.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_pLinkSnapshot.GetAt(nIndex);
		ASSERT(pLinkData != nullptr);

		// Format page rank as string
		strPageRank.Format(_T("%d"), pLinkData->GetPageRank());

		// Insert new list item with all link data
		nListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), pLinkData->GetSourceURL());
		GetListCtrl().SetItemText(nListItem, 1, pLinkData->GetTargetURL());
		GetListCtrl().SetItemText(nListItem, 2, pLinkData->GetURLName());
		GetListCtrl().SetItemText(nListItem, 3, strPageRank);
		GetListCtrl().SetItemText(nListItem, 4, pLinkData->IsValidLink() ? _T("Verified") : _T("Error"));
		GetListCtrl().SetItemData(nListItem, pLinkData->GetLinkID());

		// Track if any links have errors
		if (!pLinkData->IsValidLink())
			m_bIsVerified = false;
	}

	// Sort by first column (Source URL)
	GetListCtrl().Sort(0, TRUE, FALSE);

	// Restore selection to previously selected item, or default to first item
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

	// Set selection and focus
	GetListCtrl().SetItemState(nListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	// Re-enable redrawing and update display
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().UpdateWindow();
	ResizeListCtrl();
	return true;
}

// Display dialog to insert a new link into the list
bool CLinkView::InsertLink()
{
	CLinkPropertiesDlg dlgLinkProperties(this);

	// If user enters link data and clicks OK, add it to the snapshot and list
	if (dlgLinkProperties.DoModal() == IDOK)
	{
		// Add link to snapshot with unknown status (not yet verified)
		const DWORD dwLinkID = m_pLinkSnapshot.InsertLink(dlgLinkProperties.m_strSourceURL, dlgLinkProperties.m_strTargetURL, dlgLinkProperties.m_strURLName, 0, FALSE);

		// Add link to list control
		const int nListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), dlgLinkProperties.m_strSourceURL);
		GetListCtrl().SetItemText(nListItem, 1, dlgLinkProperties.m_strTargetURL);
		GetListCtrl().SetItemText(nListItem, 2, dlgLinkProperties.m_strURLName);
		GetListCtrl().SetItemText(nListItem, 4, _T("Unknown"));
		GetListCtrl().SetItemData(nListItem, dwLinkID);
		GetListCtrl().Sort(0, TRUE, FALSE);
	}
	return true;
}

// Modify the currently selected link
bool CLinkView::ModifyLink()
{
	// Get the selected item
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (nListItem != -1)
	{
		// Open properties dialog for selected item
		DoubleClickEntry(nListItem);
		return true;
	}
	return false;
}

// Delete the currently selected link
bool CLinkView::DeleteLink()
{
	// Get the selected item
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (nListItem != -1)
	{
		// Get link ID and remove from snapshot
		DWORD dwLinkID = (DWORD)GetListCtrl().GetItemData(nListItem);
		if (m_pLinkSnapshot.DeleteLink(dwLinkID))
		{
			// Remove from list control and re-sort
			GetListCtrl().DeleteItem(nListItem);
			GetListCtrl().Sort(0, TRUE, FALSE);
			return true;
		}
	}
	return false;
}

// Check if any list item is currently selected
bool CLinkView::IsSelected()
{
	int nListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	return (nListItem != -1);
}

// Check if all links in the list have been verified successfully
bool CLinkView::IsVerified()
{
	return m_bIsVerified;
}

// Load link configuration from storage
bool CLinkView::LoadConfig()
{
	return m_pLinkSnapshot.LoadConfig();
}

// Save link configuration to storage
bool CLinkView::SaveConfig()
{
	return m_pLinkSnapshot.SaveConfig();
}
