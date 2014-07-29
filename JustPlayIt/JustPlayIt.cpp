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
	
	CCommandLineProcessor::Instance()->Process(lpstrCmdLine);
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	TCHAR szDir[MAX_PATH] = {0};
	::GetModuleFileName(GetModuleHandle(NULL),szDir,MAX_PATH);
	::PathRemoveFileSpec(szDir);
	std::wstring strSkinPath = szDir;
	strSkinPath += L"\\skins.ui";
	DuiLib::CPaintManagerUI::SetResourceZip(strSkinPath.c_str(),true);
	CJPMainWindow::Instance()->InitPlayer();
	//CJPMainWindow::Instance()->Play(L"E:\\downloadvide\\无间道3.mp4",EMediaType_Local);
	//CJPMainWindow::Instance()->Play(L"http://pl.youku.com/playlist/m3u8?vid=184451263&type=hd2&ts=1404889662&keyframe=0&ep=cyaUHkGFX8gG5yPcjD8bYC%2fncXYGXP8C8huAgNBkA9QtSeC4&sid=140488966203412735519&token=9187&ev=1&oip=1008511208&ctype=12",EMediaType_HLS);
	//CJPMainWindow::Instance()->Play(L"http://111.161.65.69/2/11/91/202874333.0.letv?crypt=75aa7f2e203&b=742&nlh=3072&nlt=45&bf=22&p2p=1&video_type=flv&termid=1&tss=no&geo=CN-2-0-2&tm=1406004000&key=857a949c3b5be56660b026d66fca23ce&platid=1&splatid=101&proxy=2008840783,2007490457&mmsid=874333&playid=0&vtype=16&cvid=1895930965988&hwtype=un&ostype=Windows7&tag=letv&sign=letv&tn=0.10660060402005911&pay=1&iscpn=f9051&rateid=1000&gn=749&buss=16&qos=5&cips=60.28.168.232",EMediaType_Url);
	DuiLib::CPaintManagerUI::MessageLoop();
	ProcessDetach();
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	::CoUninitialize();
	return 0;
}
