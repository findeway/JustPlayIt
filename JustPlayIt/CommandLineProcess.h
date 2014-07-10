#pragma once
#include "Singleton.h"
class CCommandLineProcessor:public SohuTool::SingletonImpl<CCommandLineProcessor>
{
public:
	CCommandLineProcessor(void);
	~CCommandLineProcessor(void);
	bool Process(const wchar_t* szParams);
};