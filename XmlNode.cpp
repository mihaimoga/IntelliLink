#include "StdAfx.h"
#include "./xmlnode.h"


namespace JWXml
{

	//-------------------------------------------------------------------------
	// Function Name    :_GetValue
	// Parameter(s)     :CString & strValue OUT	value
	// Return           :BOOL					false means failed
	// Memo             :get the text value of the current node
	//					:NOTE: the node should not contain children if u invoke this method
	//-------------------------------------------------------------------------
	BOOL CXmlNode::_GetValue(CString & strValue OUT) const
	{
		ASSERT( !IsNull() );

		HRESULT hr = S_OK;
		try
		{
			BSTR bstr = NULL;
			hr = m_pNode->get_text( &bstr );
			ASSERT( SUCCEEDED(hr) );	
			strValue = (LPCTSTR)_bstr_t( bstr, true);

			if( bstr != NULL )
			{
				SysFreeString(bstr);
				bstr = NULL;
			}
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::_GetValue failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}
		
		return SUCCEEDED(hr);
	}



	//-------------------------------------------------------------------------
	// Function Name    :_SetValue
	// Parameter(s)     :CString & strValue IN
	// Return           :BOOL					false means failed
	// Memo             :Set the text value of the current node
	//-------------------------------------------------------------------------
	BOOL CXmlNode:: _SetValue(CString & strValue IN) const
	{
		ASSERT( !IsNull() );

		HRESULT hr = S_OK;

		try
		{
			hr = m_pNode->put_text( _bstr_t(strValue) );
			ASSERT( SUCCEEDED(hr) );
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::_SetValue failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}
		return SUCCEEDED(hr);
	}



	//-------------------------------------------------------------------------
	// Function Name    :_GetAttribute
	// Parameter(s)     :CString & strName	IN	
	// Return			:CString & strValue OUT	
	// Memo             :Get the attribute value
	//-------------------------------------------------------------------------
	BOOL CXmlNode::_GetAttribute( CString & strName IN, CString & strValue OUT) const
	{
		if( m_pNode == NULL )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		HRESULT hr = S_OK;

		try
		{
			MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
			hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
			if( !SUCCEEDED(hr) )
			{
				ASSERT( FALSE );
				return FALSE;
			}

			MSXML2::IXMLDOMNodePtr pIXMLDOMNode = NULL;
			pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem( _bstr_t(strName) );

			if( pIXMLDOMNode == NULL )
			{
				strValue.Empty();
			}
			else
			{
				VARIANT varValue;
				hr = pIXMLDOMNode->get_nodeValue(&varValue);
				if( !SUCCEEDED(hr) )
				{
					ASSERT( FALSE );
					return FALSE;
				}

				strValue = (LPCTSTR)(_bstr_t)varValue;
			}

			RELEASE_PTR(pIXMLDOMNode);
			RELEASE_PTR(pIXMLDOMNamedNodeMap);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::_GetAttribute failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}

		return TRUE;
	}


