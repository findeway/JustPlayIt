#pragma once

#include "SohuToolExport.h"
#include <string>
#include "SohuToolBase.h"

namespace SohuTool
{
	SOHUTOOL_API char*  WINAPI convert_to_base64(const char* buf,int len,int& outlen);
	SOHUTOOL_API char*  WINAPI convert_from_base64(const char* base64,int len,int& outlen);

	inline std::string easy_convert_to_base64(const std::string& str)
	{
		char *szBase64 = NULL;
		int  len = 0;
		szBase64 = convert_to_base64(str.c_str(),str.size(),len);
		std::string strBase64(szBase64,len);
		ReleaseBuffer(szBase64);
		return strBase64;
	}

	inline std::string easy_convert_from_base64(const std::string& str)
	{
		char *szData = NULL;
		int  len = 0;
		szData = convert_from_base64(str.c_str(),str.size(),len);
		std::string strData(szData,len);
		ReleaseBuffer(szData);
		return strData;
	}
}