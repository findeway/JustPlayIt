#pragma once

#include <string>
#include "SingletonImpl.h"

class CSyncHttpClient:
	public SingletonImpl<CSyncHttpClient>
{
public:
	CSyncHttpClient(void);
	~CSyncHttpClient(void);
	std::string GetHttpResponse(const wchar_t* url);
	std::wstring GetHost(const wchar_t* url);
};
