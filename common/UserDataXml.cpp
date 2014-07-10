#include <tchar.h>
#include <atlbase.h>
#include "UserDataXml.h"
#include <shlobj.h>
#include "Util.h"
#include "..\include\EncryptExport.h"
#include <comutil.h>

#pragma comment(lib, "comsuppw.lib")

#define  USR_DATA_XML_ENCRYPT_VERSION  1
//
wstring CUserDataXml::GetPersonalDir()
{
	TCHAR szPath[MAX_PATH] = _T("");
	SHGetSpecialFolderPath(NULL,szPath,CSIDL_PERSONAL,TRUE);
	return szPath;
}

wstring CUserDataXml::GetFeihuPersonalDir()
{
	wstring strPath = GetPersonalDir();
	strPath += _T("\\搜狐影音");
	if(_taccess(strPath.c_str(),0) != 0)
		CreateDirectory(strPath.c_str(),NULL);
	return strPath;
}
//
CUserDataXml::CUserDataXml(void)
{
	memset(m_szRootPath, 0, sizeof(m_szRootPath));
	Init();
}

CUserDataXml::~CUserDataXml()
{

}

void CUserDataXml::Init( void )
{
	_tcscpy_s(m_szRootPath, MAX_PATH, GetFeihuPersonalDir().c_str());

	_tcscat(m_szRootPath, _T("\\config"));
	if (_taccess(m_szRootPath, 0) == -1)
	{
		CreateDirectory(m_szRootPath, NULL);
	}

	_tcscat(m_szRootPath, _T("\\UserData.xml"));
	if (_taccess(m_szRootPath, 0) == -1)
	{
		//文件不存在 创建空的文件
		CreatUserDataXml();
	}
	else
	{
		bool bResult = m_rapidxmlDoc.open(m_szRootPath);
		if (!bResult)
		{
			DeleteFile(m_szRootPath); 
			CreatUserDataXml();
		}
		else
		{
			rapidxml::xml_node<wchar_t>* pRoot = m_rapidxmlDoc.first_node(_T("UserData"));  
			if (pRoot == NULL)
			{
				DeleteFile(m_szRootPath); 
				CreatUserDataXml();
			}
		}
	}
}

void CUserDataXml::UnInit( void )
{
	
}

void CUserDataXml::SetValue( LPCTSTR key, LPCTSTR val )
{
#ifndef _NO_USE_ENCRPYT
	if (NULL == val || NULL == key)
	{
		return;
	}
	std::wstring wstrValue;
	BOOL bAddEncrypt = FALSE;

	//判断key是否是pp_mail或者pp_password，如果是则需要加密,否则直接把val赋给wstrValue
	if ((_tcsicmp(key, _T("pp_email")) == 0 || _tcsicmp(key, _T("pp_password")) == 0) && _tcslen(val) > 0)
	{
		_bstr_t     bstrVal   = val;
		std::string strParam  = (char*) bstrVal; //_bstr_t的(char*)操作会调用ConverBSTRToString,从而消除Unicode到ANSI带来的字节数变更影响
		std::string strResult = easy_SHEncrypt(const_cast<char*>(strParam.c_str()), strParam.size(), Key1);
		std::string strBase64 = convert_to_base64(strResult);
		wstrValue = Utf82W(strBase64.c_str(), strBase64.size());
		bAddEncrypt = TRUE;
	}
	else
	{
		wstrValue = val;
	}

	rapidxml::xml_node<wchar_t> *pName = GetRapidXmlNodePtr(key);
	if (pName == NULL)
	{
		CreatUserDataNode(key, val, bAddEncrypt);
	}
	else
	{
		if (bAddEncrypt)
		{
			rapidxml::xml_attribute<wchar_t> *pAttr = pName->first_attribute(_T("encrypt"));
			if (NULL == pAttr)
			{
				pName->append_attribute(m_rapidxmlDoc.allocate_attribute(_T("encrypt"), USR_DATA_XML_ENCRYPT_VERSION));
			}
		}
		SetNodeCData(pName, wstrValue.c_str());
	}
#endif
}

std::wstring CUserDataXml::GetValue(LPCTSTR key)
{
#ifndef _NO_USE_ENCRPYT
	if (NULL == key)
	{
		return _T("");
	}
	rapidxml::xml_node<wchar_t> *pName = GetRapidXmlNodePtr(key);
	if (pName == NULL)
	{
		return _T("");
	}

	//若pName无属性则直接获取，否则先解密再获取
	rapidxml::xml_attribute<wchar_t> *pAttr = pName->first_attribute(_T("encrypt"));
	if (NULL == pAttr || 0 == _tcsicmp(_T("0"), pAttr->value()))
	{
        return GetNodeCData(pName);
	}
	else
	{
		std::wstring wstrRet;
		rapidxml::xml_node<wchar_t> *pCData = pName->first_node();
		if (NULL != pCData && _tcslen(pCData->value()) > 0)
		{
			//将获取的CDATA数据进行解密
			std::wstring wstrVal   = pCData->value();
			_bstr_t      bstrVal   = wstrVal.c_str();
			std::string  strBase64 = convert_from_base64((char*)bstrVal);
			bstrVal = easy_SHDecrypt(const_cast<char*>(strBase64.c_str()), strBase64.size(), Key1).c_str();
            wstrRet = (wchar_t *)bstrVal;
		}
		return wstrRet;
	}
#else
	return L"";
#endif
}

