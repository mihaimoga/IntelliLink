#pragma once

#include "./XmlNode.h"

namespace JWXml
{
	class CXmlNodes
	{
		friend class CXml;
		friend class CXmlNode;
		friend class CXmlNodes;

	public:
		//			 ____________
		//__________| constuctor |______________________________________
		~CXmlNodes(void);
		CXmlNodes(void);
		CXmlNodes( const CXmlNodes & refNodes );
		CXmlNodes( CXmlNodesPtr pNodes );

		//			 __________
		//__________| override |______________________________________
		CXmlNodesPtr operator = (CXmlNodesPtr pNodes);
		CXmlNodes & operator = (const CXmlNodes & refNodes);
		CXmlNodePtr operator[] ( LONG lIndex );
		CXmlNodePtr operator[] ( LPCTSTR lpszName );


		LONG GetCount(void);
		void Release(void);

		CXmlNodePtr GetItem( LONG nIndex );
		CXmlNodePtr GetItem( LPCTSTR lpszName );

	protected:
		CXmlNodes(MSXML2::IXMLDOMNodeListPtr pNodeList);
		MSXML2::IXMLDOMNodeListPtr m_pNodeList;

	};
}