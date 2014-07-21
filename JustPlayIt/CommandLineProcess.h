#pragma once
#include "SingletonImpl.h"
class CCommandLineProcessor:public SingletonImpl<CCommandLineProcessor>
{
public:
	CCommandLineProcessor(void);
	~CCommandLineProcessor(void);
	bool Process(const wchar_t* szParams);
};