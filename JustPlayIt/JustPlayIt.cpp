// JustPlayIt.cpp : main source file for JustPlayIt.exe
//

#include "stdafx.h"
#include "resource.h"

#include "DllCheck.h"
#include "CommandLineProcess.h"
#include "JPMainWindow.h"

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	ProcessAttach();
	typedef BOOL (WINAPI *ChangeWindowMessageFilter_PROC)(UINT , DWORD);
	ChangeWindowMessageFilter_PROC pfnChangeWindowMessageFilter;
	pfnChangeWindowMessageFilter = (ChangeWindowMessageFilter_PROC)::GetProcAddress(::GetModuleHandle(_T("USER32")),"ChangeWindowMessageFilter");
	if(pfnChangeWindowMessageFilter)
	{
		pfnChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
		pfnChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
		pfnChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);
	}
	
	CCommandLineProcessor::Instance().Process(lpstrCmdLine);
	DuiLib::CPaintManagerUI::SetInstance(hInstance);

	CJPMainWindow::Instance().Play(L"e:\\downloadvide\\无间道3.mp4");

	DuiLib::CPaintManagerUI::MessageLoop();
	ProcessDetach();
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	::CoUninitialize();
	return 0;
}