void CUserDataXml::DelValue(LPCTSTR key)
{
	rapidxml::xml_node<wchar_t> *pName = GetRapidXmlNodePtr(key);
	if (pName == NULL)
	{
		return ;
	}
	DelNodeCData(pName);
}

void CUserDataXml::CreatUserDataXml()
{
	CreateRapidXmlFile();
}

BOOL CUserDataXml::CreateRapidXmlFile()
{
	rapidxml::xml_node<wchar_t>* pHead = m_rapidxmlDoc.allocate_node(rapidxml::node_pi, m_rapidxmlDoc.allocate_string(_T("xml version=\"1.0\" encoding=\"utf-8\"")));
	if (pHead == NULL)
	{
		return FALSE;
	}
	m_rapidxmlDoc.append_node(pHead);

	rapidxml::xml_node<wchar_t>* pRoot = m_rapidxmlDoc.allocate_node(rapidxml::node_element, _T("UserData"), _T(""));  
	if (pRoot == NULL)
	{
		return FALSE;
	}
	m_rapidxmlDoc.append_node(pRoot);

	m_rapidxmlDoc.save(m_szRootPath);

	return TRUE;
}

BOOL CUserDataXml::CreatUserDataNode(LPCTSTR key, LPCTSTR val, BOOL bAddEncrypt)
{
	rapidxml::xml_node<wchar_t>* pRoot = m_rapidxmlDoc.first_node(_T("UserData"));  
	if (pRoot == NULL)
	{
		return FALSE;
	}

	rapidxml::xml_node<wchar_t> *pChild = m_rapidxmlDoc.allocate_node(rapidxml::node_element,key, _T(""));
	if (pChild == NULL)
	{
		return FALSE;
	}
	rapidxml::xml_node<wchar_t> *pSubChild = m_rapidxmlDoc.allocate_node(rapidxml::node_cdata,NULL, val);
	if (pSubChild == NULL)
	{
		return FALSE;
	}
	if (bAddEncrypt)
	{
		pChild->append_attribute(m_rapidxmlDoc.allocate_attribute(_T("encrypt"), USR_DATA_XML_ENCRYPT_VERSION));
	}
	pChild->append_node(pSubChild);
	pRoot->append_node(pChild);
	m_rapidxmlDoc.save(m_szRootPath);
	return TRUE;
}

rapidxml::xml_node<wchar_t>* CUserDataXml::GetRapidXmlNodePtr( LPCTSTR key )
{
	rapidxml::xml_node<wchar_t> *pRoot = m_rapidxmlDoc.first_node(_T("UserData"));
	if (pRoot == NULL)
	{
		return NULL;
	}
	rapidxml::xml_node<wchar_t> *pChild = pRoot->first_node(key);

	return pChild;
}

std::wstring CUserDataXml::GetNodeAttribute( rapidxml::xml_node<wchar_t> *pNode, LPCTSTR szName,LPCTSTR defaultVal )
{
	rapidxml::xml_attribute<wchar_t> *pAttribute = pNode->first_attribute(szName);
	if (pAttribute == NULL)
	{
		return defaultVal;
	}
	return pAttribute->value();
}

void CUserDataXml::SetNodeAttribute( rapidxml::xml_node<wchar_t> *pNode, LPCTSTR szName, LPCTSTR szValue )
{
	rapidxml::xml_attribute<wchar_t> *pAttribute = pNode->first_attribute(szName);
	if (pAttribute == NULL)
	{
		return;
	}
	pAttribute->value(m_rapidxmlDoc.allocate_string(szValue));
	m_rapidxmlDoc.save(m_szRootPath);
}

void CUserDataXml::DelRapidXmlNode( rapidxml::xml_node<wchar_t> *pNode )
{
	m_rapidxmlDoc.remove_node(pNode);
	m_rapidxmlDoc.save(m_szRootPath);
}

std::wstring CUserDataXml::GetNodeCData(rapidxml::xml_node<wchar_t> *pNode)
{
	rapidxml::xml_node<wchar_t> *pdata = pNode->first_node();
	if (pdata == NULL)
	{
		return _T("");
	}
	return pdata->value();
}

void CUserDataXml::SetNodeCData(rapidxml::xml_node<wchar_t> *pNode, LPCTSTR val)
{
	rapidxml::xml_node<wchar_t> *pdata = pNode->first_node();
	if (pdata == NULL)
	{
		rapidxml::xml_node<wchar_t> *pSubChild = m_rapidxmlDoc.allocate_node(rapidxml::node_cdata,NULL, val);
		if (pSubChild == NULL)
		{
			return;
		}
		pNode->append_node(pSubChild);
		return;
	}
	pdata->value(m_rapidxmlDoc.allocate_string(val));
	m_rapidxmlDoc.save(m_szRootPath);
}

void CUserDataXml::DelNodeCData(rapidxml::xml_node<wchar_t> *pNode)
{
	rapidxml::xml_node<wchar_t> *pdata = pNode->first_node();
	if (pdata == NULL)
	{
		return;
	}
	m_rapidxmlDoc.remove_node(pNode);
	m_rapidxmlDoc.save(m_szRootPath);
}
