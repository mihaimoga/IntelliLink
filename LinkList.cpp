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

// LinkData.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliLink.h"
#include "LinkList.h"
#include "tinyxml2.h"

IMPLEMENT_DYNAMIC(CLinkData, CObject)

IMPLEMENT_DYNAMIC(CLinkSnapshot, CObject)

DWORD CLinkSnapshot::m_dwLastID = 0;

bool ProcessHTML(CString strFileName, CString strSourceURL, CString strTargetURL, CString strURLName)
{
	UNREFERENCED_PARAMETER(strSourceURL);
	CString strURL;
	CString strFileLine;
	CString strLineMark;
	bool bRetVal = false;

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
							TRACE(_T("URL found - %s\n"), static_cast<LPCWSTR>(strTargetURL));
							strLineMark.Format(_T(">%s<"), static_cast<LPCWSTR>(strURLName));
							if (strFileLine.Find(strLineMark, nLast + 1) >= 0)
							{
								TRACE(_T("Name found - %s\n"), static_cast<LPCWSTR>(strURLName));
								bRetVal = true;
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
		bRetVal = false;
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
	m_bStatus = false;
}

CLinkData::CLinkData(DWORD dwLinkID, CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, bool bStatus)
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

bool CLinkData::IsValidLink()
{
	bool bRetVal = true;
	TCHAR lpszTempPath[MAX_STR_LENGTH] = { 0 };
	TCHAR lpszTempFile[MAX_STR_LENGTH] = { 0 };

	const DWORD dwTempPath = GetTempPath(MAX_STR_LENGTH, lpszTempPath);
	lpszTempPath[dwTempPath] = '\0';
	if (GetTempFileName(lpszTempPath, _T("html"), 0, lpszTempFile) != 0)
	{
		TRACE(_T("URLDownloadToFile(%s)...\n"), static_cast<LPCWSTR>(GetSourceURL()));
		if (URLDownloadToFile(nullptr, GetSourceURL(), lpszTempFile, 0, nullptr) == S_OK)
		{
			if (!ProcessHTML(lpszTempFile, GetSourceURL(), GetTargetURL(), GetURLName()))
			{
				TRACE(_T("ProcessHTML(%s) has failed\n"), lpszTempFile);
				bRetVal = false;
			}
		}
		else
		{
			TRACE(_T("URLDownloadToFile has failed\n"));
			bRetVal = false;
		}
	}
	else
	{
		TRACE(_T("GetTempFileName has failed\n"));
		bRetVal = false;
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

bool CLinkSnapshot::RemoveAll()
{
	const int nSize = (int)m_arrLinkList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
		ASSERT(pLinkData != nullptr);
		delete pLinkData;
		pLinkData = nullptr;
	}
	m_arrLinkList.RemoveAll();
	return true;
}

bool CLinkSnapshot::Refresh()
{
	return true;
}

CLinkData* CLinkSnapshot::SelectLink(DWORD dwLinkID)
{
	const int nSize = (int)m_arrLinkList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
		ASSERT(pLinkData != nullptr);
		if (pLinkData->GetLinkID() == dwLinkID)
		{
			return pLinkData;
		}
	}
	return nullptr;
}

DWORD CLinkSnapshot::InsertLink(CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, bool bStatus)
{
	CLinkData* pLinkData = new CLinkData(++m_dwLastID, strSourceURL, strTargetURL, strURLName, nPageRank, bStatus);
	m_arrLinkList.Add(pLinkData);
	return m_dwLastID;
}

bool CLinkSnapshot::DeleteLink(DWORD dwLinkID)
{
	const int nSize = (int)m_arrLinkList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
		ASSERT(pLinkData != nullptr);
		if (pLinkData->GetLinkID() == dwLinkID)
		{
			m_arrLinkList.RemoveAt(nIndex);
			delete pLinkData;
			pLinkData = nullptr;
			return true;
		}
	}
	return false;
}

std::wstring utf8_to_wstring(const std::string& string)
{
	if (string.empty())
	{
		return L"";
	}

	const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);
	if (size_needed <= 0)
	{
		throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
	}

	std::wstring result(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), result.data(), size_needed);
	return result;
}

std::string wstring_to_utf8(const std::wstring& wide_string)
{
	if (wide_string.empty())
	{
		return "";
	}

	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0)
	{
		throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
	}

	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), result.data(), size_needed, nullptr, nullptr);
	return result;
}

