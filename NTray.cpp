/*
Module : NTray.cpp
Purpose: implementation for a C++ class to encapsulate the Shell_NotifyIcon API.
Created: PJN / 14-05-1997
History: PJN / 25-11-1997 Addition of the following
                          1. HideIcon(), ShowIcon() & MoveToExtremeRight() 
                          2. Support for animated tray icons
         PJN / 23-06-1998 Class now supports the new Taskbar Creation Notification 
                          message which comes with IE 4. This allows the tray icon
                          to be recreated whenever the explorer restarts (Crashes!!)
         PJN / 22-07-1998 1. Code now compiles cleanly at warning level 4
                          2. Code is now UNICODE enabled + build configurations are provided
                          3. The documentation for the class has been updated
         PJN / 27-01-1999 1. Code first tries to load a 16*16 icon before loading the 32*32
                          version. This removes the blurryness which was previously occurring
         PJN / 28-01-1999 1. Fixed a number of level 4 warnings which were occurring.
         PJN / 09-05-1999 1. Fixed a problem as documented in KB article "PRB: Menus for 
                          Notification Icons Do Not Work Correctly", Article ID: Q135788 
         PJN / 15-05-1999 1. Now uses the author's hookwnd class. This prevents the need to 
                          create the two hidden windows namely CTrayRessurectionWnd and
                          CTrayTimerWnd
                          2. Code now compiles cleanly at warning level 4
                          3. General code tidy up and rearrangement
                          4. Added numerous ASSERT's to improve the code's robustness
                          5. Added functions to allow context menu to be customized
         PJN / 01-01-2001 1. Now includes copyright message in the source code and documentation. 
                          2. Fixed problem where the window does not get focus after double clicking 
                          on the tray icon
                          3. Now fully supports the Windows 2000 balloon style tooltips
                          4. Fixed a off by one problem in some of the ASSERT's
                          5. Fixed problems with Unicode build configurations
                          6. Provided Win2k specific build configurations
         PJN / 10-02-2001 1. Now fully supports creation of 2 tray icons at the same time
         PJN / 13-06-2001 1. Now removes windows hook upon call to RemoveIcon
         PJN / 26-08-2001 1. Fixed memory leak in RemoveIcon.
                          2. Fixed GPF in RemoveIcon by removing call to Unhook
         PJN / 28-08-2001 1. Added support for direct access to the System Tray's HDC. This allows
                          you to generate an icon for the tray on the fly using GDI calls. The idea
                          came from the article by Jeff Heaton in the April issue of WDJ. Also added
                          are overriden Create methods to allow you to easily construct a dynamic
                          tray icon given a BITMAP instead of an ICON.
         PJN / 21-03-2003 1. Fixed icon resource leaks in SetIcon(LPCTSTR lpIconName) and 
                          SetIcon(UINT nIDResource). Thanks to Egor Pervouninski for reporting this.
                          2. Fixed unhooking of the tray icon when the notification window is being
                          closed.
         PJN / 31-03-2003 1. Now uses V1.05 of my Hookwnd class
         PJN / 02-04-2003 1. Now uses v1.06 of my Hookwnd class
                          2. Fixed a bug in the sample app for this class where the hooks should
                          have been created as global instances rather than as member variables of
                          the mainframe window. This ensures that the hooks remain valid even after
                          calling DefWindowProc on the mainframe.
         PJN / 23-07-2004 1. Minor update to remove unnecessary include of "resource.h"
         PJN / 03-03-2006 1. Updated copyright details.
                          2. Updated the documentation to use the same style as the web site.
                          3. Did a spell check of the documentation.        
                          4. Fixed some issues when the code is compiled using /Wp64. Please note that
                          to support this the code now requires a recentish Platform SDK to be installed
                          if the code is compiled with Visual C++ 6.
                          5. Replaced all calls to ZeroMemory with memset.
                          6. Fixed an issue where SetBalloonDetails was not setting the cbSize parameter.
                          Thanks to Enrique Granda for reporting this issue.
                          7. Added support for NIIF_USER and NIIF_NONE flags.
                          8. Now includes support for NIM_NIMSETVERSION via SetVersion. In addition this
                          is now automatically set in the Create() calls if the Win2k boolean parameter
                          is set.
                          9. Removed derivation from CObject as it was not really needed.
                          10. Now includes support for NIM_SETFOCUS
                          11. Added support for NIS_HIDDEN via the ShowIcon and HideIcon methods.
                          12. Added support for NIIF_NOSOUND
         PJN / 27-06-2006 1. Code now uses new C++ style casts rather than old style C casts where necessary.
                          2. The class framework now requires the Platform SDK if compiled using VC 6.  
                          3. Updated the logic of the ASSERTs which validate the various string lengths.
                          4. Fixed a bug in CTrayNotifyIcon::SetFocus() where the cbSize value was not being
                          set correctly.
                          5. CTrayIconHooker class now uses ATL's CWindowImpl class in preference to the author's
                          CHookWnd class. This does mean that for MFC only client projects, you will need to add
                          ATL support to your project.
                          6. Optimized CTrayIconHooker constructor code
                          7. Updated code to compile cleanly using VC 2005. Thanks to "Itamar" for prompting this
                          update.
                          8. Addition of a CTRAYNOTIFYICON_EXT_CLASS and CTRAYNOTIFYICON_EXT_API macros which makes 
                          the class easier to use in an extension dll.
                          9. Made CTrayNotifyIcon destructor virtual
         PJN / 03-07-2005 1. Fixed a bug where the HideIcon functionality did not work on Windows 2000. This was 
                          related to how the cbSize member of the NOTIFYICONDATA structure was initialized. The code
                          now dynamically determines the correct size to set at runtime according to the instructions
                          provided by the MSDN documentation for this structure. As a result of this, all "bWin2k" 
                          parameters which were previously exposed via CTrayNotifyIcon have now been removed as there
                          is no need for them. Thanks to Edwin Geng for reporting this important bug. Client code will
                          still need to intelligently make decisions on what is supported by the OS. For example balloon
                          tray icons are only supported on Shell v5 (nominally Windows 2000 or later). CTrayNotifyIcon
                          will ASSERT if for example calls are made to it to create a balloon tray icon on operating 
                          systems < Windows 2000.
         PJN / 04-07-2006 1. Fixed a bug where the menu may pop up a second time after a menu item is chosen on 
                          Windows 2000. The problem was tracked down to the code in CTrayNotifyIcon::OnTrayNotification. 
                          During testing of this bug, I was unable to get a workable solution using the new shell 
                          messages of WM_CONTEXTMENU, NIN_KEYSELECT & NIN_SELECT on Windows 2000 and Windows XP. 
                          This means that the code in CTrayNotifyIcon::OnTrayNotification uses the old way of handling 
                          notifications (WM_RBUTTDOWN*). This does mean that by default, client apps which use the 
                          CTrayNotifyIcon class will not support the new keyboard and mouse semantics for tray icons
                          (IMHO this is no big loss!). Client code is of course free to handle their own notifications. 
                          If you go down this route then I would advise you to thoroughly test your application on 
                          Windows 2000 and Windows XP as my testing has shown that there is significant differences in 
                          how tray icons handle their messaging on these 2 operating systems. Thanks to Edwin Geng for 
                          reporting this issue.
                          2. Class now displays the menu based on the current message's screen coordinates, rather than
                          the current cursor screen coordinates.
                          3. Fixed bug in sample app where if the about dialog is already up and it is reactivated 
                          from the tray menu, it did not bring itself into the foreground
         PJN / 06-07-2006 1. Reverted the change made for v1.53 where the screen coordinates used to show the context 
                          menu use the current message's screen coordinates. Instead the pre v1.53 mechanism which 
                          uses the current cursor's screen coordinates is now used. Thanks to Itamar Syn-Hershko for 
                          reporting this issue.
         PJN / 19-07-2006 1. The default menu item can now be customized via SetDefaultMenuItem and 
                          GetDefaultMenuItem. Thanks to Mikhail Bykanov for suggesting this nice update.
                          2. Optimized CTrayNotifyIcon constructor code
         PJN / 19-08-2005 1. Updated the code to operate independent of MFC if so desired. This requires WTL which is an
                          open source library extension for ATL to provide UI support along the lines of MFC. Thanks to 
                          zhiguo zhao for providing this very nice addition.
         PJN / 15-09-2006 1. Fixed a bug where WM_DESTROY messages were not been handled correctly for the top level window
                          which the CTrayIconHooker class subclasses in order to handle the tray resurrection message,
                          the animation timers and auto destroying of the icons when the top level window is destroyed. 
                          Thanks to Edward Livingston for reporting this bug.
                          2. Fixed a bug where the tray icons were not being recreated correctly when we receive the 
                          "TaskbarCreated" when Explorer is restarted. Thanks to Nuno Esculcas for reporting this bug.
                          3. Split the functionality of hiding versus deleting and showing versus creating of the tray
                          icon into 4 separate functions, namely Delete(), Create(), Hide() and Show(). Note that Hide 
                          and Show functionality is only available on Shell v5 or later.
                          4. Fixed an issue with recreation of tray icons which use a dynamic icon created from a bitmap
                          (through the use of BitmapToIcon).
                          5. CTrayNotifyIcon::LoadIconResource now loads up an icon as a shared icon resource using 
                          LoadImage. This should avoid resource leaks using this function.
         PJN / 15-06-2007 1. Updated copyright messages.
                          2. If the code detects that MFC is not included in the project, the code uses the standard
                          preprocessor define "_CSTRING_NS" to declare the string class to use rather than explicitly 
                          using WTL::CString. Thanks to Krzysztof Suszka for reporting this issue.
                          3. Updated sample app to compile cleanly on VC 2005.
                          4. Addition of a "BOOL bShow" to all the Create methods. This allows you to create an icon 
                          without actually showing it. This avoids the flicker which previously occurred if you created 
                          the icon and then immediately hid the icon. Thanks to Krzysztof Suszka for providing this 
                          suggestion.
                          5. Demo app now initially creates the first icon as hidden for demonstration purposes.
                          6. Added support for NIIF_LARGE_ICON. This Vista only feature allows you to create a large 
                          balloon icon.
                          7. Added support for NIF_REALTIME. This Vista only flag allows you to specify not to bother 
                          showing the balloon if it is delayed due to the presence of an existing balloon.
                          8. Added support for NOTIFYICONDATA::hBalloonIcon. This Vista only feature allows you to 
                          create a user specified balloon icon which is different to the actual tray icon.
                          9. LoadIconResource method now includes support for loading large icons and has been renamed
                          to simply LoadIcon. Also two overridden versions of this method have been provided which allow
                          the hInstance resource ID to be specified to load the icon from.
                          10. Reworked the internal code to CTrayNotifyIcon which detects the shell version.
                          11. Updated the tray icon text in the demo app to better demonstrate the features of the class.
                          12. Updated the WTL sample to be consistent with the MFC sample code
                          13. Updated comments in documentation about usage of the Platform SDK.
         PJN / 13-10-2007 1. Subclassing of the top level window is now not down internally by the CTrayNotifyIcon class
                          using the CTrayIconHooker class. Instead now a hidden top level window is created for each tray 
                          icon you create and these look after handling the tray resurrection and animated icon timer
                          messages. This refactoring of the internals of the class now also fixes a bug where an application
                          which creates multiples tray icons would only get one icon recreated when the tray resurrection
                          message was received. Thanks to Steven Dwyer for prompting this update. 
                          2. Updated the MFC sample app to correctly initialize ATL for VC 6
         PJN / 12-03-2008 1. Updated copyright details
                          2. Fixed a bug in SetBalloonDetails where the code did not set the flag NIF_ICON if a user defined
                          icon was being set. Thanks to "arni" for reporting this bug. 
                          3. Updated the sample app to clean compile on VC 2008
         PJN / 22-06-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
                          2. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          3. Removed VC 6 style AppWizard comments from the code.
                          4. The code now only supports VC 2005 or later. 
         PJN / 10-04-2010 1. Updated copyright details.
                          2. Updated the project settings to more modern default values.
                          3. Updated the WTL version of LoadIcon to use the more modern ModuleHelper class from WTL to get 
                          the resource instance. Thanks to "Yarp" for reporting this issue.
                          4. The class now has support for the Windows 7 "NIIF_RESPECT_QUIET_TIME" flag. This value can be
                          set via the new "bQuietTime" parameter to the Create method.
                          5. Updated the code which does version detection of the Shell version
         PJN / 10-07-2010 1. Updated the sample app to compile cleanly on VS 2010.
                          2. Fixed a bug in CTrayNotifyIcon::Delete where the code would ASSERT if the tray notify icon was
                          never actually created. Thanks to "trophim" for reporting this bug.
         PJN / 06-11-2010 1. Minor update to code in SetTooltipText to code which handles unreferenced variable compiler 
                          warning
                          2. Implemented a GetTooltipMaxSize method which reports the maximum size which the tooltip can be
                          for a tray icon. Thanks to Geert van Horrik for this nice addition
                          3. All places which copy text to the underlying NOTIFYICONDATA now use the _TRUNCATE parameter in 
                          their call to the Safe CRT runtime. This change in behaviour means that client apps will no longer
                          crash if they supply data larger than this Windows structure can accommodate. Thanks to Geert van 
                          Horrik for prompting this update.
                          4. All calls to sizeof(struct)/sizeof(first element) have been replaced with _countof
                          5. Fixed a linker error when compiling the WTL sample app in release mode.
         PJN / 26-11-2010 1. Minor update to use DBG_UNREFERENCED_LOCAL_VARIABLE macro. Thanks to Jukka Ojanen for prompting this 
                          update.
         PJN / 27-04-2016 1. Updated copyright details.
                          2. Updated the code to clean compile on VC 2012 - VC 2015.
                          3. Removed support for CTRAYNOTIFYICON_NOWIN2K preprocessor macro
                          4. Removed various redefines of ShellApi.h constants from the code
                          5. Added SAL annotations to all the code.
                          6. Reworked the definition of the string class to now use a typedef internal to the CTrayNotifyIcon 
                          class.
                          7. Updated CTrayNotifyIcon::OnTrayNotification to handle NOTIFYICON_VERSION_4 style notifications.
                          8. Reworked the internal storage of the animation icons to use ATL::CHeapPtr
         PJN / 26-11-2017 1. Updated copyright details.
                          2. Fixed a number of compilation errors when compiled with VC 2017
                          3. Updated the code to compile cleanly when _ATL_NO_AUTOMATIC_NAMESPACE is defined.
                          4. Replaced NULL throughout the codebase with nullptr. This means that the minimum requirement 
                          for the framework is now VC 2010.
                          5. Replaced BOOL throughout the codebase with bool.
                          6. Removed GetTooltipMaxSize as it did not provide very useful functionality.
                          7. Removed all functionality for Pre Vista versions of Windows.
         PJN / 07-10-2018 1. Updated copyright details.
                          2. Fixed a number of C++ core guidelines compiler warnings. These changes mean that
                          the code will now only compile on VC 2017 or later.
         PJN / 22-04-2019 1. Updated copyright details
                          2. Updated the code to clean compile on VC 2019
         PJN / 15-09-2019 1. Replaced enum with enum class throughout the code
         PJN / 03-04-2020 1. Updated copyright details.
                          2. Fixed more Clang-Tidy static code analysis warnings in the code.
                          3. Removed internal declaration of various SDK structs. You now need to define 
                          NTDDI_VERSION >= NTDDI_VISTA to use the code.
         PJN / 12-05-2020 1. Fixed more Clang-Tidy static code analysis warnings in the code.
         PJN / 18-12-2021 1. Updated copyright details.
                          2. Fixed more Clang-Tidy static code analysis warnings in the code.
         PJN / 21-05-2022 1. Updated copyright details.
                          2. Updated the code to use C++ uniform initialization for all variable declarations
         PJN / 18-06-2023 1. Updated copyright details.
                          2. Added additional SAL annotations to the code.
         PJN / 28-10-2023 1. Fixed an issue where the CTrayNotifyIcon::OnTrayNotification callback method would 
                          not work correctly if the m_NotifyIconData.uTimeout member variable gets updated during runtime of 
                          client applications. This can occur when you call CTrayNotifyIcon::SetBalloonDetails. Thanks to 
                          Maisala Tuomo for reporting this bug.

Copyright (c) 1997 - 2023 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code.

*/


