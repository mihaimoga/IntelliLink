/* This file is part of IntelliLink application developed by Stefan-Mihai MOGA.

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliLink.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "IntelliLink.h"
#include "MainFrame.h"

#include "VersionInfo.h"
#include "HLinkCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIntelliLinkApp

BEGIN_MESSAGE_MAP(CIntelliLinkApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIntelliLinkApp::OnAppAbout)
END_MESSAGE_MAP()

// CIntelliLinkApp construction

CIntelliLinkApp::CIntelliLinkApp() : m_pInstanceChecker(_T("IntelliLink"))
{
	EnableHtmlHelp();

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("IntelliLink.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CIntelliLinkApp object

CIntelliLinkApp theApp;

// CIntelliLinkApp initialization

BOOL CIntelliLinkApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	AfxInitRichEdit2();

	//Check for the previous instance as soon as possible
	if (m_pInstanceChecker.PreviousInstanceRunning())
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		AfxMessageBox(_T("Previous version detected, will now restore it..."), MB_OK | MB_ICONINFORMATION);
		m_pInstanceChecker.ActivatePreviousInstance(cmdInfo.m_strFileName);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Mihai Moga"));

	delete AfxGetApp()->m_pszExeName;
	AfxGetApp()->m_pszExeName = new TCHAR[_MAX_PATH];

	delete AfxGetApp()->m_pszProfileName;
	AfxGetApp()->m_pszProfileName = new TCHAR[_MAX_PATH];

	TCHAR lpszDrive[_MAX_DRIVE];
	TCHAR lpszDirectory[_MAX_DIR];
	TCHAR lpszFilename[_MAX_FNAME];
	TCHAR lpszExtension[_MAX_EXT];

	LPTSTR lpszHelpPath = (LPTSTR) AfxGetApp()->m_pszHelpFilePath;
	LPTSTR lpszExePath = (LPTSTR) AfxGetApp()->m_pszExeName;
	LPTSTR lpszIniPath = (LPTSTR) AfxGetApp()->m_pszProfileName;

	VERIFY(0 == _tsplitpath_s(lpszHelpPath, lpszDrive, _MAX_DRIVE, lpszDirectory, _MAX_DIR, lpszFilename, _MAX_FNAME, lpszExtension, _MAX_EXT));
	VERIFY(0 == _tmakepath_s(lpszExePath, _MAX_PATH, lpszDrive, lpszDirectory, lpszFilename, _T(".exe")));
	VERIFY(0 == _tmakepath_s(lpszIniPath, _MAX_PATH, lpszDrive, lpszDirectory, lpszFilename, _T(".config")));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_HIDE);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	// If this is the first instance of our App then track it so any other instances can find us
	m_pInstanceChecker.TrackFirstInstanceRunning(m_pMainWnd->GetSafeHwnd());

	return TRUE;
}

int CIntelliLinkApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CIntelliLinkApp message handlers

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

protected:
	CStatic m_ctrlVersion;
	CEdit m_ctrlWarning;
	CVersionInfo m_pVersionInfo;
	CHLinkCtrl m_ctrlWebsite;
	CHLinkCtrl m_ctrlEmail;
	CHLinkCtrl m_ctrlContributors;

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION, m_ctrlVersion);
	DDX_Control(pDX, IDC_WARNING, m_ctrlWarning);
	DDX_Control(pDX, IDC_WEBSITE, m_ctrlWebsite);
	DDX_Control(pDX, IDC_EMAIL, m_ctrlEmail);
	DDX_Control(pDX, IDC_CONTRIBUTORS, m_ctrlContributors);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_pVersionInfo.Load(AfxGetApp()->m_pszExeName))
	{
		CString strName = m_pVersionInfo.GetProductName().c_str();
		CString strVersion = m_pVersionInfo.GetProductVersionAsString().c_str();
		strVersion.Replace(_T(" "), _T(""));
		strVersion.Replace(_T(","), _T("."));
		const int nFirst = strVersion.Find(_T('.'));
		const int nSecond = strVersion.Find(_T('.'), nFirst + 1);
		strVersion.Truncate(nSecond);
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion);
	}

	m_ctrlWarning.SetWindowText(_T("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>."));

	m_ctrlWebsite.SetHyperLink(_T("http://www.emvs.site/"));
	m_ctrlEmail.SetHyperLink(_T("mailto:contact@emvs.site"));
	m_ctrlContributors.SetHyperLink(_T("https://github.com/mihaimoga/IntelliLink/graphs/contributors"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

// App command to run the dialog
void CIntelliLinkApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIntelliLinkApp customization load/save methods

void CIntelliLinkApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CIntelliLinkApp::LoadCustomState()
{
}

void CIntelliLinkApp::SaveCustomState()
{
}

// CIntelliLinkApp message handlers
