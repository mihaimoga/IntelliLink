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

// Static member to track the last assigned link ID
DWORD CLinkSnapshot::m_dwLastID = 0;

/**
 * ProcessHTML - Parses an HTML file to verify the presence of a specific link
 *
 * @param strFileName - Path to the HTML file to process
 * @param strSourceURL - Source URL (currently unused)
 * @param strTargetURL - The target URL to search for in href attributes
 * @param strURLName - The expected link text/name to verify
 * @return true if the link with matching URL and name is found, false otherwise
 *
 * This function reads an HTML file line by line, searches for href attributes,
 * extracts URLs, and verifies if both the URL and link text match the expected values.
 */
bool ProcessHTML(CString strFileName, CString strSourceURL, CString strTargetURL, CString strURLName)
{
	UNREFERENCED_PARAMETER(strSourceURL);

	bool bLinkFound = false;
	CStdioFile inputFile;
	CFileException fileException;

	// Open the HTML file for reading
	if (!inputFile.Open(strFileName, CFile::modeRead | CFile::typeText, &fileException))
	{
		// Log the error and clean up the file
		TCHAR lpszError[MAX_STR_LENGTH] = { 0 };
		fileException.GetErrorMessage(lpszError, MAX_STR_LENGTH);
		OutputDebugString(lpszError);
		DeleteFile(strFileName);
		return false;
	}

	CString strFileLine;
	const CString strHrefTag = _T("href=");
	const CString strQuote = _T("\"");

	// Process each line of the HTML file
	while (inputFile.ReadString(strFileLine))
	{
		int nSearchPos = 0;

		// Search for all href attributes in the current line
		while (true)
		{
			// Find the next occurrence of "href=" tag
			int nHrefPos = strFileLine.Find(strHrefTag, nSearchPos);
			if (nHrefPos == -1)
				break;

			// Find the opening quote after href=
			int nOpenQuote = strFileLine.Find(strQuote, nHrefPos + strHrefTag.GetLength());
			if (nOpenQuote == -1)
			{
				nSearchPos = nHrefPos + strHrefTag.GetLength();
				continue;
			}

			// Find the closing quote to get the complete URL
			int nCloseQuote = strFileLine.Find(strQuote, nOpenQuote + 1);
			if (nCloseQuote == -1)
			{
				nSearchPos = nOpenQuote + 1;
				continue;
			}

			// Extract the URL from between the quotes
			CString strURL = strFileLine.Mid(nOpenQuote + 1, nCloseQuote - nOpenQuote - 1);

			// Check if this is the target URL we're looking for
			if (strURL.CompareNoCase(strTargetURL) == 0)
			{
				TRACE(_T("URL found - %s\n"), static_cast<LPCWSTR>(strTargetURL));

				// Verify the link text matches the expected name (format: >name<)
				CString strLineMark;
				strLineMark.Format(_T(">%s<"), static_cast<LPCWSTR>(strURLName));

				if (strFileLine.Find(strLineMark, nCloseQuote + 1) != -1)
				{
					TRACE(_T("Name found - %s\n"), static_cast<LPCWSTR>(strURLName));
					bLinkFound = true;
					break;
				}
			}

			// Move search position forward to continue searching
			nSearchPos = nCloseQuote + 1;
		}

		// Exit early if link is found
		if (bLinkFound)
			break;
	}

	inputFile.Close();

	// Clean up the temporary file
	if (!DeleteFile(strFileName))
	{
		TRACE(_T("Failed to delete temporary file: %s\n"), static_cast<LPCWSTR>(strFileName));
	}

	return bLinkFound;
}

///////////////////////////////////////////////////////////////////////////////
// CLinkData member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor - Initializes a link data object with default values
 */
CLinkData::CLinkData()
{
	m_dwLinkID = 0;
	m_nPageRank = 0;
	m_bStatus = false;
}

/**
 * Parameterized constructor - Initializes a link data object with specific values
 *
 * @param dwLinkID - Unique identifier for the link
 * @param strSourceURL - URL of the page containing the link
 * @param strTargetURL - URL that the link points to
 * @param strURLName - Display name/text of the link
 * @param nPageRank - PageRank value of the link
 * @param bStatus - Current validation status of the link
 */
CLinkData::CLinkData(DWORD dwLinkID, CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, bool bStatus)
{
	m_dwLinkID = dwLinkID;
	m_strSourceURL = strSourceURL;
	m_strTargetURL = strTargetURL;
	m_strURLName = strURLName;
	m_nPageRank = nPageRank;
	m_bStatus = bStatus;
}

/**
 * Destructor
 */
CLinkData::~CLinkData()
{
}