//////////////////// Includes /////////////////////////////////////////////////

#include "stdafx.h"
#include "NTray.h"
#ifndef _INC_SHELLAPI
#pragma message("To avoid this message, please put shellapi.h in your pre compiled header (normally stdafx.h)")
#include <shellapi.h>
#endif //#ifndef _INC_SHELLAPI


//////////////////// Macros / Defines /////////////////////////////////////////

#ifdef _AFX
#ifdef _DEBUG
#define new DEBUG_NEW
#endif //#ifdef _DEBUG
#endif //#ifdef _AFX

#ifndef NIIF_RESPECT_QUIET_TIME
#define NIIF_RESPECT_QUIET_TIME 0x00000080
#endif //#ifndef NIIF_RESPECT_QUIET_TIME


//////////////////// Implementation ///////////////////////////////////////////

#pragma warning(suppress: 26426)
const UINT wm_TaskbarCreated = RegisterWindowMessage(_T("TaskbarCreated"));

CTrayNotifyIcon::CTrayNotifyIcon() noexcept : m_NotifyIconData{},
m_NotifyIconDataTimeout{ 0 },
m_bCreated{ false },
m_bHidden{ false },
m_pNotificationWnd{ nullptr },
m_nDefaultMenuItem{ 0 },
m_bDefaultMenuItemByPos{ true },
m_hDynamicIcon{ nullptr },
m_nNumIcons{ 0 },
m_nTimerID{ 0 },
m_nCurrentIconIndex{ 0 }
{
	m_NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
}

