/* This file is part of IntelliLink application developed by Mihai MOGA.

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// MainFrame.h : interface of the CMainFrame class
//

#pragma once

#include "LinkView.h"
#include "NTray.h"

class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:
	HICON m_hIcons[3];
	CTrayNotifyIcon m_pTrayIcon;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	BOOL SetStatusBarText(CString strMessage);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton* m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	// CChildView    m_wndView;
	CLinkView*	m_pLinkView;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);

	afx_msg void OnLinkRefresh();
	afx_msg void OnLinkInsert();
	afx_msg void OnLinkModify();
	afx_msg void OnLinkRemove();
	afx_msg void OnUpdateModify(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemove(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};