/**
 * IsValidLink - Validates if the link exists on the source page
 *
 * @return true if the link is found on the source page, false otherwise
 *
 * This function downloads the HTML content from the source URL,
 * saves it to a temporary file, and processes it to verify the link exists.
 */
bool CLinkData::IsValidLink()
{
	bool bRetVal = true;
	TCHAR lpszTempPath[MAX_STR_LENGTH] = { 0 };
	TCHAR lpszTempFile[MAX_STR_LENGTH] = { 0 };

	// Get the system temporary directory
	const DWORD dwTempPath = GetTempPath(MAX_STR_LENGTH, lpszTempPath);
	lpszTempPath[dwTempPath] = '\0';

	// Create a unique temporary file name
	if (GetTempFileName(lpszTempPath, _T("html"), 0, lpszTempFile) != 0)
	{
		TRACE(_T("URLDownloadToFile(%s)...\n"), static_cast<LPCWSTR>(GetSourceURL()));

		// Download the source URL content to the temporary file
		if (URLDownloadToFile(nullptr, GetSourceURL(), lpszTempFile, 0, nullptr) == S_OK)
		{
			// Process the HTML file to verify the link
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

/**
 * Default constructor
 */
CLinkSnapshot::CLinkSnapshot()
{
}

/**
 * Destructor - Cleans up all link data objects
 */
CLinkSnapshot::~CLinkSnapshot()
{
	VERIFY(RemoveAll());
}

/**
 * RemoveAll - Deletes all link data objects and clears the array
 *
 * @return true on success
 */
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

/**
 * Refresh - Refreshes the link snapshot (placeholder for future implementation)
 *
 * @return true on success
 */
bool CLinkSnapshot::Refresh()
{
	return true;
}

/**
 * SelectLink - Finds and returns a link by its ID
 *
 * @param dwLinkID - The unique identifier of the link to find
 * @return Pointer to the CLinkData object if found, nullptr otherwise
 */
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

/**
 * InsertLink - Creates and adds a new link to the snapshot
 *
 * @param strSourceURL - URL of the page containing the link
 * @param strTargetURL - URL that the link points to
 * @param strURLName - Display name/text of the link
 * @param nPageRank - PageRank value of the link
 * @param bStatus - Initial validation status
 * @return The ID assigned to the new link
 */
DWORD CLinkSnapshot::InsertLink(CString strSourceURL, CString strTargetURL, CString strURLName, int nPageRank, bool bStatus)
{
	CLinkData* pLinkData = new CLinkData(++m_dwLastID, strSourceURL, strTargetURL, strURLName, nPageRank, bStatus);
	m_arrLinkList.Add(pLinkData);
	return m_dwLastID;
}

/**
 * DeleteLink - Removes a link from the snapshot by its ID
 *
 * @param dwLinkID - The unique identifier of the link to delete
 * @return true if the link was found and deleted, false otherwise
 */
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

/**
 * utf8_to_wstring - Converts a UTF-8 encoded string to a wide string
 *
 * @param string - The UTF-8 encoded string to convert
 * @return The converted wide string
 * @throws std::runtime_error if conversion fails
 */
std::wstring utf8_to_wstring(const std::string& string)
{
	if (string.empty())
	{
		return L"";
	}

	// Calculate the required buffer size
	const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);
	if (size_needed <= 0)
	{
		throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
	}

	// Perform the conversion
	std::wstring result(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), result.data(), size_needed);
	return result;
}

/**
 * wstring_to_utf8 - Converts a wide string to UTF-8 encoding
 *
 * @param wide_string - The wide string to convert
 * @return The UTF-8 encoded string
 * @throws std::runtime_error if conversion fails
 */
std::string wstring_to_utf8(const std::wstring& wide_string)
{
	if (wide_string.empty())
	{
		return "";
	}

	// Calculate the required buffer size
	const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0)
	{
		throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
	}

	// Perform the conversion
	std::string result(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), result.data(), size_needed, nullptr, nullptr);
	return result;
}

CString GetModuleFileName(_Inout_opt_ DWORD* pdwLastError = nullptr);

/**
 * GetConfigFilePath - Determines the path to the configuration XML file
 *
 * @return The full path to ConfigLink.xml in the user's profile directory,
 *         or in the application directory if profile path is unavailable
 */
const CStringA GetConfigFilePath()
{
	WCHAR* lpszSpecialFolderPath = nullptr;

	// Try to get the user's profile directory
	if ((SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &lpszSpecialFolderPath)) == S_OK)
	{
		std::wstring result(lpszSpecialFolderPath);
		CoTaskMemFree(lpszSpecialFolderPath);
		result += _T("\\ConfigLink.xml");
		return wstring_to_utf8(result).c_str();
	}

	// Fallback: use the application directory
	CString strFilePath{ GetModuleFileName() };
	std::filesystem::path strFullPath{ strFilePath.GetString() };
	strFullPath.replace_filename(_T("ConfigLink"));
	strFullPath.replace_extension(_T(".xml"));
	return wstring_to_utf8(strFullPath).c_str();
}