CTrayNotifyIcon::~CTrayNotifyIcon()
{
	//Delete the tray icon
	Delete(true);

	//Free up any dynamic icon we may have
	if (m_hDynamicIcon != nullptr)
	{
		DestroyIcon(m_hDynamicIcon);
		m_hDynamicIcon = nullptr;
	}
}

_Success_(return != false) bool CTrayNotifyIcon::Delete(_In_ bool bCloseHelperWindow) noexcept
{
	//What will be the return value from this function (assume the best)
	bool bSuccess{ true };

	if (m_bCreated)
	{
		m_NotifyIconData.uFlags = 0;
		bSuccess = Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
		m_bCreated = false;
	}

	//Close the helper window if requested to do so
	if (bCloseHelperWindow && IsWindow())
		SendMessage(WM_CLOSE);

	return bSuccess;
}

#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ bool bShow) noexcept
{
	m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

	if (!bShow)
	{
		m_NotifyIconData.uFlags |= NIF_STATE;
		m_NotifyIconData.dwState = NIS_HIDDEN;
		m_NotifyIconData.dwStateMask = NIS_HIDDEN;
	}

	const BOOL bSuccess{ Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData) };
	if (bSuccess)
	{
		m_bCreated = true;
		if (!bShow)
			m_bHidden = true;
	}

	return bSuccess;
}

_Success_(return != false) bool CTrayNotifyIcon::Hide() noexcept
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(!m_bHidden); //Only makes sense to hide the icon if it is not already hidden

	m_NotifyIconData.uFlags = NIF_STATE;
	m_NotifyIconData.dwState = NIS_HIDDEN;
	m_NotifyIconData.dwStateMask = NIS_HIDDEN;
	const BOOL bSuccess{ Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData) };
	if (bSuccess)
		m_bHidden = true;
	return bSuccess;
}

_Success_(return != false) bool CTrayNotifyIcon::Show() noexcept
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(m_bHidden); //Only makes sense to show the icon if it has been previously hidden
#pragma warning(suppress: 26477)
	ATLASSERT(m_bCreated);

	m_NotifyIconData.uFlags = NIF_STATE;
	m_NotifyIconData.dwState = 0;
	m_NotifyIconData.dwStateMask = NIS_HIDDEN;
	const BOOL bSuccess{ Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData) };
	if (bSuccess)
		m_bHidden = false;
	return bSuccess;
}

#pragma warning(suppress: 26434)
void CTrayNotifyIcon::SetMenu(_In_ HMENU hMenu)
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(hMenu);

	m_Menu.DestroyMenu();
	m_Menu.Attach(hMenu);

#ifdef _AFX
	CMenu* pSubMenu{ m_Menu.GetSubMenu(0) };
#pragma warning(suppress: 26477)
	ATLASSUME(pSubMenu != nullptr); //Your menu resource has been designed incorrectly

	//Make the specified menu item the default (bold font)
	pSubMenu->SetDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#else
	CMenuHandle subMenu{ m_Menu.GetSubMenu(0) };
