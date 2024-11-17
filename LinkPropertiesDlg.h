/* Copyright (C) 2014-2025 Stefan-Mihai MOGA
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

#pragma once

// CLinkPropertiesDlg dialog

class CLinkPropertiesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLinkPropertiesDlg)

public:
	CLinkPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLinkPropertiesDlg();

// Dialog Data
	enum { IDD = IDD_LINKPROPERTIESDIALOG };

protected:
	CEdit m_pSourceURL;
	CEdit m_pTargetURL;
	CEdit m_pURLName;
// Input/Output
public:
	CString m_strSourceURL;
	CString m_strTargetURL;
	CString m_strURLName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
