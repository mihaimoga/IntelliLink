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

// LinkPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliLink.h"
#include "LinkPropertiesDlg.h"
#include "afxdialogex.h"

// CLinkPropertiesDlg dialog

IMPLEMENT_DYNAMIC(CLinkPropertiesDlg, CDialogEx)

CLinkPropertiesDlg::CLinkPropertiesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLinkPropertiesDlg::IDD, pParent)
{
}

CLinkPropertiesDlg::~CLinkPropertiesDlg()
{
}

void CLinkPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCE_URL, m_pSourceURL);
	DDX_Control(pDX, IDC_TARGET_URL, m_pTargetURL);
	DDX_Control(pDX, IDC_URL_NAME, m_pURLName);
}

BEGIN_MESSAGE_MAP(CLinkPropertiesDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CLinkPropertiesDlg message handlers


BOOL CLinkPropertiesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pSourceURL.SetWindowText(m_strSourceURL);
	m_pTargetURL.SetWindowText(m_strTargetURL);
	m_pURLName.SetWindowText(m_strURLName);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLinkPropertiesDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CLinkPropertiesDlg::OnOK()
{
	m_pSourceURL.GetWindowText(m_strSourceURL);
	m_pTargetURL.GetWindowText(m_strTargetURL);
	m_pURLName.GetWindowText(m_strURLName);

	CDialogEx::OnOK();
}