#pragma warning(suppress: 26477)
	ATLASSERT(subMenu.IsMenu()); //Your menu resource has been designed incorrectly

	//Make the specified menu item the default (bold font)
	subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#endif //#ifdef _AFX
}

#pragma warning(suppress: 26434)
CMenu& CTrayNotifyIcon::GetMenu() noexcept
{
	return m_Menu;
}

void CTrayNotifyIcon::SetDefaultMenuItem(_In_ UINT uItem, _In_ bool fByPos)
{
	m_nDefaultMenuItem = uItem;
	m_bDefaultMenuItemByPos = fByPos;

	//Also update in the live menu if it is present
	if (m_Menu.operator HMENU())
	{
#ifdef _AFX
		CMenu* pSubMenu{ m_Menu.GetSubMenu(0) };
#pragma warning(suppress: 26477)
		ATLASSUME(pSubMenu != nullptr); //Your menu resource has been designed incorrectly

		pSubMenu->SetDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#else
		CMenuHandle subMenu{ m_Menu.GetSubMenu(0) };
#pragma warning(suppress: 26477)
		ATLASSERT(subMenu.IsMenu()); //Your menu resource has been designed incorrectly

		subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#endif //#ifdef _AFX
	}
}

#ifdef _AFX
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWnd * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ HICON hIcon, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bShow)
#else
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWindow * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ HICON hIcon, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bShow)
#endif //#ifdef _AFX
{
	//Validate our parameters
#pragma warning(suppress: 26477 26486)
	ATLASSUME((pNotifyWnd != nullptr) && ::IsWindow(pNotifyWnd->operator HWND()));
#pragma warning(suppress: 26477)
	ATLASSERT(hIcon != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(nNotifyMessage >= WM_USER); //Make sure we avoid conflict with other messages

	//Load up the menu resource which is to be used as the context menu
	if (!m_Menu.LoadMenu(uMenuID == 0 ? uID : uMenuID))
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false);
		return false;
	}
#ifdef _AFX
	CMenu* pSubMenu{ m_Menu.GetSubMenu(0) };
	if (pSubMenu == nullptr)
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false); //Your menu resource has been designed incorrectly
		return false;
	}
	//Make the specified menu item the default (bold font)
	pSubMenu->SetDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#else
	CMenuHandle subMenu{ m_Menu.GetSubMenu(0) };
	if (!subMenu.IsMenu())
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false); //Your menu resource has been designed incorrectly
		return false;
	}
	subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#endif //#ifdef _AFX

	//Create the helper window
	if (!CreateHelperWindow())
		return false;

	//Call the Shell_NotifyIcon function
	m_pNotificationWnd = pNotifyWnd;
	m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
#pragma warning(suppress: 26486)
	m_NotifyIconData.hWnd = pNotifyWnd->operator HWND();
	m_NotifyIconData.uID = uID;
	m_NotifyIconData.uCallbackMessage = nNotifyMessage;
	m_NotifyIconData.hIcon = hIcon;
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szTip, _countof(m_NotifyIconData.szTip), pszTooltipText, _TRUNCATE);

	if (!bShow)
	{
		m_NotifyIconData.uFlags |= NIF_STATE;
		m_NotifyIconData.dwState = NIS_HIDDEN;
		m_NotifyIconData.dwStateMask = NIS_HIDDEN;
	}
	m_bCreated = Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);
	if (m_bCreated)
	{
		if (!bShow)
			m_bHidden = true;

		//Turn on latest Shell behaviour
		SetVersion(NOTIFYICON_VERSION);
	}

	return m_bCreated;
}

_Success_(return != false) bool CTrayNotifyIcon::SetVersion(_In_ UINT uVersion) noexcept
{
	//Call the Shell_NotifyIcon function
	m_NotifyIconData.uVersion = uVersion;
	return Shell_NotifyIcon(NIM_SETVERSION, &m_NotifyIconData);
}

HICON CTrayNotifyIcon::BitmapToIcon(_In_ const CBitmap * pBitmap)
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSUME(pBitmap != nullptr);

	//Get the width and height of a small icon
	const int nWidth{ GetSystemMetrics(SM_CXSMICON) };
	const int nHeight{ GetSystemMetrics(SM_CYSMICON) };

	//Create a 0 mask
	const int nMaskSize = nHeight * (nWidth / 8);
	ATL::CHeapPtr<BYTE> pMask;
	if (!pMask.Allocate(nMaskSize))
#pragma warning(suppress: 26487)
		return nullptr;
#pragma warning(suppress: 26486)
	memset(pMask.m_pData, 0, nMaskSize);

	//Create a mask bitmap
	CBitmap maskBitmap;
#ifdef _AFX
#pragma warning(suppress: 26486)
	const BOOL bSuccess{ maskBitmap.CreateBitmap(nWidth, nHeight, 1, 1, pMask.m_pData) };
#else
#pragma warning(suppress: 26486)
	maskBitmap.CreateBitmap(nWidth, nHeight, 1, 1, pMask.m_pData);
	const bool bSuccess{ !maskBitmap.IsNull() };
#endif //#ifdef _AFX

	//Handle the error
	if (!bSuccess)
#pragma warning(suppress: 26487)
		return nullptr;

	//Create an ICON base on the bitmap just created
	ICONINFO iconInfo;
	iconInfo.fIcon = TRUE;
	iconInfo.xHotspot = 0;
	iconInfo.yHotspot = 0;
	iconInfo.hbmMask = maskBitmap.operator HBITMAP();
#pragma warning(suppress: 26486)
	iconInfo.hbmColor = pBitmap->operator HBITMAP();
#pragma warning(suppress: 26487)
	return CreateIconIndirect(&iconInfo);
}

#ifdef _AFX
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWnd * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ const CBitmap * pBitmap, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bShow)
#else
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWindow * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ const CBitmap * pBitmap, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bShow)
#endif //#ifdef _AFX
{
	//Convert the bitmap to an Icon
	if (m_hDynamicIcon != nullptr)
		DestroyIcon(m_hDynamicIcon);
	m_hDynamicIcon = BitmapToIcon(pBitmap);

	//Pass the buck to the other function to do the work
	return Create(pNotifyWnd, uID, pszTooltipText, m_hDynamicIcon, nNotifyMessage, uMenuID, bShow);
}

#ifdef _AFX
#pragma warning(suppress: 26434 26487)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWnd * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ HICON * phIcons, _In_ int nNumIcons, _In_ DWORD dwDelay, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bShow)
#else
#pragma warning(suppress: 26434 26487)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWindow * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ HICON * phIcons, _In_ int nNumIcons, _In_ DWORD dwDelay, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bShow)
#endif //#ifdef _AFX
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSUME(phIcons != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
#pragma warning(suppress: 26477)
	ATLASSERT(dwDelay);

	//let the normal Create function do its stuff
#pragma warning(suppress: 26481 26486)
	bool bSuccess{ Create(pNotifyWnd, uID, pszTooltipText, phIcons[0], nNotifyMessage, uMenuID, bShow) };
	if (bSuccess)
	{
		//Start the animation
		bSuccess = StartAnimation(phIcons, nNumIcons, dwDelay);
	}

	return bSuccess;
}

