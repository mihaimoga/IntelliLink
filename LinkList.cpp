/* This file is part of IntelliLink application developed by Mihai MOGA.

IntelliLink is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliLink is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliLink.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// LinkData.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliLink.h"
#include "LinkList.h"
#include "Xml.h"

using namespace JWXml;

IMPLEMENT_DYNAMIC(CLinkData, CObject)

IMPLEMENT_DYNAMIC(CLinkSnapshot, CObject)

DWORD CLinkSnapshot::m_dwLastID = 0;

BOOL ProcessHTML(CString strFileName, CString strSourceURL, CString strTargetURL, CString strURLName)
{
	CString strURL;
	CString strFileLine;
	CString strLineMark;
	BOOL bRetVal = FALSE;

	try
	{
		CStdioFile pInputFile(strFileName, CFile::modeRead | CFile::typeText);
		while (pInputFile.ReadString(strFileLine))
		{
			int nIndex = strFileLine.Find(_T("href="), 0);
			while (nIndex >= 0)
			{
				const int nFirst = strFileLine.Find(_T('\"'), nIndex);
				if (nFirst >= 0)
				{
					const int nLast = strFileLine.Find(_T('\"'), nFirst + 1);
					if (nLast >= 0)
					{
						strURL = strFileLine.Mid(nFirst + 1, nLast - nFirst - 1);
						if (strURL.CompareNoCase(strTargetURL) == 0)
						{
							TRACE(_T("URL found - %s\n"), strTargetURL);
							strLineMark.Format(_T(">%s<"), strURLName);
							if (strFileLine.Find(strLineMark, nLast + 1) >= 0)
							{
								TRACE(_T("Name found - %s\n"), strURLName);
								bRetVal = TRUE;
							}
						}
					}
				}
				nIndex = (nFirst == -1) ? -1 : strFileLine.Find(_T("href="), nFirst + 1);
			}
		}
		pInputFile.Close();
	}
	catch (CFileException * pFileException)
	{
		TCHAR lpszError[MAX_STR_LENGTH] = { 0 };
		pFileException->GetErrorMessage(lpszError, MAX_STR_LENGTH);
		pFileException->Delete();
		OutputDebugString(lpszError);
		bRetVal = FALSE;
	}
	VERIFY(DeleteFile(strFileName));
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
// CLinkData member functions
///////////////////////////////////////////////////////////////////////////////

CLinkData::CLinkData()
{
	m_dwLinkID = 0;
	m_nPageRank = 0;
	m_bStatus = FALSE;
}

CLinkData::CLinkData(DWORD dwLinkID, CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, BOOL bStatus)
{
	m_dwLinkID = dwLinkID;
	m_strSourceURL = strSourceURL;
	m_strTargetURL = strTargetURL;
	m_strURLName = strURLName;
	m_nPageRank = nPageRank;
	m_bStatus = bStatus;
}

CLinkData::~CLinkData()
{
}

BOOL CLinkData::IsValidLink()
{
	BOOL bRetVal = TRUE;
	TCHAR lpszTempPath[MAX_STR_LENGTH] = { 0 };
	TCHAR lpszTempFile[MAX_STR_LENGTH] = { 0 };

	const DWORD dwTempPath = GetTempPath(MAX_STR_LENGTH, lpszTempPath);
	lpszTempPath[dwTempPath] = '\0';
	if (GetTempFileName(lpszTempPath, _T("html"), 0, lpszTempFile) != 0)
	{
		TRACE(_T("URLDownloadToFile(%s)...\n"), GetSourceURL());
		if (URLDownloadToFile(NULL, GetSourceURL(), lpszTempFile, 0, NULL) == S_OK)
		{
			if (!ProcessHTML(lpszTempFile, GetSourceURL(), GetTargetURL(), GetURLName()))
			{
				TRACE(_T("ProcessHTML(%s) has failed\n"), lpszTempFile);
				bRetVal = FALSE;
			}
		}
		else
		{
			TRACE(_T("URLDownloadToFile has failed\n"));
			bRetVal = FALSE;
		}
	}
	else
	{
		TRACE(_T("GetTempFileName has failed\n"));
		bRetVal = FALSE;
	}
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
// CLinkSnapshot member functions
///////////////////////////////////////////////////////////////////////////////

CLinkSnapshot::CLinkSnapshot()
{
}

CLinkSnapshot::~CLinkSnapshot()
{
	VERIFY(RemoveAll());
}

BOOL CLinkSnapshot::RemoveAll()
{
	const int nSize = (int)m_arrLinkList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
		ASSERT(pLinkData != NULL);
		delete pLinkData;
		pLinkData = NULL;
	}
	m_arrLinkList.RemoveAll();
	return TRUE;
}

BOOL CLinkSnapshot::Refresh()
{
	return TRUE;
}

CLinkData* CLinkSnapshot::SelectLink(DWORD dwLinkID)
{
	const int nSize = (int)m_arrLinkList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
		ASSERT(pLinkData != NULL);
		if (pLinkData->GetLinkID() == dwLinkID)
		{
			return pLinkData;
		}
	}
	return NULL;
}

DWORD CLinkSnapshot::InsertLink(CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, BOOL bStatus)
{
	CLinkData* pLinkData = new CLinkData(++m_dwLastID, strSourceURL, strTargetURL, strURLName, nPageRank, bStatus);
	m_arrLinkList.Add(pLinkData);
	return m_dwLastID;
}

BOOL CLinkSnapshot::DeleteLink(DWORD dwLinkID)
{
	const int nSize = (int)m_arrLinkList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
		ASSERT(pLinkData != NULL);
		if (pLinkData->GetLinkID() == dwLinkID)
		{
			m_arrLinkList.RemoveAt(nIndex);
			delete pLinkData;
			pLinkData = NULL;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CLinkSnapshot::LoadConfig()
{
	VERIFY(RemoveAll());
	m_dwLastID = 0;
	CXml pXmlDocument;
	if (pXmlDocument.Open(AfxGetApp()->m_pszProfileName))
	{
		CXmlNodePtr pXmlRoot = pXmlDocument.GetRoot();
		// if (pXmlRoot != NULL)
		{
			CXmlNodesPtr pXMLChildren = pXmlRoot->GetChildren();
			// if (pXMLChildren != NULL)
			{
				const int nSize = pXMLChildren->GetCount();
				for (int nIndex = 0; nIndex < nSize; nIndex++)
				{
					CXmlNodePtr pXmlChild = pXMLChildren->GetItem(nIndex);
					//  if (pXmlChild != NULL)
					{
						DWORD dwLastID = pXmlChild->GetValue((DWORD) 0);
						if (m_dwLastID < dwLastID) m_dwLastID = dwLastID;
						CString strSourceURL = pXmlChild->GetAttribute(_T("Source"));
						CString strTargetURL = pXmlChild->GetAttribute(_T("Target"));
						CString strURLName = pXmlChild->GetAttribute(_T("Name"));
						int nPageRank = pXmlChild->GetAttribute(_T("PageRank"), (DWORD) 0);
						CLinkData* pLinkData = new CLinkData(dwLastID, strSourceURL, strTargetURL, strURLName, nPageRank, FALSE);
						m_arrLinkList.Add(pLinkData);
					}
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CLinkSnapshot::SaveConfig()
{
	CXml pXmlDocument;
	VERIFY(pXmlDocument.Create());
	CXmlNodePtr pXmlRoot = pXmlDocument.GetRoot();
	// if (pXmlRoot != NULL)
	{
		const int nSize = (int)m_arrLinkList.GetSize();
		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
			ASSERT(pLinkData != NULL);
			CXmlNodePtr pXmlChild = pXmlRoot->NewChild(_T("URL"));
			// if (pXmlChild != NULL)
			{
				pXmlChild->SetAttribute(_T("Source"), pLinkData->GetSourceURL());
				pXmlChild->SetAttribute(_T("Target"), pLinkData->GetTargetURL());
				pXmlChild->SetAttribute(_T("Name"), pLinkData->GetURLName());
				pXmlChild->SetAttribute(_T("PageRang"), pLinkData->GetPageRank());
				pXmlChild->SetValue(pLinkData->GetLinkID());
			}
		}
		VERIFY(pXmlDocument.SaveWithFormatted(AfxGetApp()->m_pszProfileName));
		return TRUE;
	}
	return FALSE;
}
