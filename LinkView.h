/* This file is part of IntelliLink application developed by Mihai MOGA.

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#pragma once

#include "MFCListView.h"
#include "LinkList.h"

class CMainFrame;

// CLinkView view

class CLinkView : public CMFCListView
{
	DECLARE_DYNCREATE(CLinkView)

public:
	CLinkView();           // protected constructor used by dynamic creation
	virtual ~CLinkView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblClickEntry(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	void ResizeListCtrl();
	void DoubleClickEntry(int nListItem);
	BOOL RefreshList();
	BOOL InsertLink();
	BOOL ModifyLink();
	BOOL DeleteLink();
	BOOL IsSelected();

	BOOL LoadConfig();
	BOOL SaveConfig();

public:
	BOOL m_bInitialized;
	CMainFrame* m_pMainFrame;
	CLinkSnapshot m_pLinkSnapshot;

	DECLARE_MESSAGE_MAP()
};