#ifdef _AFX
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWnd * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ LPCTSTR pszBalloonText, _In_ LPCTSTR pszBalloonCaption, _In_ UINT nTimeout, _In_ BalloonStyle style, _In_ HICON hIcon, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon, _In_ bool bQuietTime, _In_ bool bShow)
#else
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWindow * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ LPCTSTR pszBalloonText, _In_ LPCTSTR pszBalloonCaption, _In_ UINT nTimeout, _In_ BalloonStyle style, _In_ HICON hIcon, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon, _In_ bool bQuietTime, _In_ bool bShow)
#endif //#ifdef _AFX
{
	//Validate our parameters
#pragma warning(suppress: 26477 26486)
	ATLASSUME((pNotifyWnd != nullptr) && ::IsWindow(pNotifyWnd->operator HWND()));

	//Load up the menu resource which is to be used as the context menu
	if (!m_Menu.LoadMenu(uMenuID == 0 ? uID : uMenuID))
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false);
		return false;
	}
#ifdef _AFX
	CMenu* pSubMenu{ m_Menu.GetSubMenu(0) };
	if (pSubMenu == nullptr)
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false); //Your menu resource has been designed incorrectly
		return false;
	}
	//Make the specified menu item the default (bold font)
	pSubMenu->SetDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#else
	CMenuHandle subMenu{ m_Menu.GetSubMenu(0) };
	if (!subMenu.IsMenu())
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false); //Your menu resource has been designed incorrectly
		return false;
	}
	//Make the specified menu item the default (bold font)
	subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
#endif //#ifdef _AFX

	//Create the helper window
	if (!CreateHelperWindow())
		return false;

	//Call the Shell_NotifyIcon function
#pragma warning(suppress: 26486)
	m_pNotificationWnd = pNotifyWnd;
#pragma warning(suppress: 26486)
	m_NotifyIconData.hWnd = pNotifyWnd->operator HWND();
	m_NotifyIconData.uID = uID;
	m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	m_NotifyIconData.uCallbackMessage = nNotifyMessage;
	m_NotifyIconData.hIcon = hIcon;
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szTip, _countof(m_NotifyIconData.szTip), pszTooltipText, _TRUNCATE);
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szInfo, _countof(m_NotifyIconData.szInfo), pszBalloonText, _TRUNCATE);
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szInfoTitle, _countof(m_NotifyIconData.szInfoTitle), pszBalloonCaption, _TRUNCATE);
	m_NotifyIconData.uTimeout = nTimeout;
	m_NotifyIconDataTimeout = nTimeout;
	switch (style)
	{
		case BalloonStyle::Warning:
		{
			m_NotifyIconData.dwInfoFlags = NIIF_WARNING;
			break;
		}
		case BalloonStyle::Error:
		{
			m_NotifyIconData.dwInfoFlags = NIIF_ERROR;
			break;
		}
		case BalloonStyle::Info:
		{
			m_NotifyIconData.dwInfoFlags = NIIF_INFO;
			break;
		}
		case BalloonStyle::None:
		{
			m_NotifyIconData.dwInfoFlags = NIIF_NONE;
			break;
		}
		case BalloonStyle::User:
		{
			if (hBalloonIcon != nullptr)
				m_NotifyIconData.hBalloonIcon = hBalloonIcon;
			else
			{
#pragma warning(suppress: 26477)
				ATLASSERT(hIcon != nullptr); //You forget to provide a user icon
			}
			m_NotifyIconData.dwInfoFlags = NIIF_USER;
			break;
		}
		default:
		{
#pragma warning(suppress: 26477)
			ATLASSERT(false);
			break;
		}
	}
	if (bNoSound)
		m_NotifyIconData.dwInfoFlags |= NIIF_NOSOUND;
	if (bLargeIcon)
		m_NotifyIconData.dwInfoFlags |= NIIF_LARGE_ICON;
	if (bRealtime)
		m_NotifyIconData.uFlags |= NIF_REALTIME;
	if (!bShow)
	{
		m_NotifyIconData.uFlags |= NIF_STATE;
		m_NotifyIconData.dwState = NIS_HIDDEN;
		m_NotifyIconData.dwStateMask = NIS_HIDDEN;
	}
	if (bQuietTime)
		m_NotifyIconData.dwInfoFlags |= NIIF_RESPECT_QUIET_TIME;

	m_bCreated = Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);
	if (m_bCreated)
	{
		if (!bShow)
			m_bHidden = true;

		//Turn on Shell v5 tray icon behaviour
		SetVersion(NOTIFYICON_VERSION);
	}

	return m_bCreated;
}

#ifdef _AFX
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWnd * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ LPCTSTR pszBalloonText, _In_ LPCTSTR pszBalloonCaption, _In_ UINT nTimeout, _In_ BalloonStyle style, _In_ const CBitmap * pBitmap, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon, _In_ bool bQuietTime, _In_ bool bShow)
#else
#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWindow * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ LPCTSTR pszBalloonText, _In_ LPCTSTR pszBalloonCaption, _In_ UINT nTimeout, _In_ BalloonStyle style, _In_ const CBitmap * pBitmap, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon, _In_ bool bQuietTime, _In_ bool bShow)
#endif //#ifdef _AFX
{
	//Convert the bitmap to an ICON
	if (m_hDynamicIcon != nullptr)
		DestroyIcon(m_hDynamicIcon);
	m_hDynamicIcon = BitmapToIcon(pBitmap);

	//Pass the buck to the other function to do the work
	return Create(pNotifyWnd, uID, pszTooltipText, pszBalloonText, pszBalloonCaption, nTimeout, style, m_hDynamicIcon, nNotifyMessage, uMenuID, bNoSound, bLargeIcon, bRealtime, hBalloonIcon, bQuietTime, bShow);
}

#ifdef _AFX
#pragma warning(suppress: 26434 26487)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWnd * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ LPCTSTR pszBalloonText, _In_ LPCTSTR pszBalloonCaption, _In_ UINT nTimeout, _In_ BalloonStyle style, _In_ HICON * phIcons, _In_ int nNumIcons, _In_ DWORD dwDelay, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon, _In_ bool bQuietTime, _In_ bool bShow)
#else
#pragma warning(suppress: 26434 26487)
_Success_(return != false) bool CTrayNotifyIcon::Create(_In_ CWindow * pNotifyWnd, _In_ UINT uID, _In_z_ LPCTSTR pszTooltipText, _In_ LPCTSTR pszBalloonText, _In_ LPCTSTR pszBalloonCaption, _In_ UINT nTimeout, _In_ BalloonStyle style, _In_ HICON * phIcons, _In_ int nNumIcons, _In_ DWORD dwDelay, _In_ UINT nNotifyMessage, _In_ UINT uMenuID, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon, _In_ bool bQuietTime, _In_ bool bShow)
#endif //#ifdef _AFX
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSUME(phIcons != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
#pragma warning(suppress: 26477)
	ATLASSERT(dwDelay);

	//let the normal Create function do its stuff
#pragma warning(suppress: 26481 26486)
	bool bSuccess{ Create(pNotifyWnd, uID, pszTooltipText, pszBalloonText, pszBalloonCaption, nTimeout, style, phIcons[0], nNotifyMessage, uMenuID, bNoSound, bLargeIcon, bRealtime, hBalloonIcon, bQuietTime, bShow) };
	if (bSuccess)
	{
		//Start the animation
		bSuccess = StartAnimation(phIcons, nNumIcons, dwDelay);
	}

	return bSuccess;
}

