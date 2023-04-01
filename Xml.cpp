#include "StdAfx.h"
#include "./xml.h"
#include <vector>


namespace JWXml
{
	CXml::CXml(void)
	: m_strFilePath(_T(""))
	, m_pDoc(NULL)
	, m_emVersion(MSXML_UNKNOWN)
	{
	}

	CXml::~CXml(void)
	{
		Close();
	}

	//-------------------------------------------------------------------------
	// Function Name    :IsFileExist	[static]
	// Parameter(s)     :CString strFilePath
	// Return           :BOOL
	// Memo             :Whether the file exist
	//-------------------------------------------------------------------------
	BOOL CXml::IsFileExist(CString strFilePath)
	{
		BOOL bExist = FALSE;
		HANDLE hFile = NULL;

		hFile = CreateFile( strFilePath
			, GENERIC_READ
			, FILE_SHARE_READ | FILE_SHARE_WRITE
			, NULL
			, OPEN_EXISTING
			, 0
			, 0
			);

		if( hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle( hFile );
			bExist = TRUE;
		}

		return (bExist);
	}


	//-------------------------------------------------------------------------
	// Function Name    :CreateInstance
	// Parameter(s)     :
	// Return           :
	// Create			:2007-8-2 9:22 Jerry.Wang
	// Memo             :Create the MSXML instance
	//-------------------------------------------------------------------------
	BOOL CXml::CreateInstance(void)
	{
		Close();

		HRESULT hr = S_FALSE;

		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML60 : m_emVersion;	
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument30) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML30 : m_emVersion;
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument50) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML50 : m_emVersion;
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument40) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML40 : m_emVersion;		
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument26) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML26 : m_emVersion;
		hr = (hr == S_OK) ? hr : m_pDoc.CreateInstance( __uuidof(MSXML2::DOMDocument) );
		m_emVersion = (m_emVersion == 0 && hr == S_OK) ? MSXML_UNKNOWN : m_emVersion;
		if( hr != S_OK ) 
		{
			// did u CoInitialize ?
			// did u install MSXML?
			ASSERT(FALSE);
			return FALSE;
		}

		m_pDoc->setProperty( _bstr_t(_T("SelectionLanguage")), _variant_t(_T("XPath")) );	// 3.0 only
		if( m_emVersion == MSXML40 )
			m_pDoc->setProperty( _bstr_t(_T("NewParser")), _variant_t(true) );

		m_pDoc->setProperty( _bstr_t(_T("AllowXsltScript")), _variant_t(true));
		m_pDoc->setProperty( _bstr_t(_T("AllowDocumentFunction")), _variant_t(true));
		m_pDoc->resolveExternals = VARIANT_TRUE; 
		m_pDoc->preserveWhiteSpace = VARIANT_FALSE;
		m_pDoc->validateOnParse = VARIANT_FALSE;
		m_strFilePath = _T("");
		m_mpNamespace.clear();

		return TRUE;
	}

	//-------------------------------------------------------------------------
	// Function Name    :Open
	// Parameter(s)     :LPCTSTR lpszXmlFilePath
	//					:LPCTSTR lpszRootName		The name of the root element if create
	// Return           :BOOL		false when failed
	// Memo             :open xml file, if not exist then create a new one
	//-------------------------------------------------------------------------
	BOOL CXml::Open( LPCTSTR lpszXmlFilePath )
	{
		// HRESULT hr = S_FALSE;

		if( !IsFileExist(lpszXmlFilePath) )
			return FALSE;

		if( !CreateInstance() )
			return FALSE;

		m_strFilePath = lpszXmlFilePath;
		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

		try
		{
			vbSuccessful = m_pDoc->load( _variant_t(lpszXmlFilePath) );
		}
		catch( _com_error e )
		{
			TRACE( _T("CXml::Open( %s ) failed:%s\n"), lpszXmlFilePath, e.ErrorMessage());
			vbSuccessful = FALSE;
			ASSERT( FALSE );
		}
		catch(...)
		{
			vbSuccessful = FALSE;
			ASSERT( FALSE );
		}

		return (vbSuccessful == VARIANT_TRUE);
	}

	//-------------------------------------------------------------------------
	// Function Name    :Create
	// Parameter(s)     :lpszRootName		The name of the root node
	//					:lpszPrefix			The prefix of the root node [optional]
	//					:lpszNamespaceURI	The namespace URL of the root node [optional]
	// Return           :BOOL
	// Create			:2008-1-16 14:22 Jerry.Wang
	// Memo             :Create a new xml file
	//-------------------------------------------------------------------------
	BOOL CXml::Create( LPCTSTR lpszRootName /* = _T("xmlRoot") */
		, LPCTSTR lpszPrefix /* = _T("") */
		, LPCTSTR lpszNamespaceURI /* = _T("") */
		)
	{
		HRESULT hr = S_FALSE;

		if( !CreateInstance() )
			return FALSE;

		m_strFilePath = _T("");
		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

		try
		{
			MSXML2::IXMLDOMProcessingInstructionPtr pPI = NULL;
			pPI = m_pDoc->createProcessingInstruction( _bstr_t(_T("xml")), _bstr_t(_T("version=\"1.0\" ")) );
			if( pPI == NULL )
			{
				ASSERT(FALSE);
				return FALSE;
			}

			m_pDoc->appendChild(pPI);

			// create the root element
			CXmlNode root = CreateNode( lpszRootName, lpszPrefix, lpszNamespaceURI);
			m_pDoc->appendChild(root.m_pNode);

			vbSuccessful = SUCCEEDED(hr) ? VARIANT_TRUE : VARIANT_FALSE;
		}
		catch( _com_error e )
		{
			TRACE( _T("CXml::Create( %s, %s, %s) failed:%s\n")
				, lpszRootName
				, lpszPrefix
				, lpszNamespaceURI
				, e.ErrorMessage()
				);
			vbSuccessful = FALSE;
			ASSERT( FALSE );
		}
		catch(...)
		{
			vbSuccessful = FALSE;
			ASSERT( FALSE );
		}

		return (vbSuccessful == VARIANT_TRUE);
	}

	//-------------------------------------------------------------------------
	// Function Name    :LoadXml
	// Parameter(s)     :LPCTSTR lpszXmlContent
	// Return           :BOOL
	// Memo             :Load the xml content
	//-------------------------------------------------------------------------
	BOOL CXml::LoadXml(LPCTSTR lpszXmlContent)
	{
		if( !CreateInstance() )
			return FALSE;

		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

		try
		{
			vbSuccessful = m_pDoc->loadXML( _bstr_t(lpszXmlContent) );
		}
		catch( _com_error e )
		{
			TRACE( _T("CXml::LoadXml failed:%s\n"), e.ErrorMessage());
			vbSuccessful = FALSE;
			ASSERT( FALSE );
		}
		catch(...)
		{
			vbSuccessful = FALSE;
			ASSERT( FALSE );
		}

		return (vbSuccessful == VARIANT_TRUE);
	}


	//-------------------------------------------------------------------------
	// Function Name    :Close
	// Parameter(s)     :void
	// Return           :void
	// Memo             :Release
	//-------------------------------------------------------------------------
	void CXml::Close(void)
	{
		m_mpNamespace.clear();
		m_strFilePath = _T("");
		m_emVersion = MSXML_UNKNOWN;
		RELEASE_PTR(m_pDoc);
	}


	

	//-------------------------------------------------------------------------
	// Function Name    :GetXmlFile
	// Parameter(s)     :void
	// Return           :CString
	// Create			:2008-1-16 10:58 Jerry.Wang
	// Memo             :Get the path of the xml file
	//-------------------------------------------------------------------------
	CString CXml::GetXmlFile(void) const
	{
		return m_strFilePath;
	}


	//-------------------------------------------------------------------------
	// Function Name    :CreateNode
	// Parameter(s)     :LPCTSTR lpszName			node local name
	//					:LPCTSTR lpszPrefix			node prefix
	//					:LPCTSTR lpszNamespaceURI	namespace URI
	// Return           :CXmlNodePtr
	// Create			:2007-8-2 9:59 Jerry.Wang
	//-------------------------------------------------------------------------
	CXmlNodePtr CXml::CreateNode(LPCTSTR lpszName
		, LPCTSTR lpszPrefix /* = _T("") */
		, LPCTSTR lpszNamespaceURI /* = _T("") */
		)
	{
		ASSERT( m_pDoc != NULL );
		
		CXmlNodePtr pChild( new CXmlNode() );

		CString strName;
		strName.Format( _tcslen(lpszPrefix) > 0 ? _T("%s:%s") : _T("%s%s"), lpszPrefix, lpszName);
		try
		{	
			MSXML2::IXMLDOMNodePtr pChildNode = NULL;
			pChildNode = m_pDoc->createNode(_variant_t(_T("element")), _bstr_t(strName), _bstr_t(lpszNamespaceURI) );
			ASSERT( pChildNode != NULL );
			pChild->m_pNode = pChildNode;
			RELEASE_PTR(pChildNode);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::CreateNode( %s, %s, %s) failed:%s\n"), lpszName, lpszPrefix, lpszNamespaceURI, e.ErrorMessage());
			ASSERT( FALSE );
		}
		return pChild;
	}



	//-------------------------------------------------------------------------
	// Function Name    :AddSelectionNamespace
	// Parameter(s)     :LPCTSTR lpszPrefix	The prefix, should not contain xmlns
	//					:LPCTSTR lpszURI
	// Return           :
	// Create			:2007-8-1 18:06 Jerry.Wang
	// Memo             :Add the selection namespace for the XPath
	//-------------------------------------------------------------------------
	void CXml::AddSelectionNamespace( LPCTSTR lpszPrefix, LPCTSTR lpszURI)
	{
		ASSERT( m_pDoc != NULL );

		try
		{
			m_mpNamespace[lpszPrefix] = lpszURI;
			CString strNamespaces = _T("");
			std::map< CString, CString>::iterator iter;
			for( iter = m_mpNamespace.begin(); iter != m_mpNamespace.end(); iter++)
			{
				CString strNamespace;
				strNamespace.Format( _T("xmlns:%s='%s' "), static_cast<LPCWSTR>(iter->first), static_cast<LPCWSTR>(iter->second));

				strNamespaces += strNamespace;
			}

			m_pDoc->setProperty( _T("SelectionNamespaces"), _variant_t((LPCTSTR)strNamespaces));
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::AddNamespace( %s, %s) failed:%s\n"), lpszPrefix, lpszURI, e.ErrorMessage());
			ASSERT( FALSE );
		}
	}

	
	//-------------------------------------------------------------------------
	// Function Name    :GetRoot
	// Parameter(s)     :
	// Return           :
	// Memo             :get the root element
	//-------------------------------------------------------------------------
	CXmlNodePtr CXml::GetRoot(void)
	{	
		ASSERT( m_pDoc != NULL );

		CXmlNodePtr pNode( new CXmlNode() );

		try
		{
			MSXML2::IXMLDOMElementPtr pElement = NULL;
			HRESULT hr = m_pDoc->get_documentElement(&pElement);
			ASSERT( SUCCEEDED(hr) );
			ASSERT( pElement != NULL );

			pNode->m_pNode = pElement;
			RELEASE_PTR(pElement);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::GetRoot failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		
		return pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :SaveWithFormatted
	// Parameter(s)     :LPCTSTR lpszFilePath	[in, optional] 
	// Return           :BOOL
	// Memo             :Save the xml file
	//-------------------------------------------------------------------------
	BOOL CXml::Save(LPCTSTR lpszFilePath)
	{
		if( m_pDoc == NULL )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		HRESULT hr = S_OK;

		try
		{
			if( lpszFilePath == NULL )
			{
				ASSERT(!m_strFilePath.IsEmpty());
				hr = m_pDoc->save( _variant_t((LPCTSTR)m_strFilePath) );
			}
			else
			{
				hr = m_pDoc->save( _variant_t( lpszFilePath ) );
				if( SUCCEEDED(hr) )
					m_strFilePath = lpszFilePath;
			}
			return SUCCEEDED(hr);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::Save( %s ) failed:%s\n"), lpszFilePath, e.ErrorMessage());
			ASSERT( FALSE );
			hr = e.Error();
		}

		return SUCCEEDED(hr);
	}

	//-------------------------------------------------------------------------
	// Function Name    :SaveWithFormatted
	// Parameter(s)     :LPCTSTR lpszFilePath	[in, optional] 
	//					:LPCTSTR lpszEncoding	[in, optional] 
	// Return           :BOOL
	// Memo             :Save the xml file in formatted
	// History			:V3.1 Thanks roel_'s advice to support formatted output
	//-------------------------------------------------------------------------
	BOOL CXml::SaveWithFormatted(LPCTSTR lpszFilePath /* = NULL */, LPCTSTR lpszEncoding /* = _T("UTF-8") */)
	{
		if( m_pDoc == NULL )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		HRESULT hr = S_OK;

		MSXML2::IMXWriterPtr pMXWriter = NULL;
		MSXML2::ISAXXMLReaderPtr pSAXReader = NULL;
		MSXML2::ISAXContentHandlerPtr pISAXContentHandler = NULL;
		MSXML2::ISAXErrorHandlerPtr pISAXErrorHandler = NULL;
		MSXML2::ISAXDTDHandlerPtr pISAXDTDHandler = NULL;
		try
		{
		
			// create
			hr = pMXWriter.CreateInstance( __uuidof(MSXML2::MXXMLWriter));
			if( FAILED(hr) ){ ASSERT(FALSE); goto lblErrorHandlerForSave; }
			
			hr = pSAXReader.CreateInstance( __uuidof(MSXML2::SAXXMLReader));
			if( FAILED(hr) ){ ASSERT(FALSE); goto lblErrorHandlerForSave; }

			// save in formatted
			pISAXContentHandler = pMXWriter;
			pISAXErrorHandler = pMXWriter;
			pISAXDTDHandler = pMXWriter;

			if (FAILED (pMXWriter->put_omitXMLDeclaration (VARIANT_FALSE)) ||
				FAILED (pMXWriter->put_standalone (VARIANT_FALSE)) ||
				FAILED (pMXWriter->put_indent (VARIANT_TRUE)) ||
				FAILED (pMXWriter->put_encoding(_bstr_t(lpszEncoding)) )
				)
			{
				ASSERT(FALSE);
				goto lblErrorHandlerForSave;
			}

			if (FAILED(pSAXReader->putContentHandler (pISAXContentHandler)) ||
				FAILED(pSAXReader->putDTDHandler (pISAXDTDHandler)) ||
				FAILED(pSAXReader->putErrorHandler (pISAXErrorHandler)) ||
				FAILED(pSAXReader->putProperty ((unsigned short*)L"http://xml.org/sax/properties/lexical-handler", _variant_t(pMXWriter.GetInterfacePtr()))) ||
				FAILED(pSAXReader->putProperty ((unsigned short*)L"http://xml.org/sax/properties/declaration-handler", _variant_t(pMXWriter.GetInterfacePtr()))))
			{
				ASSERT(FALSE);
				goto lblErrorHandlerForSave;
			}
						
			IStream * pOutStream = NULL;
			::CreateStreamOnHGlobal( NULL, TRUE, &pOutStream);
			hr = pMXWriter->put_output(_variant_t(pOutStream));
			if( FAILED(hr) ){ ASSERT(FALSE); goto lblErrorHandlerForSave; }

			hr = pSAXReader->parse(m_pDoc.GetInterfacePtr());
			if( FAILED(hr) ){ ASSERT(FALSE); goto lblErrorHandlerForSave; }
					
			if( lpszFilePath == NULL )
			{
				ASSERT(!m_strFilePath.IsEmpty());
				if( !CXml::SaveStreamToFile( pOutStream, m_strFilePath) )
					return FALSE;
			}
			else
			{
				if( !CXml::SaveStreamToFile( pOutStream, lpszFilePath) )
					return FALSE;
				m_strFilePath = lpszFilePath;
			}			
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::SaveWithFormatted( %s ) failed:%s\n"), lpszFilePath, e.ErrorMessage());
			ASSERT( FALSE );
			hr = e.Error();
		}

lblErrorHandlerForSave:
		RELEASE_PTR(pISAXDTDHandler);
		RELEASE_PTR(pISAXErrorHandler);
		RELEASE_PTR(pISAXContentHandler);
		RELEASE_PTR(pSAXReader);
		RELEASE_PTR(pMXWriter);

		return SUCCEEDED(hr);
	}

	//-------------------------------------------------------------------------
	// Function Name    :SaveStreamToFile
	// Parameter(s)     :IStream * pStream
	//					:LPCTSTR lpszFilePath
	// Return           :BOOL
	// Memo             :Save IStream to file, this method is inner used by JWXml
	//-------------------------------------------------------------------------
	BOOL CXml::SaveStreamToFile(IStream * pStream, LPCTSTR lpszFilePath)
	{
		HRESULT hr = S_OK;
		LPVOID pOutput = NULL;
		HGLOBAL hGlobal = NULL;

		hr = ::GetHGlobalFromStream( pStream, &hGlobal);
		if( FAILED(hr) ) { ASSERT(FALSE); return FALSE; }
		::GlobalUnlock(hGlobal);
		LARGE_INTEGER llStart = {0, 0};
		ULARGE_INTEGER ullSize = {0, 0};
		pStream->Seek( llStart, STREAM_SEEK_CUR, &ullSize);
		pOutput = ::GlobalLock(hGlobal);

		HANDLE hFile = ::CreateFile( lpszFilePath
			, GENERIC_WRITE
			, FILE_SHARE_WRITE | FILE_SHARE_READ
			, NULL
			, CREATE_ALWAYS
			, 0
			, NULL
			);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		DWORD dwWritten = 0;
		::WriteFile( hFile, pOutput, (UINT)ullSize.QuadPart, &dwWritten, NULL);
		::FlushFileBuffers(hFile);
		::CloseHandle(hFile);

		::GlobalUnlock(hGlobal);

		return TRUE;
	}



	//-------------------------------------------------------------------------
	// Function Name    :SelectSingleNode
	// Parameter(s)     :LPCTSTR strPath 
	// Return           :CXmlNodePtr
	// Memo             :Query node by XPath
	//-------------------------------------------------------------------------
	CXmlNodePtr CXml::SelectSingleNode(LPCTSTR lpszPath)
	{
		ASSERT( m_pDoc != NULL );

		CXmlNodePtr pNode ( new CXmlNode() );

		if( !GetRoot()->IsNull() )
			(*pNode) = GetRoot()->SelectSingleNode(lpszPath);
		
		return pNode;
	}

	//-------------------------------------------------------------------------
	// Function Name    :SelectNodes
	// Parameter(s)     :LPCTSTR strPath	
	// Return           :CXmlNodesPtr
	// Memo             :Query nodes by XPath
	//-------------------------------------------------------------------------
	CXmlNodesPtr CXml::SelectNodes(LPCTSTR lpszPath)
	{
		ASSERT( m_pDoc != NULL );

		CXmlNodesPtr pNodes( new CXmlNodes() );

		if( !GetRoot()->IsNull() )
			(*pNodes) = GetRoot()->SelectNodes(lpszPath);

		return pNodes;
	}



	//-------------------------------------------------------------------------
	// Function Name    :EncodeBase64
	// Parameter(s)     :LPBYTE *pBuf	The binary buffer
	//					:ULONG ulSize	size
	// Return           :CString		the result
	// Memo             :encoding the binary buffer into Base64 string
	//-------------------------------------------------------------------------
	CString CXml::Base64Encode(LPBYTE pBuf, ULONG ulSize)
	{
		ASSERT( m_pDoc != NULL );
		ASSERT( pBuf != NULL );
		
		CString strRet = _T("");
		try
		{
			MSXML2::IXMLDOMElementPtr pElement = NULL;
			pElement = m_pDoc->createElement( _bstr_t(_T("Base64")) );
			ASSERT( pElement != NULL );

			HRESULT hr = S_OK;
			hr = pElement->put_dataType( _bstr_t(_T("bin.base64")) );
			ASSERT( SUCCEEDED(hr) );

			SAFEARRAY * pAry = SafeArrayCreateVector( VT_UI1, 0L, ulSize);
			::memcpy( pAry->pvData, pBuf, ulSize);

			VARIANT var;
			VariantInit(&var);
			var.parray = pAry;
			var.vt = VT_ARRAY | VT_UI1;
			pElement->nodeTypedValue = var;

			BSTR bstr = NULL;
			hr = pElement->get_text( &bstr );
			ASSERT( SUCCEEDED(hr) );	
			strRet = (LPCTSTR)_bstr_t( bstr, true);

			if( bstr != NULL )
			{
				SysFreeString(bstr);
				bstr = NULL;
			}

			RELEASE_PTR(pElement);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::Base64Encode failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}

		return strRet;
	}



	//-------------------------------------------------------------------------
	// Function Name    :Base64Decode
	// Parameter(s)     :CString strIn		The base64 string
	//					:LPBYTE pBuf		The output buffer
	//					:ULONG * ulSize		the site of buffer
	// Return           :BOOL
	// Memo             :Decode the base64 string into buffer
	//					:NOTE: If the pBuf equal to NULL, then the lSize is the buffer length
	//					:Please see demo
	//-------------------------------------------------------------------------
	BOOL CXml::Base64Decode(CString strIn IN, LPBYTE pBuf IN OUT, LONG & lSize IN OUT)
	{
		ASSERT( m_pDoc != NULL );
		
		try
		{
			MSXML2::IXMLDOMElementPtr pElement = NULL;
			pElement = m_pDoc->createElement( _bstr_t(_T("Base64")) );
			ASSERT( pElement != NULL );

			HRESULT hr = S_OK;
			hr = pElement->put_dataType( _bstr_t(_T("bin.base64")) );
			ASSERT( SUCCEEDED(hr) );

			hr = pElement->put_text( _bstr_t(strIn) );
			ASSERT( SUCCEEDED(hr) );

			hr = SafeArrayGetUBound( pElement->nodeTypedValue.parray, 1, &lSize);
			ASSERT( SUCCEEDED(hr) );
			lSize ++;
			if( pBuf )
			{
				memset( pBuf, 0, lSize);

				memcpy( pBuf, LPVOID(pElement->nodeTypedValue.parray->pvData), lSize);	
			}		

			RELEASE_PTR(pElement);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXml::Base64Decode failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}
		
		return TRUE;
	}

	//-------------------------------------------------------------------------
	// Function Name    :GetVersion
	// Parameter(s)     :void
	// Return           :MSXML_VERSION
	// Create			:2008-1-16 15:05 Jerry.Wang
	// Memo             :Get the version of current MSXML
	//-------------------------------------------------------------------------
	MSXML_VERSION CXml::GetVersion(void) const
	{
		return m_emVersion;
	}

}