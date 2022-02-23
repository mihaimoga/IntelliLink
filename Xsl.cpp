#include "StdAfx.h"
#include "./Xsl.h"

namespace JWXml
{
	CXsl::CXsl(void)
	{
	}

	CXsl::~CXsl(void)
	{
		Close();
	}

	//-------------------------------------------------------------------------
	// Function Name    :Open
	// Parameter(s)     :LPCTSTR lpszXslFilePath	XSL file path
	// Return           :BOOL
	// Create			:2008-1-16 16:09 Jerry.Wang
	// Memo             :Open the xsl file
	//-------------------------------------------------------------------------
	BOOL CXsl::Open(LPCTSTR lpszXslFilePath)
	{
		Close();

		HRESULT hr = S_OK;

		try
		{
			hr = m_pIXSLTemplate.CreateInstance( __uuidof(MSXML2::XSLTemplate) );
			if( FAILED(hr) ) { ASSERT(FALSE); return FALSE; }

			hr = m_pStyleSheet.CreateInstance( __uuidof(MSXML2::FreeThreadedDOMDocument) );
			if( FAILED(hr) ) { ASSERT(FALSE); return FALSE; }

			VARIANT_BOOL vbSuccessful = VARIANT_TRUE;
			vbSuccessful = m_pStyleSheet->load( _variant_t(lpszXslFilePath) );
			if( vbSuccessful == VARIANT_FALSE ) { ASSERT(FALSE); return FALSE; }

			hr = m_pIXSLTemplate->putref_stylesheet(m_pStyleSheet);
			if( FAILED(hr) ) { VERIFY(FALSE); return FALSE; }

			m_pIXSLProcessor = m_pIXSLTemplate->createProcessor();
			if( m_pIXSLProcessor == NULL ) { ASSERT(FALSE); return FALSE; }

			return TRUE;
		}
		catch( _com_error e)
		{
			TRACE( _T("CXsl::Open(%s) failed:%s\n"), lpszXslFilePath, e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}
	}

	//-------------------------------------------------------------------------
	// Function Name    :Close
	// Parameter(s)     :void
	// Return           :void
	// Create			:2008-1-16 16:10 Jerry.Wang
	// Memo             :Close the xsl file
	//-------------------------------------------------------------------------
	void CXsl::Close(void)
	{
		RELEASE_PTR(m_pIXSLTemplate);
		RELEASE_PTR(m_pStyleSheet);
		RELEASE_PTR(m_pIXSLProcessor);	
	}

	//-------------------------------------------------------------------------
	// Function Name    :AddParameter
	// Parameter(s)     :LPCTSTR lpszParamName
	//					:LPCTSTR lpszParamValue
	//					:LPCTSTR lpszNamespaceURI
	// Return           :BOOL
	// Create			:2008-1-18 22:10 Jerry.Wang
	// Memo             :Add a parameter to the XSLT
	//-------------------------------------------------------------------------
	BOOL CXsl::AddParameter( LPCTSTR lpszParamName, LPCTSTR lpszParamValue, LPCTSTR lpszNamespaceURI /* = _T("") */)
	{
		ASSERT(m_pIXSLProcessor != NULL);
		try
		{
			HRESULT hr = m_pIXSLProcessor->addParameter( _bstr_t(lpszParamName)
				, _variant_t(lpszParamValue)
				, _bstr_t(lpszNamespaceURI)
				);
			return SUCCEEDED(hr);
		}
		catch( _com_error e)
		{
			TRACE( _T("CXsl::AddParameter( %s, %s, %s) failed:%s\n")
				, lpszParamName
				, lpszParamValue
				, lpszNamespaceURI
				, e.ErrorMessage()
				);
			ASSERT( FALSE );
			return FALSE;
		}
	}

	//-------------------------------------------------------------------------
	// Function Name    :TransformToFile
	// Parameter(s)     :CXml & objXml			the CXml instance
	//					:LPCTSTR lpszFilePath	The destination file
	// Return           :BOOL
	// Create			:2008-1-16 16:38 Jerry.Wang
	// Memo             :Transform XML + XSL To File
	//-------------------------------------------------------------------------
	BOOL CXsl::TransformToFile( CXml & objXml, LPCTSTR lpszFilePath)
	{
		HRESULT hr					= S_FALSE;
		IStream * pOutStream		= NULL;
		// LPVOID pOutput				= NULL;
		VARIANT_BOOL vbSuccessful	= VARIANT_TRUE;

		try
		{
			::CreateStreamOnHGlobal( NULL, TRUE, &pOutStream);
			m_pIXSLProcessor->put_output(_variant_t(pOutStream));

			hr = m_pIXSLProcessor->put_input( _variant_t((IUnknown*)objXml.m_pDoc) );
			if( FAILED(hr) ) { ASSERT(FALSE); return FALSE; }

			vbSuccessful = m_pIXSLProcessor->transform();
			if( vbSuccessful == VARIANT_FALSE ) { VERIFY(FALSE); return FALSE; }

			CXml::SaveStreamToFile( pOutStream, lpszFilePath);

			return TRUE;
		}
		catch( _com_error e)
		{
			TRACE( _T("CXsl::Transform failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}
	}
}