	//-------------------------------------------------------------------------
	// Function Name    :_SetAttribute
	// Parameter(s)     :CString & strName	IN	
	//					:CString & strValue IN  
	// Return           :BOOL
	// Memo             :Set the attribute value
	//-------------------------------------------------------------------------
	BOOL CXmlNode::_SetAttribute( CString & strName IN
								, CString & strValue IN
								, CString & strPrefix IN
								, CString & strNamespaceURI IN
								) const
	{
		if( m_pNode == NULL )
		{
			ASSERT(FALSE);
			return FALSE;
		}

		HRESULT hr = S_OK;
		try
		{
			MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
			hr = m_pNode->get_ownerDocument(&pDoc);
			ASSERT( SUCCEEDED(hr) );
			
			CString strFullName;
			strFullName.Format( strPrefix.IsEmpty() ? _T("%s%s") : _T("%s:%s"), static_cast<LPCWSTR>(strPrefix), static_cast<LPCWSTR>(strName));
			
			MSXML2::IXMLDOMAttributePtr pAttribute = NULL;
			pAttribute = pDoc->createNode( _variant_t(_T("attribute")), _bstr_t(strFullName), _bstr_t(strNamespaceURI) );
			pAttribute->Putvalue(_variant_t(strValue));
			ASSERT( pAttribute != NULL );
			
			MSXML2::IXMLDOMElementPtr pElement = static_cast<MSXML2::IXMLDOMElementPtr> (m_pNode);
			hr = pElement->setAttributeNode(pAttribute);
			RELEASE_PTR(pAttribute);
			RELEASE_PTR(pElement);
			RELEASE_PTR(pDoc);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::_SetAttribute failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}

		return SUCCEEDED(hr);
	}





	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :MSXML2::IXMLDOMNodePtr pNode	[in]
	// Memo             :constructor
	//-------------------------------------------------------------------------
	CXmlNode::CXmlNode( MSXML2::IXMLDOMNodePtr pNode IN)
	{
		m_pNode = pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :void
	// Memo             :constructor
	//-------------------------------------------------------------------------
	CXmlNode::CXmlNode(void)
	{
		m_pNode = NULL;	
	}


	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :CXmlNode & refNode [in]
	// Memo             :constructor
	//-------------------------------------------------------------------------
	CXmlNode::CXmlNode( const CXmlNode & refNode IN)
	{
		m_pNode = refNode.m_pNode;
	}

	//-------------------------------------------------------------------------
	// Function Name    :CXmlNode
	// Parameter(s)     :CXmlNode & refNode [in]
	// Memo             :constructor
	//-------------------------------------------------------------------------
	CXmlNode::CXmlNode( const CXmlNodePtr pNode IN)
	{
		m_pNode = pNode->m_pNode;
	}

	//-------------------------------------------------------------------------
	// Function Name    :~CXmlNode
	// Parameter(s)     :void
	// Memo             :destructor 
	//-------------------------------------------------------------------------
	CXmlNode::~CXmlNode(void)
	{
		RELEASE_PTR(m_pNode);
	}


	//-------------------------------------------------------------------------
	// Function Name    :Release
	// Parameter(s)     :void
	// Return           :void
	// Memo             :release the node
	//-------------------------------------------------------------------------
	void CXmlNode::Release(void)
	{
		RELEASE_PTR(m_pNode);
	}




	//-------------------------------------------------------------------------
	// Function Name    :operator =
	// Parameter(s)     :CXmlNodePtr pNode	[in]
	// Return           :CXmlNodePtr
	// Memo             :override the =
	//-------------------------------------------------------------------------
	CXmlNodePtr CXmlNode::operator = (CXmlNodePtr pNode IN)
	{
		RELEASE_PTR(m_pNode);

		m_pNode = pNode->m_pNode;
		return pNode;
	}


	//-------------------------------------------------------------------------
	// Function Name    :operator =
	// Parameter(s)     :CXmlNode & refNode	[in]
	// Return           :CXmlNode &
	// Memo             :override the =
	//-------------------------------------------------------------------------
	CXmlNode & CXmlNode::operator = (const CXmlNode & refNode IN)
	{
		RELEASE_PTR(m_pNode);

		m_pNode = refNode.m_pNode;
		return (*this);
	}


	//-------------------------------------------------------------------------
	// Function Name    :IsNull
	// Parameter(s)     :void
	// Return           :BOOL
	// Memo             :Whether this node exist
	//-------------------------------------------------------------------------
	BOOL CXmlNode::IsNull(void) const
	{
		return m_pNode == NULL;
	}

	//-------------------------------------------------------------------------
	// Function Name    :GetChild
	// Parameter(s)     :CString strName		�ڵ�����
	//					:BOOL bBuildIfNotExist	true: create if the node not exist
	// Return           :The child node pointer
	// Memo             :get the child
	//-------------------------------------------------------------------------
	CXmlNodePtr CXmlNode::GetChild( CString strName, BOOL bBuildIfNotExist /* = TRUE */)
	{
		ASSERT( m_pNode != NULL );

		CXmlNodePtr pChild( new CXmlNode() );

		try
		{
			MSXML2::IXMLDOMNodePtr pChildNode = NULL;
			CString strXPath;
			strName.Replace( _T("'"), _T("''"));
			strXPath.Format( _T("*[local-name(.) = '%s']"), static_cast<LPCWSTR>(strName));
			pChildNode = m_pNode->selectSingleNode(_bstr_t((LPCTSTR)strXPath));

			// create if not exist
			if( pChildNode == NULL && bBuildIfNotExist )
			{
				MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
				HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
				ASSERT( SUCCEEDED(hr) );

				pChildNode = pDoc->createElement( _bstr_t(strName) );
				ASSERT( pChildNode != NULL );

				m_pNode->appendChild(pChildNode);

				RELEASE_PTR(pDoc);
			}

			pChild->m_pNode = pChildNode;	
			RELEASE_PTR(pChildNode);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::GetChild failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}

		return pChild;
	}


	//-------------------------------------------------------------------------
	// Function Name    :NewChild
	// Parameter(s)     :CString strName	the node name
	// Return           :CXmlNodePtr
	// Memo             :create new child
	//-------------------------------------------------------------------------
	CXmlNodePtr CXmlNode::NewChild( CString strName )
	{
		ASSERT( m_pNode != NULL );

		CXmlNodePtr pChild( new CXmlNode() );

		try
		{
			MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
			HRESULT hr = m_pNode->get_ownerDocument(&pDoc);
			ASSERT( SUCCEEDED(hr) );

			MSXML2::IXMLDOMNodePtr pChildNode = NULL;
			pChildNode = pDoc->createElement( _bstr_t(strName) );
			ASSERT( pChildNode != NULL );
			RELEASE_PTR(pDoc);

			m_pNode->appendChild(pChildNode);
			pChild->m_pNode = pChildNode;
			RELEASE_PTR(pChildNode);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::NewChild failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		return pChild;	
	}


	//-------------------------------------------------------------------------
	// Function Name    :AttachChild
	// Parameter(s)     :CXmlNodePtr & pChildNode
	// Return           :void
	// Memo             :Attach a new child
	//-------------------------------------------------------------------------
	void CXmlNode::AttachChild( CXmlNodePtr & pChildNode)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			m_pNode->appendChild( pChildNode->m_pNode );
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::AttachChild failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
	}


	//-------------------------------------------------------------------------
	// Function Name    :AttachChild
	// Parameter(s)     :CXmlNode & refChildNode
	// Return           :void
	// Memo             :Attach a new child
	//-------------------------------------------------------------------------
	void CXmlNode::AttachChild( CXmlNode & refChildNode)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			m_pNode->appendChild( refChildNode.m_pNode );
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::AttachChild failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
	}


