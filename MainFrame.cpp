/* This file is part of IntelliLink application developed by Stefan-Mihai MOGA.

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "IntelliLink.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_TRAYNOTIFY WM_USER + 100

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CFrameWndEx::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CFrameWndEx::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CFrameWndEx::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CFrameWndEx::OnHelpFinder)
	// Default commands
	ON_COMMAND(ID_REFRESH, &CMainFrame::OnLinkRefresh)
	ON_COMMAND(ID_INSERT, &CMainFrame::OnLinkInsert)
	ON_COMMAND(ID_MODIFY, &CMainFrame::OnLinkModify)
	ON_COMMAND(ID_REMOVE, &CMainFrame::OnLinkRemove)
	ON_UPDATE_COMMAND_UI(ID_MODIFY, &CMainFrame::OnUpdateModify)
	ON_UPDATE_COMMAND_UI(ID_REMOVE, &CMainFrame::OnUpdateRemove)
	ON_COMMAND(ID_SHOW_APPLICATION, &CMainFrame::OnShowApplication)
	ON_COMMAND(ID_HIDE_APPLICATION, &CMainFrame::OnHideApplication)
	ON_MESSAGE(WM_TRAYNOTIFY, OnTrayNotification)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND(ID_STARTUP_APPS, &CMainFrame::OnStartupApps)
	ON_COMMAND(ID_WEBSITE_REVIEW, &CMainFrame::OnWebsiteReview)
	ON_COMMAND(ID_WEBMASTER_TOOLS, &CMainFrame::OnWebmasterTools)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_MainButton = nullptr;
	m_pLinkView = nullptr;
	m_hIcons[0] = CTrayNotifyIcon::LoadIcon(IDR_MAINFRAME);
	m_hIcons[1] = CTrayNotifyIcon::LoadIcon(IDR_HAPPY);
	m_hIcons[2] = CTrayNotifyIcon::LoadIcon(IDR_SAD);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// set the visual style to be used the by the visual manager
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);

	/* create a view to occupy the client area of the frame
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}*/

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	m_MainButton = new CMFCRibbonApplicationButton;
	m_MainButton->SetVisible(FALSE);
	m_wndRibbonBar.SetApplicationButton(m_MainButton, CSize());

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane;
	bNameValid = strTitlePane.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(
		ID_STATUSBAR_PANE1, strTitlePane, TRUE, nullptr,
		_T("012345678901234567890123456789012345678901234567890123456789")), strTitlePane);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// if (!m_pTrayIcon.Create(this, IDR_TRAYPOPUP, _T("Static Tray Icon"), _T("Demo Tray Application"), _T("Static Tray Icon"), 10, CTrayNotifyIcon::User, m_hIcons[0], WM_TRAYNOTIFY))
	if (!m_pTrayIcon.Create(this, IDR_TRAYPOPUP, _T("IntelliLink"), m_hIcons[0], WM_TRAYNOTIFY))
	{
		AfxMessageBox(_T("Failed to create tray icon"), MB_OK | MB_ICONSTOP);
		return -1;
	}
	m_nTimerID = SetTimer(1, TIMER_INTERVAL, nullptr);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CCreateContext pLinkContext;
	pLinkContext.m_pCurrentDoc = nullptr;
	pLinkContext.m_pCurrentFrame = this;
	pLinkContext.m_pLastView = nullptr;
	pLinkContext.m_pNewDocTemplate = nullptr;
	pLinkContext.m_pNewViewClass = RUNTIME_CLASS(CLinkView);

	if ((m_pLinkView = (CLinkView*) CreateView(&pLinkContext, AFX_IDW_PANE_FIRST)) != nullptr)
	{
		m_pLinkView->ShowWindow(SW_SHOW);
		m_pLinkView->OnInitialUpdate();

		m_pLinkView->m_pMainFrame = this;
	}

	return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDestroy()
{
	m_pTrayIcon.Hide();

	CFrameWndEx::OnDestroy();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

BOOL CMainFrame::SetStatusBarText(CString strMessage)
{
	if (m_wndStatusBar.GetSafeHwnd() != nullptr)
	{
		m_wndStatusBar.GetElement(0)->SetText(strMessage);
		m_wndStatusBar.Invalidate();
		m_wndStatusBar.UpdateWindow();
		return TRUE;
	}
	return FALSE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_pLinkView->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_pLinkView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	//Delegate all the work back to the default implementation in CTrayNotifyIcon.
	m_pTrayIcon.OnTrayNotification(wParam, lParam);
	return 0L;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == m_nTimerID)
	{
		VERIFY(KillTimer(m_nTimerID));
		OnLinkRefresh();
		m_nTimerID = SetTimer(1, TIMER_INTERVAL, nullptr);
	}

	CFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::OnLinkRefresh()
{
	m_pTrayIcon.SetIcon(m_hIcons[0]);
	ASSERT_VALID(m_pLinkView);
	VERIFY(m_pLinkView->RefreshList());
	m_pTrayIcon.SetIcon(m_pLinkView->IsVerified() ? m_hIcons[1] : m_hIcons[2]);
}

void CMainFrame::OnLinkInsert()
{
	ASSERT_VALID(m_pLinkView);
	VERIFY(m_pLinkView->InsertLink());
	VERIFY(m_pLinkView->SaveConfig());
}

void CMainFrame::OnLinkModify()
{
	ASSERT_VALID(m_pLinkView);
	VERIFY(m_pLinkView->ModifyLink());
	VERIFY(m_pLinkView->SaveConfig());
}

void CMainFrame::OnLinkRemove()
{
	ASSERT_VALID(m_pLinkView);
	VERIFY(m_pLinkView->DeleteLink());
	VERIFY(m_pLinkView->SaveConfig());
}

void CMainFrame::OnUpdateModify(CCmdUI* pCmdUI)
{
	ASSERT_VALID(m_pLinkView);
	pCmdUI->Enable(m_pLinkView->IsSelected());
}

void CMainFrame::OnUpdateRemove(CCmdUI* pCmdUI)
{
	ASSERT_VALID(m_pLinkView);
	pCmdUI->Enable(m_pLinkView->IsSelected());
}

void CMainFrame::OnShowApplication()
{
	// The one and only window has been initialized, so show and update it
	ShowWindow(SW_SHOW);
	UpdateWindow();
}

void CMainFrame::OnHideApplication()
{
	// The one and only window has been initialized, so hide and update it
	ShowWindow(SW_HIDE);
	UpdateWindow();
}

void CMainFrame::OnStartupApps()
{
	HKEY regValue;
	TCHAR lpszApplicationBuffer[MAX_PATH + 1] = { 0, };
	if (GetModuleFileName(nullptr, lpszApplicationBuffer, MAX_PATH) > 0)
	{
		// const DWORD nApplicationLength = _tcslen(lpszApplicationBuffer) * sizeof(TCHAR);
		if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &regValue) == ERROR_SUCCESS)
		{
			if (RegDeleteValue(regValue, _T("IntelliLink")) == ERROR_SUCCESS)
			{
				MessageBox(_T("This application has been removed successfully from Startup Apps!"), nullptr, MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				std::wstring quoted(_T("\""));
				quoted += lpszApplicationBuffer;
				quoted += _T("\"");
				const size_t length = quoted.length() * sizeof(TCHAR);
				if (RegSetValueEx(regValue, _T("IntelliLink"), 0, REG_SZ, (LPBYTE)quoted.c_str(), (DWORD)length) == ERROR_SUCCESS)
				{
					MessageBox(_T("This application has been added successfully to Startup Apps!"), nullptr, MB_OK | MB_ICONINFORMATION);
				}
			}
			RegCloseKey(regValue);
		}
	}
}

void CMainFrame::OnWebsiteReview()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.website-review.ro/"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnWebmasterTools()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.webmaster-tools.ro/"), nullptr, nullptr, SW_SHOW);
}