/**
 * LoadConfig - Loads link data from the XML configuration file
 *
 * @return true if the configuration was successfully loaded, false otherwise
 *
 * This function uses TinyXML2 to parse the XML file and populate
 * the link list with stored link data.
 * Reference: https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/
 */
bool CLinkSnapshot::LoadConfig()
{
	// Clear existing data
	VERIFY(RemoveAll());

	int dwLastID = 0;
	const char* lpszSource = nullptr;
	const char* lpszTarget = nullptr;
	const char* lpszURL_Name = nullptr;
	int nPageRank = 0;

	tinyxml2::XMLDocument xmlDocument;

	// Load the XML configuration file
	if (xmlDocument.LoadFile(GetConfigFilePath()) == tinyxml2::XML_SUCCESS)
	{
		// Get the XML declaration node
		tinyxml2::XMLNode* xmlNode = xmlDocument.FirstChild();
		if (xmlNode != nullptr)
		{
			// Get the root element (APPLICATION_NAME)
			tinyxml2::XMLNode* xmlRoot = xmlNode->NextSibling();
			if (xmlRoot != nullptr)
			{
				// Iterate through all URL elements
				tinyxml2::XMLElement* xmlElement = xmlRoot->FirstChildElement("URL");
				while (xmlElement != nullptr)
				{
					// Read attributes from the URL element
					if ((xmlElement->QueryAttribute("Source", &lpszSource) == tinyxml2::XML_SUCCESS) &&
						(xmlElement->QueryAttribute("Target", &lpszTarget) == tinyxml2::XML_SUCCESS) &&
						(xmlElement->QueryAttribute("Name", &lpszURL_Name) == tinyxml2::XML_SUCCESS) &&
						(xmlElement->QueryAttribute("PageRank", &nPageRank) == tinyxml2::XML_SUCCESS))
					{
						// Convert UTF-8 strings to wide strings
						CString strSourceURL = utf8_to_wstring(lpszSource).c_str();
						CString strTargetURL = utf8_to_wstring(lpszTarget).c_str();
						CString strURL_Name = utf8_to_wstring(lpszURL_Name).c_str();

						// Create and add the link data object
						CLinkData* pLinkData = new CLinkData(dwLastID++, strSourceURL, strTargetURL, strURL_Name, nPageRank, false);
						m_arrLinkList.Add(pLinkData);
					}

					// Move to the next URL element
					xmlElement = xmlElement->NextSiblingElement("URL");
				}
				return true;
			}
		}
	}
	return false;
}

/**
 * SaveConfig - Saves the current link data to the XML configuration file
 *
 * @return true if the configuration was successfully saved, false otherwise
 *
 * This function uses TinyXML2 to create an XML document and serialize
 * all link data to the configuration file.
 * Reference: https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/
 */
bool CLinkSnapshot::SaveConfig()
{
	tinyxml2::XMLDocument xmlDocument;

	// Create the XML declaration
	tinyxml2::XMLNode* xmlNode = xmlDocument.InsertFirstChild(xmlDocument.NewDeclaration());

	// Create the root element
	tinyxml2::XMLElement* xmlRoot = xmlDocument.NewElement(APPLICATION_NAME);
	if (xmlRoot != nullptr)
	{
		xmlDocument.InsertAfterChild(xmlNode, xmlRoot);

		// Iterate through all links and save them
		const int nSize = (int)m_arrLinkList.GetSize();
		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			CLinkData* pLinkData = m_arrLinkList.GetAt(nIndex);
			ASSERT(pLinkData != nullptr);

			// Create a URL element for each link
			tinyxml2::XMLElement* xmlElement = xmlDocument.NewElement("URL");
			if (xmlElement != nullptr)
			{
				// Set attributes with UTF-8 encoded strings
				xmlElement->SetAttribute("Source", wstring_to_utf8(std::wstring(pLinkData->GetSourceURL())).c_str());
				xmlElement->SetAttribute("Target", wstring_to_utf8(std::wstring(pLinkData->GetTargetURL())).c_str());
				xmlElement->SetAttribute("Name", wstring_to_utf8(std::wstring(pLinkData->GetURLName())).c_str());
				xmlElement->SetAttribute("PageRank", pLinkData->GetPageRank());
				xmlElement->SetText((int)pLinkData->GetLinkID());
				xmlRoot->InsertEndChild(xmlElement);
			}
		}

		// Save the document to file
		if (xmlDocument.SaveFile(GetConfigFilePath()) == tinyxml2::XML_SUCCESS)
			return true;
	}
	return false;
}