	//-------------------------------------------------------------------------
	// Function Name    :GetParent
	// Parameter(s)     :void
	// Return           :CXmlNodePtr
	// Memo             :get the parent node
	//-------------------------------------------------------------------------
	CXmlNodePtr CXmlNode::GetParent(void)
	{
		ASSERT( m_pNode != NULL );

		CXmlNodePtr pParent( new CXmlNode() );

		try
		{
			pParent->m_pNode = m_pNode->GetparentNode();
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::GetParent failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		return pParent;
	}




	//-------------------------------------------------------------------------
	// Function Name    :GetName
	// Parameter(s)     :void
	// Return           :CString
	// Memo             :get the name of the current node
	//-------------------------------------------------------------------------
	CString CXmlNode::GetName(void) const
	{
		ASSERT( m_pNode != NULL );

		CString strRet;

		try
		{
			BSTR bstr = NULL;
			HRESULT hr = m_pNode->get_nodeName(&bstr);
			ASSERT( SUCCEEDED(hr) );	
			strRet = (LPCTSTR)_bstr_t( bstr, true);

			if( bstr != NULL )
			{
				SysFreeString(bstr);
				bstr = NULL;
			}
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::GetName failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}

		return strRet;
	}


	//-------------------------------------------------------------------------
	// Function Name    :RemoveAttribute
	// Parameter(s)     :CString strName IN ��������
	// Return           :BOOL
	// Memo             :remove attribute
	//-------------------------------------------------------------------------
	BOOL CXmlNode::RemoveAttribute( CString strName IN )
	{
		ASSERT( m_pNode != NULL );

		HRESULT hr = S_OK;

		try
		{
			MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;
			hr = m_pNode->get_attributes(&pIXMLDOMNamedNodeMap);
			ASSERT( SUCCEEDED(hr) );

			if( SUCCEEDED(hr) )
				pIXMLDOMNamedNodeMap->removeNamedItem( _bstr_t(strName) );
			ASSERT( SUCCEEDED(hr) );

			RELEASE_PTR(pIXMLDOMNamedNodeMap);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::RemoveAttribute failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}

		
		return SUCCEEDED(hr);
	}



	//-------------------------------------------------------------------------
	// Function Name    :HasChildren
	// Parameter(s)     :void
	// Return           :BOOL
	// Memo             :whether the node has children
	//-------------------------------------------------------------------------
	BOOL CXmlNode::HasChildren(void)
	{
		ASSERT( m_pNode != NULL );

		BOOL bHasChildren = FALSE;
		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
			pNodeList =	m_pNode->selectNodes( _bstr_t(_T("child::*")) );
			ASSERT( pNodeList != NULL );

			bHasChildren = pNodeList->length > 0;
			RELEASE_PTR(pNodeList);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::HasChildren failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		return bHasChildren;
	}



	//-------------------------------------------------------------------------
	// Function Name    :Detach
	// Parameter(s)     :
	// Return           :
	// Memo             :detach the current node from parent
	//-------------------------------------------------------------------------
	CXmlNode & CXmlNode::Detach(void)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			HRESULT hr = S_OK;
			MSXML2::IXMLDOMNodePtr pNode = NULL;
			hr = m_pNode->get_parentNode(&pNode);
			ASSERT( SUCCEEDED(hr) );

			pNode->removeChild(m_pNode);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::Detach failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		return (*this);
	}