CString GetModuleFileName(_Inout_opt_ DWORD* pdwLastError = nullptr);
const CStringA GetConfigFilePath()
{
	WCHAR* lpszSpecialFolderPath = nullptr;
	if ((SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &lpszSpecialFolderPath)) == S_OK)
	{
		std::wstring result(lpszSpecialFolderPath);
		CoTaskMemFree(lpszSpecialFolderPath);
		result += _T("\\ConfigLink.xml");
		return wstring_to_utf8(result).c_str();
	}

	CString strFilePath{ GetModuleFileName() };
	std::filesystem::path strFullPath{ strFilePath.GetString() };
	strFullPath.replace_filename(_T("ConfigLink"));
	strFullPath.replace_extension(_T(".xml"));
	return wstring_to_utf8(strFullPath).c_str();
}

bool CLinkSnapshot::LoadConfig()
{
	/* https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/ */
	VERIFY(RemoveAll());
	int dwLastID = 0;
	const char* lpszSource = nullptr;
	const char* lpszTarget = nullptr;
	const char* lpszURL_Name = nullptr;
	int nPageRank = 0;
	tinyxml2::XMLDocument xmlDocument;
	if (xmlDocument.LoadFile(GetConfigFilePath()) == tinyxml2::XML_SUCCESS)
	{
		tinyxml2::XMLNode* xmlNode = xmlDocument.FirstChild();
		if (xmlNode != nullptr)
		{
			tinyxml2::XMLNode* xmlRoot = xmlNode->NextSibling();
			if (xmlRoot != nullptr)
			{
				tinyxml2::XMLElement* xmlElement = xmlRoot->FirstChildElement("URL");
				while (xmlElement != nullptr)
				{
					if ((xmlElement->QueryAttribute("Source", &lpszSource) == tinyxml2::XML_SUCCESS) &&
						(xmlElement->QueryAttribute("Target", &lpszTarget) == tinyxml2::XML_SUCCESS) &&
						(xmlElement->QueryAttribute("Name", &lpszURL_Name) == tinyxml2::XML_SUCCESS) &&
						(xmlElement->QueryAttribute("PageRank", &nPageRank) == tinyxml2::XML_SUCCESS))
					{
						CString strSourceURL = utf8_to_wstring(lpszSource).c_str();
						CString strTargetURL = utf8_to_wstring(lpszTarget).c_str();
						CString strURL_Name = utf8_to_wstring(lpszURL_Name).c_str();
						CLinkData* pLinkData = new CLinkData(dwLastID++, strSourceURL, strTargetURL, strURL_Name, nPageRank, false);
						m_arrLinkList.Add(pLinkData);
					}
					xmlElement = xmlElement->NextSiblingElement("URL");
				}
				return true;
			}
		}
	}
	return false;
}

bool CLinkSnapshot::SaveConfig()
{
	/* https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/ */
	tinyxml2::XMLDocument xmlDocument;
	tinyxml2::XMLNode* xmlNode = xmlDocument.InsertFirstChild(xmlDocument.NewDeclaration());
	tinyxml2::XMLElement* xmlRoot = xmlDocument.NewElement(APPLICATION_NAME);
	if (xmlRoot != nullptr)
	{
		xmlDocument.InsertAfterChild(xmlNode, xmlRoot);
		const int nSize = (int)m_arrLinkList.GetSize();
		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
			ASSERT(pLinkData != nullptr);
			tinyxml2::XMLElement* xmlElement = xmlDocument.NewElement("URL");
			if (xmlElement != nullptr)
			{
				xmlElement->SetAttribute("Source", wstring_to_utf8(std::wstring(pLinkData->GetSourceURL())).c_str());
				xmlElement->SetAttribute("Target", wstring_to_utf8(std::wstring(pLinkData->GetTargetURL())).c_str());
				xmlElement->SetAttribute("Name", wstring_to_utf8(std::wstring(pLinkData->GetURLName())).c_str());
				xmlElement->SetAttribute("PageRank", pLinkData->GetPageRank());
				xmlElement->SetText((int)pLinkData->GetLinkID());
				xmlRoot->InsertEndChild(xmlElement);
			}
		}
		if (xmlDocument.SaveFile(GetConfigFilePath()) == tinyxml2::XML_SUCCESS)
			return true;
	}
	return false;
}