_Success_(return != false) bool CTrayNotifyIcon::SetBalloonDetails(_In_z_ LPCTSTR pszBalloonText, _In_z_ LPCTSTR pszBalloonCaption, _In_ BalloonStyle style, _In_ UINT nTimeout, _In_opt_ HICON hUserIcon, _In_ bool bNoSound, _In_ bool bLargeIcon, _In_ bool bRealtime, _In_opt_ HICON hBalloonIcon) noexcept
{
	if (!m_bCreated)
		return false;

	//Call the Shell_NotifyIcon function
	m_NotifyIconData.uFlags = NIF_INFO;
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szInfo, _countof(m_NotifyIconData.szInfo), pszBalloonText, _TRUNCATE);
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szInfoTitle, _countof(m_NotifyIconData.szInfoTitle), pszBalloonCaption, _TRUNCATE);
	//Save a copy of uVersion as uTimeout is a union of it, meaning that writing to uTimeout overwrites uVersion.
	//Because this class requires uVersion to be a specific value in its OnTrayNotification method, we restore
	//this value at the bottom of this method.
	const auto uOldVersion = m_NotifyIconData.uVersion;
	m_NotifyIconData.uTimeout = nTimeout;
	m_NotifyIconDataTimeout = nTimeout;
	switch (style)
	{
	case BalloonStyle::Warning:
	{
		m_NotifyIconData.dwInfoFlags = NIIF_WARNING;
		break;
	}
	case BalloonStyle::Error:
	{
		m_NotifyIconData.dwInfoFlags = NIIF_ERROR;
		break;
	}
	case BalloonStyle::Info:
	{
		m_NotifyIconData.dwInfoFlags = NIIF_INFO;
		break;
	}
	case BalloonStyle::None:
	{
		m_NotifyIconData.dwInfoFlags = NIIF_NONE;
		break;
	}
	case BalloonStyle::User:
	{
		if (hBalloonIcon != nullptr)
			m_NotifyIconData.hBalloonIcon = hBalloonIcon;
		else
		{
#pragma warning(suppress: 26477)
			ATLASSERT(hUserIcon != nullptr); //You forget to provide a user icon
			m_NotifyIconData.uFlags |= NIF_ICON;
			m_NotifyIconData.hIcon = hUserIcon;
		}

		m_NotifyIconData.dwInfoFlags = NIIF_USER;
		break;
	}
	default:
	{
#pragma warning(suppress: 26477)
		ATLASSERT(false);
		break;
	}
	}
	if (bNoSound)
		m_NotifyIconData.dwInfoFlags |= NIIF_NOSOUND;
	if (bLargeIcon)
		m_NotifyIconData.dwInfoFlags |= NIIF_LARGE_ICON;
	if (bRealtime)
		m_NotifyIconData.uFlags |= NIF_REALTIME;
	const bool bSuccess = Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
	//Preserve the value in the "uVersion" member variable. See the comments above.
	m_NotifyIconData.uVersion = uOldVersion;
	return bSuccess;
}

CTrayNotifyIcon::String CTrayNotifyIcon::GetBalloonText() const
{
	String sText;
	if (m_bCreated)
#pragma warning(suppress: 26485)
		sText = m_NotifyIconData.szInfo;
	return sText;
}

CTrayNotifyIcon::String CTrayNotifyIcon::GetBalloonCaption() const
{
	String sText;
	if (m_bCreated)
#pragma warning(suppress: 26485)
		sText = m_NotifyIconData.szInfoTitle;
	return sText;
}

UINT CTrayNotifyIcon::GetBalloonTimeout() const noexcept
{
	UINT nTimeout{ 0 };
	if (m_bCreated)
		nTimeout = m_NotifyIconDataTimeout;
	return nTimeout;
}

_Success_(return != false) bool CTrayNotifyIcon::SetTooltipText(_In_z_ LPCTSTR pszTooltipText) noexcept
{
	if (!m_bCreated)
		return false;

	//Call the Shell_NotifyIcon function
	m_NotifyIconData.uFlags = NIF_TIP;
#pragma warning(suppress: 26485)
	_tcsncpy_s(m_NotifyIconData.szTip, _countof(m_NotifyIconData.szTip), pszTooltipText, _TRUNCATE);
	return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
}

_Success_(return != false) bool CTrayNotifyIcon::SetTooltipText(_In_ UINT nID)
{
	String sToolTipText;
	if (!sToolTipText.LoadString(nID))
		return false;

	//Let the other version of the function handle the rest
	return SetTooltipText(sToolTipText);
}

#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::SetIcon(_In_ const CBitmap * pBitmap)
{
	//Convert the bitmap to an ICON
	if (m_hDynamicIcon != nullptr)
		DestroyIcon(m_hDynamicIcon);
	m_hDynamicIcon = BitmapToIcon(pBitmap);

	//Delegate to the other version of this method
	return SetIcon(m_hDynamicIcon);
}

#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::SetIcon(_In_ HICON hIcon) noexcept
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(hIcon != nullptr);

	if (!m_bCreated)
		return false;

	//Since we are going to use one icon, stop any animation
	StopAnimation();

	//Call the Shell_NotifyIcon function
	m_NotifyIconData.uFlags = NIF_ICON;
	m_NotifyIconData.hIcon = hIcon;
	return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
}

#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::SetIcon(_In_ LPCTSTR lpIconName)
{
	return SetIcon(LoadIcon(lpIconName));
}

#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::SetIcon(_In_ UINT nIDResource)
{
	return SetIcon(LoadIcon(nIDResource));
}

_Success_(return != false) bool CTrayNotifyIcon::SetStandardIcon(_In_ LPCTSTR lpIconName) noexcept
{
	return SetIcon(::LoadIcon(nullptr, lpIconName));
}

_Success_(return != false) bool CTrayNotifyIcon::SetStandardIcon(_In_ UINT nIDResource) noexcept
{
	return SetIcon(::LoadIcon(nullptr, MAKEINTRESOURCE(nIDResource)));
}

#pragma warning(suppress: 26434 26487)
_Success_(return != false) bool CTrayNotifyIcon::SetIcon(_In_ HICON * phIcons, _In_ int nNumIcons, _In_ DWORD dwDelay) noexcept
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
#pragma warning(suppress: 26477)
	ATLASSUME(phIcons != nullptr);
#pragma warning(suppress: 26477)
	ATLASSERT(dwDelay);