	//-------------------------------------------------------------------------
	// Function Name    :RemoveChildren
	// Parameter(s)     :
	// Return           :BOOL
	// Memo             :remove all children
	//-------------------------------------------------------------------------
	BOOL CXmlNode::RemoveChildren(void)
	{
		ASSERT( m_pNode != NULL );

		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = m_pNode->childNodes;

			for( int i = pNodeList->length - 1; i >= 0; i--)
			{
				m_pNode->removeChild( pNodeList->item[i] );
			}

			RELEASE_PTR(pNodeList);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::Remove failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
			return FALSE;
		}

		return TRUE;
	}


	//-------------------------------------------------------------------------
	// Function Name    :GetChildren
	// Parameter(s)     :void
	// Return           :CXmlNodesPtr
	// Memo             :get all children nodes
	//-------------------------------------------------------------------------
	CXmlNodesPtr CXmlNode::GetChildren()
	{
		ASSERT( m_pNode != NULL );

		CXmlNodesPtr pNodes ( new CXmlNodes() );
		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
			pNodeList =	m_pNode->selectNodes(_bstr_t(_T("child::*")));

			pNodes->m_pNodeList = pNodeList;
			RELEASE_PTR(pNodeList);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::GetChildren failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		return pNodes;
	}




	//-------------------------------------------------------------------------
	// Function Name    :SelectSingleNode
	// Parameter(s)     :LPCTSTR lpszPath		XPATH
	// Return           :CXmlNodePtr
	// Memo             :XPath selectSingleNode
	//-------------------------------------------------------------------------
	CXmlNodePtr CXmlNode::SelectSingleNode(LPCTSTR lpszPath)
	{
		ASSERT( m_pNode != NULL );

		CXmlNodePtr pNode ( new CXmlNode() );

		try
		{
			MSXML2::IXMLDOMNodePtr pItem = NULL;
			pItem = m_pNode->selectSingleNode( _bstr_t(lpszPath) );

			pNode->m_pNode = pItem;
			RELEASE_PTR(pItem);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::SelectSingleNode( %s ) failed:%s\n"), lpszPath, e.ErrorMessage());
			ASSERT( FALSE );
		}

		return pNode;
	}



	//-------------------------------------------------------------------------
	// Function Name    :SelectNodes
	// Parameter(s)     :LPCTSTR lpszPath		XPATH
	// Return           :CXmlNodesPtr
	// Memo             :XPath selectNodes
	//-------------------------------------------------------------------------
	CXmlNodesPtr CXmlNode::SelectNodes(LPCTSTR lpszPath)
	{
		ASSERT( m_pNode != NULL );

		CXmlNodesPtr pNodes ( new CXmlNodes() );
		try
		{
			MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
			pNodeList =	m_pNode->selectNodes( _bstr_t(lpszPath) );

			pNodes->m_pNodeList = pNodeList;
			RELEASE_PTR(pNodeList);
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::SelectNodes failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}

		return pNodes;
	}




	//////////////////////////////////////////////////////////////////////////
	// the following methods are getting value

	// get CString value
	CString CXmlNode::GetValue(LPCTSTR lpszValue /* = NULL */ ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() &&
			lpszValue != NULL )
		{
			strValue = lpszValue;
			_SetValue(strValue);
		}

		return strValue;
	}

	// get bool value
	bool CXmlNode::GetValue( bool bDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.CompareNoCase(_T("1")) == 0 )
		{
			return true;
		}
		else if( strValue.CompareNoCase(_T("0")) == 0 )
		{
			return false;
		}
		else
		{
			strValue = bDefault ? "1" : "0";
			_SetValue(strValue);
			return bDefault;
		}	
	}

	// get int value
	int CXmlNode::GetValue( int nDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() )
		{
			strValue.Format( _T("%d"), nDefault);
			_SetValue(strValue);
		}

		return _ttoi(strValue);
	}

	// get long value
	long CXmlNode::GetValue( long lDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() )
		{
			strValue.Format( _T("%ld"), lDefault);
			_SetValue(strValue);
		}

		return _ttol(strValue);
	}

	// get __int64 value
	__int64 CXmlNode::GetValue( __int64 llDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() )
		{
			strValue.Format( _T("%I64d"), llDefault);
			_SetValue(strValue);
		}

		return _ttoi64(strValue);
	}

	// get float value
	float CXmlNode::GetValue( float fDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() )
		{
			strValue.Format( _T("%f"), fDefault);
			_SetValue(strValue);
		}

		return static_cast <float> (_tstof(strValue));
	}

	// get double value
	double CXmlNode::GetValue( double dDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() )
		{
			strValue.Format( _T("%e"), dDefault);
			_SetValue(strValue);
		}

		return _tstof(strValue);
	}

	// get DWORD value
	DWORD CXmlNode::GetValue( DWORD dwDefault ) const
	{
		CString strValue;
		_GetValue(strValue);

		if( strValue.IsEmpty() )
		{
			strValue.Format( _T("%lu"), dwDefault);
			_SetValue(strValue);
		}

		return _tcstoul(strValue, NULL, 10);
	}


	//////////////////////////////////////////////////////////////////////////
	// the following methods are setting value

	// set LPCTSTR value
	BOOL CXmlNode::SetValue( LPCTSTR lpszValue )
	{
		CString strValue(lpszValue);
		return _SetValue(strValue);
	}

	// set bool value
	BOOL CXmlNode::SetValue( bool bValue )
	{
		CString strValue;
		strValue = bValue ? _T("1") : _T("0");

		return _SetValue(strValue);
	}

	// set int value
	BOOL CXmlNode::SetValue( int nValue )
	{
		CString strValue;
		strValue.Format( _T("%d"), nValue);

		return _SetValue(strValue);
	}

	// set long value
	BOOL CXmlNode::SetValue( long lValue )
	{
		CString strValue;
		strValue.Format( _T("%ld"), lValue);

		return _SetValue(strValue);
	}

	// set __int64 value
	BOOL CXmlNode::SetValue( __int64 llValue )
	{
		CString strValue;
		strValue.Format( _T("%I64d"), llValue);

		return _SetValue(strValue);
	}


	// set float value
	BOOL CXmlNode::SetValue( float fValue )
	{
		CString strValue;
		strValue.Format( _T("%f"), fValue);

		return _SetValue(strValue);
	}

	// set double value
	BOOL CXmlNode::SetValue( double dValue )
	{
		CString strValue;
		strValue.Format( _T("%e"), dValue);

		return _SetValue(strValue);
	}

	// set DWORD value
	BOOL CXmlNode::SetValue( DWORD dwValue )
	{
		CString strValue;
		strValue.Format( _T("%lu"), dwValue);

		return _SetValue(strValue);
	}


	//////////////////////////////////////////////////////////////////////////
	// The following methods are getting attribute

	// get LPCTSTR attribute
	CString CXmlNode::GetAttribute( CString strName, LPCTSTR lpszDefault /* = NULL */) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() &&
			lpszDefault != NULL )
		{
			strValue = lpszDefault;
		}
		return strValue;
	}

	// get bool attribute
	bool CXmlNode::GetAttribute( CString strName, bool bDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.CompareNoCase(_T("1")) == 0 )
		{
			return true;
		}
		else if( strValue.CompareNoCase(_T("0")) == 0 )
		{
			return false;
		}
		else
		{
			return bDefault;
		}	
	}

	// get int attribute
	int	CXmlNode::GetAttribute( CString strName, int nDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() )
			strValue.Format( _T("%d"), nDefault);

		return _ttoi(strValue);
	}


	// get long attribute
	long CXmlNode::GetAttribute( CString strName, long lDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() )
			strValue.Format( _T("%ld"), lDefault);

		return _ttol(strValue);
	}

	// get __int64 attribute
	__int64 CXmlNode::GetAttribute( CString strName, __int64 llDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() )
			strValue.Format( _T("%I64d"), llDefault);

		return _ttoi64(strValue);
	}


	// get float attribute
	float CXmlNode::GetAttribute( CString strName, float fDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() )
			strValue.Format( _T("%f"), fDefault);

		return static_cast <float> (_tstof(strValue));
	}

	// get double attribute
	double CXmlNode::GetAttribute( CString strName, double dDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() )
			strValue.Format( _T("%e"), dDefault);

		return _tstof(strValue);
	}

	// get DWORD attribute
	DWORD CXmlNode::GetAttribute( CString strName, DWORD dwDefault) const
	{
		CString strValue;
		_GetAttribute( strName, strValue);

		if( strValue.IsEmpty() )
			strValue.Format( _T("%lu"), dwDefault);

		return _tcstoul(strValue, NULL, 10);
	}


	//////////////////////////////////////////////////////////////////////////
	// The following methods are setting attribute

	// set LPCTSTR attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, LPCTSTR lpszValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue = lpszValue;

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);	
	}

	// set bool attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, bool bValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue = bValue ? _T("1") : _T("0");

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

	// set int attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, int nValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue.Format( _T("%d"), nValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

	// set long attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, long lValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue.Format( _T("%ld"), lValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

	// set __int64 attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, __int64 llValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue.Format( _T("%I64d"), llValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}


	// set float attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, float fValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue.Format( _T("%f"), fValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

	// set double attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, double dValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue.Format( _T("%e"), dValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

	// set DWORD attribute
	BOOL CXmlNode::SetAttribute( CString strName
		, DWORD dwValue
		, CString strPrefix /* = _T("") */
		, CString strNamespaceURI /* = _T("") */
		)
	{
		CString strValue;
		strValue.Format( _T("%lu"), dwValue);

		return _SetAttribute( strName, strValue, strPrefix, strNamespaceURI);
	}

	//-------------------------------------------------------------------------
	// Function Name    :GetOuterHTML
	// Parameter(s)     :void
	// Return           :CString
	// Create			:2008-1-14 11:04 ����
	// Memo             :Get OuterXml
	//-------------------------------------------------------------------------
	CString CXmlNode::GetOuterXml(void) const
	{
		ASSERT(!IsNull());

		CString strRet = _T("");
		try
		{
			if( !IsNull() )
			{
				HRESULT hr = S_OK;
				BSTR bstr = NULL;
				hr = m_pNode->get_xml(&bstr);

				ASSERT( SUCCEEDED(hr) );	
				strRet = (LPCTSTR)_bstr_t( bstr, true);

				if( bstr != NULL )
				{
					SysFreeString(bstr);
					bstr = NULL;
				}
			}
		}
		catch ( _com_error e )
		{
			TRACE( _T("CXmlNode::GetOuterXml failed:%s\n"), e.ErrorMessage());
			ASSERT( FALSE );
		}
		return strRet;
	}// GetOuterXml


	//-------------------------------------------------------------------------
	// Function Name    :GetInnerXml
	// Parameter(s)     :void
	// Return           :CString
	// Create			:2008-1-14 11:37 ����
	// Memo             :get InnerXml
	//-------------------------------------------------------------------------
	CString CXmlNode::GetInnerXml(void) const
	{
		ASSERT(!IsNull());

		CString strRet = GetOuterXml();
		int nFirst = strRet.Find('>');
		int nEnd = strRet.ReverseFind('<');
		if( nFirst > 0 && nEnd > 0 && nEnd > nFirst )
			strRet = strRet.Mid( nFirst + 1, nEnd - nFirst - 1);
		return strRet;
	}// GetOuterXml

}