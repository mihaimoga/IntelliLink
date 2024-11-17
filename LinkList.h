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

///////////////////////////////////////////////////////////////////////////////
// CLinkData command target
///////////////////////////////////////////////////////////////////////////////

class CLinkData : public CObject
{
protected:
	DECLARE_DYNAMIC(CLinkData)
	
public:
	CLinkData();
	CLinkData(DWORD dwLinkID, CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, bool bStatus);
	virtual ~CLinkData();

public:
	DWORD GetLinkID() { return m_dwLinkID; }
	void SetLinkID(DWORD dwLinkID) { m_dwLinkID = dwLinkID; }
	CString GetSourceURL() { return m_strSourceURL; }
	void SetSourceURL(CString strSourceURL) { m_strSourceURL = strSourceURL; }
	CString GetTargetURL() { return m_strTargetURL; }
	void SetTargetURL(CString strTargetURL) { m_strTargetURL = strTargetURL; }
	CString GetURLName() { return m_strURLName; }
	void SetURLName(CString strURLName) { m_strURLName = strURLName; }
	int GetPageRank() { return m_nPageRank; }
	void SetPageRank(int nPageRank) { m_nPageRank = nPageRank; }
	bool GetStatus() { return m_bStatus; }
	void SetStatus(bool bStatus) { m_bStatus = bStatus; }

	bool IsValidLink();

protected:
	DWORD m_dwLinkID;
	CString m_strSourceURL;
	CString m_strTargetURL;
	CString m_strURLName;
	int m_nPageRank;
	bool m_bStatus;
};

typedef CArray<CLinkData*> CLinkList;

///////////////////////////////////////////////////////////////////////////////
// CLinkSnapshot command target
///////////////////////////////////////////////////////////////////////////////

class CLinkSnapshot : public CObject
{
protected:
	DECLARE_DYNAMIC(CLinkSnapshot)
	
public:
	CLinkSnapshot();
	virtual ~CLinkSnapshot();

public:
	bool RemoveAll();
	int GetSize() { return (int)m_arrLinkList.GetSize(); }
	CLinkData* GetAt(int nIndex) { return m_arrLinkList.GetAt(nIndex); }

	bool Refresh();
	CLinkData* SelectLink(DWORD dwLinkID);
	DWORD InsertLink(CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, bool bStatus);
	bool DeleteLink(DWORD dwLinkID);

	bool LoadConfig();
	bool SaveConfig();

protected:
	CLinkList m_arrLinkList;
	static DWORD m_dwLastID;
};