#pragma warning(suppress: 26481 26486)
	if (!SetIcon(phIcons[0]))
		return false;

	//Start the animation
	return StartAnimation(phIcons, nNumIcons, dwDelay);
}

HICON CTrayNotifyIcon::LoadIcon(_In_ HINSTANCE hInstance, _In_ LPCTSTR lpIconName, _In_ bool bLargeIcon) noexcept
{
	return static_cast<HICON>(::LoadImage(hInstance, lpIconName, IMAGE_ICON, bLargeIcon ? GetSystemMetrics(SM_CXICON) : GetSystemMetrics(SM_CXSMICON), bLargeIcon ? GetSystemMetrics(SM_CYICON) : GetSystemMetrics(SM_CYSMICON), LR_SHARED));
}

HICON CTrayNotifyIcon::LoadIcon(_In_ HINSTANCE hInstance, _In_ UINT nIDResource, _In_ bool bLargeIcon) noexcept
{
	return LoadIcon(hInstance, MAKEINTRESOURCE(nIDResource), bLargeIcon);
}

HICON CTrayNotifyIcon::LoadIcon(_In_ LPCTSTR lpIconName, _In_ bool bLargeIcon)
{
#ifdef _AFX
	return LoadIcon(AfxGetResourceHandle(), lpIconName, bLargeIcon);
#else
	return LoadIcon(ModuleHelper::GetResourceInstance(), lpIconName, bLargeIcon);
#endif //#ifdef _AFX
}

HICON CTrayNotifyIcon::LoadIcon(_In_ UINT nIDResource, _In_ bool bLargeIcon)
{
	return LoadIcon(MAKEINTRESOURCE(nIDResource), bLargeIcon);
}

#ifdef _AFX
_Success_(return != false) bool CTrayNotifyIcon::SetNotificationWnd(_In_ CWnd * pNotifyWnd) noexcept
#else
_Success_(return != false) bool CTrayNotifyIcon::SetNotificationWnd(_In_ CWindow * pNotifyWnd) noexcept
#endif //#ifdef _AFX
{
	//Validate our parameters
#pragma warning(suppress: 26477 26486)
	ATLASSUME((pNotifyWnd != nullptr) && ::IsWindow(pNotifyWnd->m_hWnd));

	if (!m_bCreated)
		return false;

	//Call the Shell_NotifyIcon function
	m_pNotificationWnd = pNotifyWnd;
	m_NotifyIconData.hWnd = pNotifyWnd->m_hWnd;
	m_NotifyIconData.uFlags = 0;
	return Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
}

CTrayNotifyIcon::String CTrayNotifyIcon::GetTooltipText() const
{
	String sText;
	if (m_bCreated)
#pragma warning(suppress: 26485)
		sText = m_NotifyIconData.szTip;
	return sText;
}

#pragma warning(suppress: 26434)
HICON CTrayNotifyIcon::GetIcon() const noexcept
{
	HICON hIcon{ nullptr };
	if (m_bCreated)
	{
		if (UsingAnimatedIcon())
			hIcon = GetCurrentAnimationIcon();
		else
			hIcon = m_NotifyIconData.hIcon;
	}
	return hIcon;
}

#ifdef _AFX
CWnd* CTrayNotifyIcon::GetNotificationWnd() const noexcept
#else
ATL::CWindow* CTrayNotifyIcon::GetNotificationWnd() const noexcept
#endif //#ifdef _AFX
{
	return m_pNotificationWnd;
}

#pragma warning(suppress: 26434)
_Success_(return != false) bool CTrayNotifyIcon::SetFocus() noexcept
{
	//Call the Shell_NotifyIcon function
	return Shell_NotifyIcon(NIM_SETFOCUS, &m_NotifyIconData);
}

LRESULT CTrayNotifyIcon::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	bool bShowMenu{ false };
	bool bDoubleClick{ false };
	UINT nIconID{ 0 };
	if ((m_NotifyIconData.uVersion == 0) || (m_NotifyIconData.uVersion == NOTIFYICON_VERSION))
	{
#pragma warning(suppress: 26472)
		nIconID = static_cast<UINT>(wParam);
		bShowMenu = (lParam == WM_RBUTTONUP);
		bDoubleClick = (lParam == WM_LBUTTONDBLCLK);
	}
	else
	{
		nIconID = HIWORD(lParam);
		bShowMenu = (LOWORD(lParam) == WM_CONTEXTMENU);
		bDoubleClick = (LOWORD(lParam) == WM_LBUTTONDBLCLK);
	}

	//Return quickly if its not for this tray icon
	if (nIconID != m_NotifyIconData.uID)
		return 0L;

	//Show the context menu or handle the double click
	if (bShowMenu || bDoubleClick)
	{
#ifdef _AFX
		CMenu* pSubMenu{ m_Menu.GetSubMenu(0) };
#pragma warning(suppress: 26477)
		ATLASSUME(pSubMenu != nullptr); //Your menu resource has been designed incorrectly
#else
		CMenuHandle subMenu{ m_Menu.GetSubMenu(0) };
#pragma warning(suppress: 26477)
		ATLASSERT(subMenu.IsMenu());
#endif //#ifdef _AFX

		if (bShowMenu)
		{
			CPoint ptCursor;
			GetCursorPos(&ptCursor);
			::SetForegroundWindow(m_NotifyIconData.hWnd);
#ifdef _AFX
#pragma warning(suppress: 26486)
			::TrackPopupMenu(pSubMenu->m_hMenu, TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, 0, m_NotifyIconData.hWnd, nullptr);
#else
			::TrackPopupMenu(subMenu, TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, 0, m_NotifyIconData.hWnd, nullptr);
#endif //#ifdef _AFX
			::PostMessage(m_NotifyIconData.hWnd, WM_NULL, 0, 0);
		}
		else if (bDoubleClick) //double click received, the default action is to execute first menu item
		{
			::SetForegroundWindow(m_NotifyIconData.hWnd);
#ifdef _AFX
			const UINT nDefaultItem{ pSubMenu->GetDefaultItem(GMDI_GOINTOPOPUPS, FALSE) };
#else
			const UINT nDefaultItem{ subMenu.GetMenuDefaultItem(FALSE, GMDI_GOINTOPOPUPS) };
#endif //#ifdef _AFX
#pragma warning(suppress: 26472)
			if (nDefaultItem != static_cast<UINT>(-1))
				::SendMessage(m_NotifyIconData.hWnd, WM_COMMAND, nDefaultItem, 0);
		}
	}

	return 1; // handled
}

_Success_(return != false) bool CTrayNotifyIcon::GetDynamicDCAndBitmap(_In_ CDC * pDC, _In_ CBitmap * pBitmap)
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSUME(pDC != nullptr);
#pragma warning(suppress: 26477)
	ATLASSUME(pBitmap != nullptr);

	//Get the HWND for the desktop
#ifdef _AFX
	CWnd* pWndScreen{ CWnd::GetDesktopWindow() };
	if (pWndScreen == nullptr)
		return false;
#else
	CWindow WndScreen{ ::GetDesktopWindow() };
	if (!WndScreen.IsWindow())
		return false;
