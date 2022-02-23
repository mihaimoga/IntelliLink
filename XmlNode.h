#pragma once


#import "msxml3.dll"   // msxml3.dll or msxml4.dll or msxml5.dll or msxml6.dll
#include <memory>

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#ifndef TRACE
#define TRACE ATLTRACE
#endif


namespace MSXML2
{
	struct __declspec(uuid("f6d90f11-9c73-11d3-b32e-00c04f990bb4"))
		DOMDocument;

	struct __declspec(uuid("f5078f1b-c551-11d3-89b9-0000f81fe221"))
		DOMDocument26;

	struct __declspec(uuid("f5078f32-c551-11d3-89b9-0000f81fe221"))
		DOMDocument30;

	struct __declspec(uuid("88d969c0-f192-11d4-a65f-0040963251e5"))
		DOMDocument40;

	struct __declspec(uuid("88d969e5-f192-11d4-a65f-0040963251e5"))
		DOMDocument50;

	struct __declspec(uuid("88d96a05-f192-11d4-a65f-0040963251e5"))
		DOMDocument60;
}




#ifndef RELEASE_PTR
#define RELEASE_PTR(x)	\
	if( x != NULL )		\
		{					\
		x.Release();	\
		x = NULL;		\
		}
#endif


namespace JWXml
{
	class CXml;
	class CXmlNode;
	class CXmlNodes;

#ifdef _HAS_TR1
	typedef std::tr1::shared_ptr<CXmlNode>  CXmlNodePtr;
	typedef std::tr1::shared_ptr<CXmlNodes>  CXmlNodesPtr;
#else
	typedef std::auto_ptr<CXmlNode>  CXmlNodePtr;
	typedef std::auto_ptr<CXmlNodes>  CXmlNodesPtr;
#endif

	
}

#include ".\XmlNodes.h"

namespace JWXml
{
	class CXmlNode
	{
		friend class CXml;
		friend class CXmlNode;
		friend class CXmlNodes;

	protected:		
		MSXML2::IXMLDOMNodePtr   m_pNode;

		CXmlNode( MSXML2::IXMLDOMNodePtr pNode);

		BOOL _GetValue(CString & strValue) const;
		BOOL _SetValue(CString & strValue) const;

		BOOL _GetAttribute( CString & strName, CString & strValue) const;
		BOOL _SetAttribute( CString & strName IN
						  , CString & strValue IN
						  , CString & strPrefix IN
					      , CString & strNamespaceURI IN
						  ) const;

	public:

		//		 _____________
		//______| constructor |___________________________________________________________________
		CXmlNode(void);
		CXmlNode(const CXmlNode & refNode IN);
		CXmlNode(const CXmlNodePtr pNode IN);
		~CXmlNode(void);

		//		 _______________
		//______|	override =	|___________________________________________________________________
		CXmlNodePtr operator = (CXmlNodePtr pNode);
		CXmlNode & operator = (const CXmlNode & refNode);



		BOOL IsNull(void) const; 	// Whether the current element exist
		CString GetName(void) const;// Get the name of the current node
		CXmlNode & Detach(void);	// Detach the current node
		void Release(void);			// Release this node


		//		 _______________
		//______|Parent - Child	|___________________________________________________________________
		CXmlNodePtr GetChild( CString strName, BOOL bBuildIfNotExist = TRUE);
		CXmlNodePtr NewChild( CString strName );
		CXmlNodePtr GetParent(void);
		CXmlNodesPtr GetChildren();
		void AttachChild( CXmlNodePtr & pChildNode);
		void AttachChild( CXmlNode & refChildNode);
		BOOL HasChildren(void);
		BOOL RemoveChildren(void);



		//		 _______________
		//______|	attribute	|___________________________________________________________________
		CString	GetAttribute( CString strName, LPCTSTR lpszDefault = NULL)	const;
		bool	GetAttribute( CString strName, bool bDefault)				const;	
		int		GetAttribute( CString strName, int nDefault)				const;
		long	GetAttribute( CString strName, long lDefault)				const;
		__int64	GetAttribute( CString strName, __int64 llDefault)			const;
		float	GetAttribute( CString strName, float fDefault)				const;	
		double	GetAttribute( CString strName, double dDefault)				const;
		DWORD	GetAttribute( CString strName, DWORD dwDefault)				const;

		BOOL	SetAttribute( CString strName, LPCTSTR lpszValue	, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, bool bValue			, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, int nValue			, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, long lValue			, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, __int64 llValue		, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, float fValue			, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, double dValue		, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));
		BOOL	SetAttribute( CString strName, DWORD dwValue		, CString strPrefix = _T(""), CString strNamespaceURI = _T(""));

		BOOL RemoveAttribute( CString strName );


		//		 _______
		//______| value |___________________________________________________________________	
		CString	GetValue( LPCTSTR lpszDefault = NULL )						const;
		bool	GetValue( bool bDefault )									const;
		int		GetValue( int nDefault)										const;
		long	GetValue( long lDefault)									const;
		__int64	GetValue( __int64 llDefault)								const;
		float	GetValue( float fDefault)									const;
		double	GetValue( double dDefault)									const;
		DWORD	GetValue( DWORD dwDefault)									const;

		BOOL	SetValue( LPCTSTR lpszValue );
		BOOL	SetValue( bool bValue );
		BOOL	SetValue( int nValue );
		BOOL	SetValue( long lValue );
		BOOL	SetValue( __int64 llValue );
		BOOL	SetValue( float fValue );
		BOOL	SetValue( double dValue );
		BOOL	SetValue( DWORD dwValue );


		CXmlNodePtr SelectSingleNode(LPCTSTR lpszPath);
		CXmlNodesPtr SelectNodes(LPCTSTR lpszPath);

		//		 _______________________
		//______|	InnerXml OuterXml	|___________________________________________________________________
		CString GetOuterXml(void) const;
		CString GetInnerXml(void) const;

	};
}
