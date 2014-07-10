#pragma  once
#include <Windows.h>
#include <string>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "shxmlparser.hpp"
//
using namespace std;
//
class CUserDataXml
{
public:
	static CUserDataXml& Instance(){ static CUserDataXml _Instance; return _Instance;}

	~CUserDataXml();
	//
	void Init(void);
	void UnInit(void);

	void SetValue(LPCTSTR key, LPCTSTR val);
	wstring GetValue(LPCTSTR key);
	void DelValue(LPCTSTR key);

private:
	CUserDataXml();
	void CreatUserDataXml();
	BOOL CreateRapidXmlFile();
	BOOL CreatUserDataNode(LPCTSTR key, LPCTSTR val, BOOL bAddEncrypt = FALSE);
	rapidxml::xml_node<wchar_t>* GetRapidXmlNodePtr(LPCTSTR szPath);
	//
	wstring GetNodeAttribute(rapidxml::xml_node<wchar_t> *pNode, LPCTSTR szName,LPCTSTR defaultVal);
	void	SetNodeAttribute(rapidxml::xml_node<wchar_t> *pNode, LPCTSTR szName, LPCTSTR szValue);
	void DelRapidXmlNode(rapidxml::xml_node<wchar_t> *pNode);
	//
	wstring GetNodeCData(rapidxml::xml_node<wchar_t> *pNode);
	void SetNodeCData(rapidxml::xml_node<wchar_t> *pNode, LPCTSTR val);
	void DelNodeCData(rapidxml::xml_node<wchar_t> *pNode);
	//
	wstring GetPersonalDir();
	wstring GetFeihuPersonalDir();
private:
	TCHAR m_szRootPath[MAX_PATH]; 
	sh_xml_document<sh_xml_encode_type_utf8> m_rapidxmlDoc;
};