#endif //#ifdef _AFX

	//Get the desktop HDC to create a compatible bitmap from
#ifdef _AFX
	CDC* pDCScreen{ pWndScreen->GetDC() };
	if (pDCScreen == nullptr)
		return false;
#else
	CDC DCScreen{ WndScreen.GetDC() };
	if (DCScreen.IsNull())
		return false;
#endif //#ifdef _AFX

	//Get the width and height of a small icon
	const int nWidth{ GetSystemMetrics(SM_CXSMICON) };
	const int nHeight{ GetSystemMetrics(SM_CYSMICON) };

	//Create an off-screen bitmap that the dynamic tray icon 
	//can be drawn into (Compatible with the desktop DC)
#ifdef _AFX
#pragma warning(suppress: 26486)
	BOOL bSuccess{ pBitmap->CreateCompatibleBitmap(pDCScreen, nWidth, nHeight) };
#else
#pragma warning(suppress: 26486)
	BOOL bSuccess = (pBitmap->CreateCompatibleBitmap(DCScreen.operator HDC(), nWidth, nHeight) != nullptr);
#endif //#ifdef _AFX
	if (!bSuccess)
	{
#ifdef _AFX
		pWndScreen->ReleaseDC(pDCScreen);
#else
		WndScreen.ReleaseDC(DCScreen);
#endif //#ifdef _AFX
		return false;
	}

	//Get a HDC to the newly created off-screen bitmap
#ifdef _AFX
	bSuccess = pDC->CreateCompatibleDC(pDCScreen);
#else
#pragma warning(suppress: 26486)
	bSuccess = (pDC->CreateCompatibleDC(DCScreen.operator HDC()) != nullptr);
#endif //#ifdef _AFX
	if (!bSuccess)
	{
		//Release the Screen DC now that we are finished with it
#ifdef _AFX
		pWndScreen->ReleaseDC(pDCScreen);
#else
		WndScreen.ReleaseDC(DCScreen);
#endif //#ifdef _AFX

		//Free up the bitmap now that we are finished with it
		pBitmap->DeleteObject();

		return false;
	}

	//Select the bitmap into the offscreen DC
#ifdef _AFX
	pDC->SelectObject(pBitmap);
#else
	pDC->SelectBitmap(pBitmap->operator HBITMAP());
#endif //#ifdef _AFX

	//Release the Screen DC now that we are finished with it
#ifdef _AFX
	pWndScreen->ReleaseDC(pDCScreen);
#else
	WndScreen.ReleaseDC(DCScreen);
#endif //#ifdef _AFX

	return true;
}

#pragma warning(suppress: 26487)
_Success_(return != false) bool CTrayNotifyIcon::StartAnimation(_In_ HICON * phIcons, _In_ int nNumIcons, _In_ DWORD dwDelay) noexcept
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
#pragma warning(suppress: 26477)
	ATLASSUME(phIcons != nullptr); //array of icon handles must be valid
#pragma warning(suppress: 26477)
	ATLASSERT(dwDelay); //must be non zero timer interval

	//Stop the animation if already started
	StopAnimation();

	//Hive away all the values locally
#pragma warning(suppress: 26477)
	ATLASSERT(m_Icons.m_pData == nullptr);
	if (!m_Icons.Allocate(nNumIcons))
		return false;
#pragma warning(suppress: 26477)
	ATLASSUME(m_Icons.m_pData != nullptr);
	for (int i = 0; i < nNumIcons; i++)
#pragma warning(suppress: 26481)
		m_Icons.m_pData[i] = phIcons[i];
	m_nNumIcons = nNumIcons;

	//Start up the timer
	m_nTimerID = SetTimer(m_NotifyIconData.uID, dwDelay);

	return true;
}

void CTrayNotifyIcon::StopAnimation() noexcept
{
	//Kill the timer
	if (m_nTimerID)
	{
		if (::IsWindow(m_hWnd))
			KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}

	//Free up the memory
	if (m_Icons.m_pData != nullptr)
		m_Icons.Free();

	//Reset the other animation related variables
	m_nCurrentIconIndex = 0;
	m_nNumIcons = 0;
}

_Success_(return != false) bool CTrayNotifyIcon::UsingAnimatedIcon() const noexcept
{
	return (m_nNumIcons != 0);
}

HICON CTrayNotifyIcon::GetCurrentAnimationIcon() const noexcept
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(UsingAnimatedIcon());
#pragma warning(suppress: 26477)
	ATLASSUME(m_Icons.m_pData != nullptr);

#pragma warning(suppress: 26481)
	return m_Icons.m_pData[m_nCurrentIconIndex];
}

_Success_(return != FALSE) BOOL CTrayNotifyIcon::ProcessWindowMessage(_In_ HWND /*hWnd*/, _In_ UINT nMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_ LRESULT & lResult, _In_ DWORD /*dwMsgMapID*/) noexcept
{
	lResult = 0;
	BOOL bHandled{ FALSE };

	if (nMsg == wm_TaskbarCreated)
	{
		lResult = OnTaskbarCreated(wParam, lParam);
		bHandled = TRUE;
	}
	else if ((nMsg == WM_TIMER) && (wParam == m_NotifyIconData.uID))
	{
		OnTimer(m_NotifyIconData.uID);
		bHandled = TRUE;
	}
	else if (nMsg == WM_DESTROY)
	{
		OnDestroy();
		bHandled = TRUE;
	}

	return bHandled;
}

void CTrayNotifyIcon::OnDestroy() noexcept
{
	StopAnimation();
}

LRESULT CTrayNotifyIcon::OnTaskbarCreated(WPARAM /*wParam*/, LPARAM /*lParam*/) noexcept
{
	//Refresh the tray icon if necessary
	const bool bShowing{ IsShowing() };
	Delete(false);
	Create(bShowing);

	return 0;
}

#ifdef _DEBUG
void CTrayNotifyIcon::OnTimer(UINT_PTR nIDEvent) noexcept
#else
void CTrayNotifyIcon::OnTimer(UINT_PTR /*nIDEvent*/) noexcept
#endif //#ifdef _DEBUG
{
	//Validate our parameters
#pragma warning(suppress: 26477)
	ATLASSERT(nIDEvent == m_nTimerID);
#pragma warning(suppress: 26477)
	ATLASSUME(m_Icons.m_pData != nullptr);

	//increment the icon index
	++m_nCurrentIconIndex;
	m_nCurrentIconIndex = m_nCurrentIconIndex % m_nNumIcons;

	//update the tray icon
	m_NotifyIconData.uFlags = NIF_ICON;
#pragma warning(suppress: 26481)
	m_NotifyIconData.hIcon = m_Icons.m_pData[m_nCurrentIconIndex];
	Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
}

_Success_(return != false) bool CTrayNotifyIcon::CreateHelperWindow()
{
	//Let the base class do its thing
	return (CWindowImpl<CTrayNotifyIcon>::Create(nullptr, CWindow::rcDefault, _T("CTrayNotifyIcon Helper Window"), WS_OVERLAPPEDWINDOW) != nullptr);
}
