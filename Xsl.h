#pragma once
#include "./Xml.h"



namespace JWXml
{
	class CXsl
	{
	public:
		CXsl(void);
		~CXsl(void);

		// Open xsl file
		BOOL Open(LPCTSTR lpszXslFilePath);

		// close xsl file
		void Close(void);

		// transform to file
		BOOL TransformToFile( CXml & objXml, LPCTSTR lpszFilePath);

		// add a parameter
		BOOL AddParameter( LPCTSTR lpszParamName, LPCTSTR lpszParamValue, LPCTSTR lpszNamespaceURI = NULL);

	protected:
		MSXML2::IXSLTemplatePtr				m_pIXSLTemplate;
		MSXML2::IXMLDOMDocument2Ptr			m_pStyleSheet;
		MSXML2::IXSLProcessorPtr			m_pIXSLProcessor;
	};
}