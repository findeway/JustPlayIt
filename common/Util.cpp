#include <Windows.h>
#include <atlbase.h>
#include "Util.h"
#include <algorithm>
#include <atlcomcli.h>
#include <atlcrypt.h>
#include <dbt.h>						// For DeviceChange.
#include <winioctl.h>					// For DeviceIOCtl.
#include <TLHELP32.h>
#include <Iphlpapi.h>
#include <shlobj.h>
#include "utf8.hpp"
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Version.lib")
#pragma comment(lib,"Rpcrt4.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma warning(disable:4996)
#include <atltime.h>


#define MS_VC_EXCEPTION 0x406d1388

typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;        // must be 0x1000
	LPCSTR szName;       // pointer to name (in same addr space)
	DWORD dwThreadID;    // thread ID (-1 caller thread)
	DWORD dwFlags;       // reserved for future use, most be zero
} THREADNAME_INFO;

vector<wstring> SpliterString(const wstring& data,const wstring& spliter)
{
	size_t pos,start = 0;
	vector<wstring> strs;
	while ((pos = data.find(spliter,start)) != wstring::npos)
	{
		strs.push_back(data.substr(start,pos-start));
		start = pos + spliter.size();
	}
	if(start < data.size())
		strs.push_back(data.substr(start));
	return strs;
}

vector<string> SpliterString(const string& data,const string& spliter)
{	
	size_t pos,start = 0;
	vector<string> strs;
	while ((pos = data.find(spliter,start)) != string::npos)
	{
		strs.push_back(data.substr(start,pos-start));
		start = pos + spliter.size();
	}
	if(start < data.size())
		strs.push_back(data.substr(start));
	return strs;
}

std::pair<wstring,wstring> ParseHeader(const wstring& header)
{
	size_t pos;
	std::pair<wstring,wstring> headerpair;
	if ((pos = header.find(TCHAR(':'))) != wstring::npos)
	{
		headerpair.first  = header.substr(0,pos);
		headerpair.second = header.substr(pos+1);
		trim_str(headerpair.second);
	}
	return headerpair;
}

wstring urlencode(const wstring& str,bool bUtf8)
{
	USES_CONVERSION;
	std::string strbase;
	if(bUtf8)
		strbase = W2Utf8(str.c_str(),str.size());
	else
		strbase = W2A(str.c_str());
	std::string strout;
	unsigned char c;
	for (size_t i = 0; i < strbase.size(); ++i)
	{
		c = strbase.at(i);
		if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
		{
			strout.append(1,c);
		}
// 		else if(c == ' ')
// 		{
// 			strout.append(1,'+');
// 		}
		else
		{
			strout.append(1,'%');
			strout.append(1,(c >= 0xA0) ? ((c >> 4) - 10 + 'A') : ((c >> 4) + '0'));
			strout.append(1,((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + 'A') : ((c & 0xF) + '0'));
		}
	}
	return bUtf8 ? Utf82W(strout.c_str(),strout.size()) : A2W(strout.c_str());
}

wstring urldecode(const wstring& str,bool bUtf8)
{
	USES_CONVERSION;
	std::string strbase;
	if(bUtf8)
		strbase = W2Utf8(str.c_str(),str.size());
	else
		strbase = W2A(str.c_str());
	std::string strout;
	size_t i = 0;
	unsigned char c;
	while (i < strbase.size())
	{
		c = strbase.at(i);
		if(c == '%')
		{
			int c1 = 0;
			//高位
			if(i + 1 < strbase.size())
			{
				if(strbase.at(i+1) >= 'A' && strbase.at(i+1) <= 'F')
					c1 += (strbase.at(i+1) - 'A' + 10) * 0x10;
				else if(strbase.at(i+1) >= 'a' && strbase.at(i+1) <= 'f')
					c1 += (strbase.at(i+1) - 'a' + 10) * 0x10;
				else
					c1 += (strbase.at(i+1) - '0') * 0x10;
			}
			//低位
			if(i + 2 < strbase.size())
			{
				if(strbase.at(i+2) >= 'A' && strbase.at(i+2) <= 'F')
					c1 += (strbase.at(i+2) - 'A' + 10);
				else if(strbase.at(i+2) >= 'a' && strbase.at(i+2) <= 'f')
					c1 += (strbase.at(i+2) - 'a' + 10);
				else
					c1 += (strbase.at(i+2) - '0');
			}
			//
			i += 3;
			strout.append(1,c1);
		}
		else if(c == '+')
		{
			strout.append(1,'+');
			++i;
		}
		else
		{
			strout.append(1,c);
			++i;
		}
	}
	return bUtf8 ? Utf82W(strout.c_str(),strout.size()) : A2W(strout.c_str());
}

wstring urldecode(wstring& url,bool bUtf8)
{
	const wstring& curl = url;
	url = urldecode(curl,bUtf8);
	return url;
}

wstring	urlencode(wstring& url,bool bUtf8)
{
	const wstring& curl = url;
	url = urlencode(curl,bUtf8);
	return url;
}

void trim_left(wstring& data,const wstring& str)
{
	size_t pos;
	while ((pos = data.find(str)) == 0)
	{
		data.erase(0,pos+str.size());
	}
}

void trim_right(wstring& data,const wstring& str)
{
	size_t pos;
	while ((pos = data.rfind(str)) != wstring::npos && pos == data.size() - str.size())
	{
		data.erase(pos,str.size());
	}
}
void trim_str(wstring& data,const wstring& str)
{
	trim_left(data,str);
	trim_right(data,str);
}

void trim_left(string& data,const string& str)
{
	size_t pos;
	while ((pos = data.find(str)) == 0)
	{
		data.erase(0,pos+str.size());
	}
}

void trim_right(string& data,const string& str)
{
	size_t pos;
	while ((pos = data.rfind(str)) != string::npos && pos == data.size() - str.size())
	{
		data.erase(pos,str.size());
	}
}

void trim_str(string& data,const string& str)
{
	trim_left(data,str);
	trim_right(data,str);
}

void trace(wchar_t *fmt,...)
{
	va_list args;  
	va_start(args, fmt);  
	int len = _vscwprintf(fmt, args)+1;
	wstring text;
	text.resize(len);
	vswprintf_s((wchar_t *)text.data(), len, fmt, args);  
	va_end(args); 
	text.resize(len-1);
	OutputDebugStringW(text.c_str());
}


wstring hash_to_string(const string& hashin)
{
	USES_CONVERSION;
	static const char* hashbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
	string hashout;
	char  *pHash = (char *)hashin.data();
	int   nShift = 7;

	int nchar;
	for (nchar = 32 ; nchar ; nchar-- )
	{
		BYTE nBits = 0;
		int  nbit;
		for ( nbit = 0 ; nbit < 5 ; nbit++ )
		{
			if ( nbit ) nBits <<= 1;
			nBits |= ( *pHash >> nShift ) & 1;

			if ( ! nShift-- )
			{
				nShift = 7;
				pHash++;
			}
		}
		hashout.push_back(hashbase64[nBits]);
	}
	return A2W(hashout.c_str());
}

 string hash_from_string(const wstring& hashstr)
 {
	USES_CONVERSION;
 	int	    nchars	= 0;
 	int		nbits   = 0;
 	int		ncount  = 0;
	string  hashout;
 	if ( hashstr.size() < 32 )
 		return hashout;
	string  ahash   = W2A(hashstr.c_str());
 	BYTE*	stringin =  (BYTE*)ahash.data();
 	for (nchars = 32 ; nchars-- ; stringin++ )
 	{
 		if ( *stringin >= 'A' && *stringin <= 'Z' )
 		{
 			nbits |= ( *stringin - 'A' );
 		}
 		else if ( *stringin >= 'a' && *stringin <= 'z' )
 		{
 			nbits |= ( *stringin - 'a' );
 		}
 		else if ( *stringin >= '2' && *stringin <= '7' )
 		{
 			nbits |= ( *stringin - '2' + 26 );
 		}
 		else
 		{
 			return hashout;
 		}
 		ncount += 5;
 		if ( ncount >= 8 )
 		{
 			hashout.push_back((BYTE)(nbits >> (ncount - 8)));
 			ncount -= 8;
 		}
 		nbits <<= 5;
 	}
 	return hashout;
 }

 BOOL IsUsbDisk(std::wstring &strDisk)
 {
	 BOOL bResult = FALSE;

	 CAtlString strPath(strDisk.c_str());
	 int nidx =  strPath.Find('\\');
	 if (nidx == -1)
	 {
		 return bResult;
	 }
	 strPath = strPath.Mid(0, nidx);

	 TCHAR szDisk[MAX_PATH];
	 memset(szDisk, 0, sizeof(szDisk));
	 _stprintf(szDisk, _T("\\\\?\\%s"), (LPCTSTR)strPath);

	 HANDLE hDevice = CreateFile(szDisk, GENERIC_READ,
		 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL); 
	 if (hDevice == INVALID_HANDLE_VALUE)
	 {
		 return bResult;
	 }

	 STORAGE_PROPERTY_QUERY Query; // input param for query
	 DWORD dwOutBytes; // IOCTL output length

	 Query.PropertyId = StorageDeviceProperty;
	 Query.QueryType = PropertyStandardQuery;

	 STORAGE_DEVICE_DESCRIPTOR pDevDesc;
	 pDevDesc.Size = sizeof(STORAGE_DEVICE_DESCRIPTOR);

	 //用 IOCTL_STORAGE_QUERY_PROPERTY
	 BOOL bBack = ::DeviceIoControl(hDevice, // device handle
		 IOCTL_STORAGE_QUERY_PROPERTY, // info of device property
		 &Query, sizeof(STORAGE_PROPERTY_QUERY), // input data buffer
		 &pDevDesc, pDevDesc.Size, // output data buffer
		 &dwOutBytes, // out's length
		 (LPOVERLAPPED)NULL);

	 UINT Type = pDevDesc.BusType;
	 if (Type == BusTypeUsb)
	 {
		 bResult = TRUE;
	 }

	 CloseHandle(hDevice);
	 return bResult;
 }

 wstring GetMaxFreeDisk(__int64& capacity)
 {
	 TCHAR  szDisks[1024] = _T("");
	 DWORD  len = GetLogicalDriveStrings(sizeof(szDisks)/sizeof(TCHAR),szDisks);
	 size_t i   = 0;
	 //获取磁盘列表
	 vector<wstring> diskVector;
	 LPCTSTR   szDiskStr = szDisks;
	 while (i < len)
	 {
		 if(szDiskStr)
			diskVector.push_back(szDiskStr);
		 else
			 break;
		 i		   += _tcslen(szDiskStr)+1;
		 szDiskStr += _tcslen(szDiskStr)+1;
	 }
	//去掉不符合类型的磁盘
	 vector<wstring>::iterator iter = diskVector.begin();
	 while (iter != diskVector.end())
	 {
		 if(GetDriveType(iter->c_str()) != DRIVE_FIXED)
		 {
			 iter = diskVector.erase(iter);
		 }
		 else
		 {
			 if (IsUsbDisk(*iter))
			 {
				 iter = diskVector.erase(iter);
			 }
			 else
			 {
				 CAtlString strTestPath;
				 strTestPath.Format(_T("%s%s"),iter->c_str(),GetUuid().c_str());
				 if(CreateDirectory(strTestPath,NULL))
				 {
					 ::RemoveDirectory(strTestPath);
					 ++iter;
				 }
				 else
				 {
					 iter = diskVector.erase(iter);
				 }
			 }			 
		 }
	 }
	 //看看那个磁盘的自由空间最大
	 wstring maxFreeDisk;
 	 capacity = 0;
 	 for (i = 0; i < diskVector.size(); ++i)
 	 {
 		 __int64 freeBytesAvailable = GetFreeCapacity(diskVector[i]);
 		 if(freeBytesAvailable > capacity)
 		 {
 			 capacity = freeBytesAvailable;
 			 maxFreeDisk = diskVector[i];
 		 }
 	 }
	return maxFreeDisk;
 }

 __int64 GetFreeCapacity(const wstring& disk)
 {
	ULARGE_INTEGER freeBytesAvailable = {0};
	GetDiskFreeSpaceEx(disk.c_str(),&freeBytesAvailable,NULL,NULL);
	return freeBytesAvailable.QuadPart;
 }

 DWORD	GetVersionNumber(LPCTSTR szFilePath)
 {
	 if(_taccess(szFilePath,0) != 0)
		 return 0;
	 //获得当前程序的版本号
	 DWORD dwVersion = 0;

	 DWORD len = GetFileVersionInfoSize(szFilePath,NULL);
	 if(len == 0)
		 return 0;
	 string versionInfo;
	 versionInfo.resize(len);
	 if (versionInfo.data() == NULL)
	 {
		 return dwVersion;
	 }
	 GetFileVersionInfo(szFilePath,NULL,len,(LPVOID)versionInfo.data());
	 VS_FIXEDFILEINFO  *pInfo = NULL;    
	 unsigned int	  nInfoLen;  
	 VerQueryValue((LPVOID)versionInfo.data(),_T("\\"),(LPVOID*)&pInfo,&nInfoLen);

	 dwVersion	= MAKELONG(MAKEWORD((BYTE)( pInfo->dwFileVersionLS & 0xFFFF ), (BYTE)( pInfo->dwFileVersionLS >> 16 )),
		 MAKEWORD((BYTE)( pInfo->dwFileVersionMS & 0xFFFF ), (BYTE)( pInfo->dwFileVersionMS >> 16 )));
	 return dwVersion;
 }

 wstring GetVersionString(DWORD dwVersion)
 {
	 TCHAR szVersion[MAX_PATH] = _T("");
	 wsprintf(szVersion,_T("%d.%d.%d.%d"),(dwVersion>>24)&0xFF,(dwVersion>>16)&0xFF,(dwVersion>>8)&0xFF,dwVersion&0xFF);
	 return szVersion;
 }

 wstring GetSizeDesc(__int64 size)
 {
	 double fsize;
	 CAtlString strDesc = _T("B");
	 strDesc.Format(_T("%.2f B"),(float)size);
	 if((__int64)size / 1024 <= 0)
		 return (LPCTSTR)strDesc;
	 //
	 fsize = (double)size / 1024; 
	 strDesc.Format(_T("%.2f KB"),fsize);
	 if((__int64)fsize / 1024 <= 0)
		 return (LPCTSTR)strDesc;
	 //
	 fsize /= 1024; 
	 strDesc.Format(_T("%.2f MB"),fsize);
	 if((__int64)fsize / 1024 <= 0)
		 return (LPCTSTR)strDesc;
	 //
	 fsize /= 1024; 
	 strDesc.Format(_T("%.2f GB"),fsize);
	 return (LPCTSTR)strDesc;
 }

 int GetHostLocalIp()
 {
	 char szHostName[MAX_PATH] = "";
	 int  localIp = 0;
	 if(gethostname(szHostName,MAX_PATH) == 0)
	 {
		 hostent *hostInfo = gethostbyname(szHostName);	
		 int index = 0;
		 if(hostInfo && hostInfo->h_addr_list)
		 {
			 while (hostInfo->h_addr_list[index])
			 {
				 if(*(int *)hostInfo->h_addr_list[index] != inet_addr("127.0.0.1"))
				 {
					 localIp = *(int *)hostInfo->h_addr_list[index];
					 break;
				 }
				 ++index;
			 }
		 }
	 }
	 return localIp;
 }

 wstring GetIpStr(int ip)
 {
	 if(ip == 0)
	 {
		 return _T("0.0.0.0");
	 }
	 else
	 {
		 USES_CONVERSION;
		 return A2W(inet_ntoa(*(in_addr *)&ip));
	 }
 }

 BOOL CopyText(HWND hWnd,LPCTSTR szText)
 {
	 USES_CONVERSION;
	 if(!::OpenClipboard(hWnd))
		 return FALSE;
	 char* pszText = T2A(szText);
	 HGLOBAL clipbuffer;
	 char * buffer;
	 EmptyClipboard();
	 clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(pszText)+1);
	 buffer = (char *)GlobalLock(clipbuffer);
	 strcpy(buffer, pszText);
	 GlobalUnlock(clipbuffer);
	 SetClipboardData(CF_TEXT,clipbuffer);
	 //GlobalFree(clipbuffer);
	 CloseClipboard();
	 return TRUE;
 }

 wstring Utf82W(LPCSTR szContent,int size)
 {
	 wstring strContent;
	 int len = MultiByteToWideChar(CP_UTF8,0,szContent,size,NULL,0);
	 strContent.resize(len);
	 MultiByteToWideChar(CP_UTF8,0,szContent,size,(wchar_t*)strContent.c_str(),len);
	 if(len > 0 && size == -1)
		 strContent.resize(len-1);
	 return strContent;
 }

 string  W2Utf8(LPCWSTR szContent,int size)
 {
	 string strContent;
	 int len = WideCharToMultiByte(CP_UTF8,0,szContent,size,NULL,0,NULL,NULL);
	 strContent.resize(len);
	 WideCharToMultiByte(CP_UTF8,0,szContent,size,(char *)strContent.c_str(),len,NULL,NULL);
	 if(len > 0 && size == -1)
		 strContent.resize(len-1);
	 return strContent;
 }

 void WaitForThreadExit(HANDLE hThread,DWORD timeOut)
 {
	 if(hThread == NULL)
		 return;
	 MSG msg;
	 DWORD startTime = GetTickCount();
	 BOOL isRecvQuit = FALSE;
	 BOOL bExit = FALSE;
	 while (!bExit)
	 {
		 DWORD dwRet = MsgWaitForMultipleObjects(1, &hThread, FALSE, timeOut, QS_ALLINPUT);
		 if (dwRet == WAIT_OBJECT_0 + 1)
		 {	
			
			 while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			 {
				 if(msg.message == WM_QUIT)
				 {
					 isRecvQuit = TRUE;
					 continue;
				 }
				 else
				 {
					 if(timeOut > 0 && GetTickCount() - startTime > timeOut)
					 {
						::TerminateThread(hThread,0);
						bExit = TRUE;
						break;
					 }
					 TranslateMessage(&msg);
					 DispatchMessage(&msg);
				 }
			 }
		 }
		 else if(dwRet == WAIT_TIMEOUT)
		 {
			::TerminateThread(hThread,0);
			break;
		 }
		 else
		 {
			 break;
		 }
	 }
	if(isRecvQuit)
		PostQuitMessage(0);
 }

 wstring GetUuid()
 {
	 USES_CONVERSION;
	 wstring strUid;
	 UUID uid;
	 if(UuidCreate(&uid) == RPC_S_OK)
	 {
		RPC_WSTR szUid = NULL;
		if(UuidToString(&uid,&szUid) == RPC_S_OK)
		{
			strUid = (wchar_t *)szUid;
			RpcStringFree(&szUid);
		}
	 }
	 return strUid;
 }

 void SetThreadName(DWORD dwThreadID, LPCSTR szThreadName)
 {
	 THREADNAME_INFO info;
	 info.dwType = 0x1000;
	 info.szName = szThreadName;
	 info.dwThreadID = dwThreadID;
	 info.dwFlags = 0;

	 __try
	 {
		 RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(DWORD),
			 (DWORD *)&info);
	 }
	 __except (EXCEPTION_CONTINUE_EXECUTION)
	 {
	 }
 }

 wstring GetPersonalDir()
 {
	TCHAR szPath[MAX_PATH] = _T("");
	SHGetSpecialFolderPath(NULL,szPath,CSIDL_PERSONAL,TRUE);
	return szPath;
 }

 wstring GetCurrentProcessPersonalDir()
 {
	wstring strPath = GetPersonalDir();
	TCHAR szName[MAX_PATH] = _T("");
	GetModuleFileName(NULL,szName,MAX_PATH);
	LPTSTR lpszName =  _tcsrchr(szName,TCHAR('\\'));
	*_tcsrchr(lpszName,TCHAR('.')) = TCHAR('\0');
	strPath += lpszName;
	if(_taccess(strPath.c_str(),0) != 0)
		CreateDirectory(strPath.c_str(),NULL);
	return strPath;
 }

 std::wstring ANSIToUnicode(LPCSTR szContent,int size /* = -1 */)
 {
     int  unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, szContent, size, NULL, 0);
     std::auto_ptr<wchar_t>  pUnicode(new wchar_t[unicodeLen + 1]);
     memset(pUnicode.get(), 0, (unicodeLen + 1) * sizeof(wchar_t));
     ::MultiByteToWideChar(CP_ACP, 0, szContent, size, pUnicode.get(), unicodeLen);
     wstring  rt;
     rt = ( wchar_t* )pUnicode.get();
     return  rt;
 }

 std::wstring GBKToUnicode( LPCSTR szContent,int size /*= -1*/ )
 {
	 int  unicodeLen = ::MultiByteToWideChar(936, 0, szContent, size, NULL, 0);
	 std::auto_ptr<wchar_t>  pUnicode(new wchar_t[unicodeLen + 1]);
	 memset(pUnicode.get(), 0, (unicodeLen + 1) * sizeof(wchar_t));
	 ::MultiByteToWideChar(936, 0, szContent, size, pUnicode.get(), unicodeLen);
	 wstring  rt;
	 rt = ( wchar_t* )pUnicode.get();
	 return  rt;
 }

 BOOL IsRuning(LPCTSTR softname)
 {
	 wstring finder = softname;
	 finder += _T(".exe");
	 //WCHAR *finder=_T("iResearchiClick.exe");

	 PROCESSENTRY32 pe32 ={0}; 
	 pe32.dwSize = sizeof(pe32); 

	 HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	 BOOL bProcessExist = Process32First(hProcessSnap, &pe32); 
	 while (bProcessExist)
	 { 
		 if (!wcsicmp(pe32.szExeFile,finder.c_str()))
		 {
			 //wprintf(_T("%d-----%s\n"),pe32.th32ProcessID,pe32.szExeFile);
			 break;
		 }
		 bProcessExist = Process32Next(hProcessSnap, &pe32);
	 }
	 CloseHandle(hProcessSnap);
	 return bProcessExist;
 }

int CheckSoftState(LPCTSTR softname)
{
	int iClickState = 0;	
	if (IsRuning(softname))
	{
		iClickState = 2;
	}
	return iClickState;
}

vector<DWORD> GetProcessList(LPCTSTR name)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    vector<DWORD> handles;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return handles;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if(!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return handles;
    }
    do
    {
        if (_tcsicmp(name, pe32.szExeFile) == 0)
        {
            handles.push_back(pe32.th32ProcessID);
        }
    }
    while(Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return handles;
}

BOOL KillProcess(DWORD dwProcessID)
{
    HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
    if (!hProcess)
    {
        return FALSE;
    }
    BOOL bRet = ::TerminateProcess(hProcess, 0);
    ::WaitForSingleObject(hProcess,5000);
    CloseHandle(hProcess);
    return bRet;
}
 
 BOOL GetDesktopPath(wstring &strPath)
 {
	 TCHAR szPath[MAX_PATH] = {0};
	 BOOL bResult = SHGetSpecialFolderPath(NULL, szPath, CSIDL_DESKTOP, TRUE);
	 strPath = szPath;
	 return bResult;
 }
 static void CpuID(BYTE *szCpu,UINT &uCpuID,BOOL &bException)
 {
	 BOOL bExp		  = FALSE;
	 BYTE szCpuID[16]  = {0};
	 UINT ucpuID        = 0U; 
	 __try 
	 {
		 _asm 
		 {
			 mov eax, 0
			 cpuid
			 mov dword ptr szCpuID[0], ebx
			 mov dword ptr szCpuID[4], edx
			 mov dword ptr szCpuID[8], ecx
			 mov eax, 1
			 cpuid 
			 mov ucpuID, edx
		 }
	 }
	 __except( EXCEPTION_EXECUTE_HANDLER )
	 {
		 bExp   = TRUE;
	 }
	 bException = bExp;
	 uCpuID     = ucpuID;
	 memcpy(szCpu,szCpuID,16);
 }
  static string GetCpuID()
 {
	 BOOL bException = FALSE;
	 BYTE szCpu[16]  = {0};
	 UINT uCpuID     = 0U; 
	 char pdData[1024] ={0};
	 string cupId;
	 int pdDatalen = 0;
	 CpuID(szCpu,uCpuID,bException);
	 if( !bException )
	 {
		 CopyMemory( pdData + pdDatalen, &uCpuID, sizeof( UINT ) );
		 pdDatalen += sizeof( UINT );
		 uCpuID = strlen( ( char* )szCpu );
		 CopyMemory( pdData + pdDatalen, szCpu, uCpuID );
		 pdDatalen += uCpuID;
		 cupId.append(pdData,pdDatalen);
	 }
	 return cupId;
 }

 static string GetMac()
 {
	 PIP_ADAPTER_INFO pAdapterInfo;  
	 DWORD AdapterInfoSize;  
	 DWORD Err; 
	 char pdData[1024] ={0};
	 int pdDatalen = 0;
	 string mac;
	 AdapterInfoSize   = 0;  
	 Err = GetAdaptersInfo(NULL,   &AdapterInfoSize);  
	 if((Err != 0) && (Err != ERROR_BUFFER_OVERFLOW))
	 {  
		 ATLTRACE(_T("获得网卡信息失败！"));  
		 return mac;  
	 }  
	 pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR,AdapterInfoSize);  
	 if(pAdapterInfo == NULL)
	 {  
		 ATLTRACE(_T("分配网卡信息内存失败"));  
		 return mac;  
	 }  
	 if(GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize) != 0)
	 {  
		 ATLTRACE(_T("获得网卡信息失败！\n"));  
		 GlobalFree(pAdapterInfo);  
		 return mac;  
	 }

	 CopyMemory(pdData + pdDatalen, pAdapterInfo->Address, pAdapterInfo->AddressLength );
	 pdDatalen += pAdapterInfo->AddressLength;
	 mac.append(pdData,pdDatalen);
	 GlobalFree(pAdapterInfo);  
	 return mac;  
 }

 string GetMachineCode()
 {
	return GetMac()+GetCpuID();
 }

 //
 string	 GetMd5Org(const string& buf)
 {
	 string md5value;
	 //计算HASH值
	 ATL::CCryptProv prov;
	 HRESULT hr = prov.Initialize( PROV_RSA_FULL , NULL , MS_DEF_PROV , CRYPT_VERIFYCONTEXT | CRYPT_SILENT );
	 if( hr == NTE_KEYSET_NOT_DEF )
	 {
		 hr = prov.Initialize( PROV_RSA_FULL ,
			 NULL ,
			 MS_DEF_PROV ,
			 CRYPT_VERIFYCONTEXT | CRYPT_SILENT | CRYPT_NEWKEYSET);
	 }
	 if( FAILED( hr ) ) 
		 return md5value;
	 ATL::CCryptMD5Hash hashmd5;
	 hr = hashmd5.Initialize( prov );
	 if( FAILED( hr ) )
		 return md5value;
	 hr = hashmd5.AddData((const BYTE*)buf.data() , buf.size());
	 if( FAILED( hr ) ) 
		 return md5value;
	 DWORD len = 17;
	 md5value.resize(len);
	 hr = hashmd5.GetValue((BYTE *)md5value.data() ,&len);
	 if( FAILED( hr ) )
		 return md5value;
	 md5value.resize(16);
	 return md5value;
 }

 wstring	Md52String(const string& md5)
 {
	 if (md5.size() != 16)
	 {
		return L"";
	 }
	 ATLASSERT(md5.size() == 16);
	 CAtlString md5str;
	 md5str.Format(_T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),
		 (unsigned char)md5.at(0), (unsigned char)md5.at(1), (unsigned char)md5.at(2), (unsigned char)md5.at(3),
		 (unsigned char)md5.at(4), (unsigned char)md5.at(5), (unsigned char)md5.at(6), (unsigned char)md5.at(7),
		 (unsigned char)md5.at(8), (unsigned char)md5.at(9), (unsigned char)md5.at(10), (unsigned char)md5.at(11),
		 (unsigned char)md5.at(12), (unsigned char)md5.at(13), (unsigned char)md5.at(14), (unsigned char)md5.at(15));
	 return (LPCTSTR)md5str;
 }

 wstring GetMd5String(const string& buf)
 {
	 string  md5value = GetMd5Org(buf);
	 return Md52String(md5value);
 }


 namespace UtilBase64
 {

	 namespace inner
	 {
		 static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";
		 // static const std::wstring base64_wchars = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ" L"abcdefghijklmnopqrstuvwxyz" L"0123456789+/";

		 inline bool is_base64( unsigned char c ) {
			 return (isalnum(c) || (c == '+') || (c == '/'));
		 }

		 //inline bool w_is_base64( unsigned short c ) {
		 //	return (isalnum(c) || (c == '+') || (c == '/'));
		 //}
	 }



	 std::string convert_to_base64( const std::string& buf )
	 {
		 std::string ret;
		 const unsigned char* bytes_to_encode = reinterpret_cast< const unsigned char* >( buf.data() );
		 int in_len = buf.size();
		 int i = 0;
		 int j = 0;
		 unsigned char char_array_3[3];
		 unsigned char char_array_4[4];

		 while (in_len--) {
			 char_array_3[i++] = *(bytes_to_encode++);
			 if (i == 3) {
				 char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				 char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				 char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				 char_array_4[3] = char_array_3[2] & 0x3f;

				 for(i = 0; (i <4) ; i++)
					 ret += inner::base64_chars[char_array_4[i]];
				 i = 0;
			 }
		 }

		 if (i)
		 {
			 for(j = i; j < 3; j++)
				 char_array_3[j] = '\0';

			 char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			 char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			 char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			 char_array_4[3] = char_array_3[2] & 0x3f;

			 for (j = 0; (j < i + 1); j++)
				 ret += inner::base64_chars[char_array_4[j]];

			 while((i++ < 3))
				 ret += '=';

		 }
		 return ret;
	 }

	 std::string convert_from_base64( const std::string& base64 )
	 {
		 int in_len = base64.size();
		 int i = 0;
		 int j = 0;
		 int in_ = 0;
		 unsigned char char_array_4[4], char_array_3[3];
		 std::string ret;

		 while (in_len-- && ( base64[in_] != '=') && inner::is_base64(base64[in_])) {
			 char_array_4[i++] = base64[in_]; in_++;
			 if (i ==4) {
				 for (i = 0; i <4; i++)
					 char_array_4[i] = inner::base64_chars.find(char_array_4[i]);

				 char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				 char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				 char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				 for (i = 0; (i < 3); i++)
					 ret += char_array_3[i];
				 i = 0;
			 }
		 }

		 if (i) {
			 for (j = i; j <4; j++)
				 char_array_4[j] = 0;

			 for (j = 0; j <4; j++)
				 char_array_4[j] = inner::base64_chars.find(char_array_4[j]);

			 char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			 char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			 char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			 for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		 }

		 return ret;
	 }
 }

//XXTEA 加密
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

 void btea(UINT32 *v, int n, UINT32 const key[4]) {
	 UINT32 y, z, sum;
	 unsigned p, rounds, e;
	 if (n > 1) {          /* Coding Part */
		 rounds = 6 + 52/n;
		 sum = 0;
		 z = v[n-1];
		 do {
			 sum += DELTA;
			 e = (sum >> 2) & 3;
			 for (p=0; p<n-1; p++) {
				 y = v[p+1]; 
				 z = v[p] += MX;
			 }
			 y = v[0];
			 z = v[n-1] += MX;
		 } while (--rounds);
	 } else if (n < -1) {  /* Decoding Part */
		 n = -n;
		 rounds = 6 + 52/n;
		 sum = rounds*DELTA;
		 y = v[0];
		 do {
			 e = (sum >> 2) & 3;
			 for (p=n-1; p>0; p--) {
				 z = v[p-1];
				 y = v[p] -= MX;
			 }
			 z = v[n-1];
			 y = v[0] -= MX;
		 } while ((sum -= DELTA) != 0);
	 }
 }

 wstring OS_GetUserName()
 {
	CAtlString os_username;
	TCHAR szInfo[MAX_PATH];
	memset(szInfo,0,sizeof(TCHAR)*MAX_PATH);
	DWORD  nLen = MAX_PATH;
	::GetUserName(szInfo,&nLen);
	os_username.Append(szInfo,nLen);
	return (LPCTSTR)os_username;
 }

 wstring OS_GetSysInstallTime()
 {
	 HKEY hKey;
	 CAtlString strinstallTime;
	 LPCTSTR StrKey=_T("Software\\Microsoft\\Windows NT\\CurrentVersion");
	 if (ERROR_SUCCESS==::RegOpenKeyEx(HKEY_LOCAL_MACHINE,StrKey,NULL,KEY_ALL_ACCESS,&hKey))
	 {
		 DWORD dwSize=sizeof(DWORD);
		 DWORD dwType=REG_DWORD;
		 DWORD installDate;
		 if (ERROR_SUCCESS==::RegQueryValueEx(hKey,_T("InstallDate"),NULL,&dwType,(LPBYTE)&installDate,&dwSize))
		 {
			 CTime t(installDate);
			 strinstallTime = t.Format(_T("%Y/%m/%d:%H-%M-%S"));
		 }
		 RegCloseKey(hKey);
	 }	
	 return (LPCTSTR)strinstallTime;
 }

 wstring AddSlash(LPCTSTR szStr){
	 if(szStr == NULL)
		 return L"";
	 wstring result;
	 for( int i =0;i < wcslen(szStr) ; i++)
	 {
		 if( szStr[i] == '"')result += L"\\\"";
		 else if( szStr[i] == '\\')result += L"\\\\";
		 else if( szStr[i] == '\t')result += L"\\t";
		 else if( szStr[i] == '\f')result += L"\\f";
		 else if( szStr[i] == '\b')result += L"\\b";
		 else if( szStr[i] == '\n')result += L"\\n";
		 else if( szStr[i] == '\r')result += L"\\r";
		 else result += szStr[i];
	 }
	return result;
 }

 time_t StringToTime(wstring wtime)
 {
	 USES_CONVERSION;
	 SYSTEMTIME st = {0};
	 if(!wtime.empty())
	 {
		 string time = W2A(wtime.c_str());
		 sscanf(time.c_str(),"%04d-%02d-%02d %02d:%02d:%02d",&st.wYear,&st.wMonth,&st.wDay,&st.wHour,&st.wMinute,&st.wSecond);
		 struct tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, st.wMonth-1, st.wYear-1900, st.wDayOfWeek, 0, 0};
		 return mktime(&gm);
	 }
	 return 0;
 }

 time_t	SystemTimeToTime(SYSTEMTIME time)
 {
	 struct tm gm = {time.wSecond, time.wMinute, time.wHour, time.wDay, time.wMonth-1, time.wYear-1900, time.wDayOfWeek, 0, 0};
	 return mktime(&gm);
 }

 SYSTEMTIME TimeToSystemTime(time_t time)
 {
	 tm temptm = *localtime(&time);
	 SYSTEMTIME st = {1900 + temptm.tm_year, 
		 1 + temptm.tm_mon, 
		 temptm.tm_wday, 
		 temptm.tm_mday, 
		 temptm.tm_hour, 
		 temptm.tm_min, 
		 temptm.tm_sec, 
		 0};
	 return st;
 }

 wstring urlescape(const wstring& str)
 {
	 std::string strbase = W2Utf8(str.c_str(),str.size());	 
	 std::string strout;
	 strout.resize((str.size() + 1) * 10); //\uxxxx or \UXXXXXXXX
	 u8_escape((char*)strout.data(), strout.size() , strbase.c_str(), 0);

	 return Utf82W(strout.c_str(),strout.size());
 }

 wstring urlunescape(const wstring& str)
 {
	 std::string strbase = W2Utf8(str.c_str(),str.size());
	 std::string strout;
	 strout.resize(str.size());
	 u8_unescape((char*)strout.data(), strout.size() , strbase.c_str());

	 return Utf82W(strout.c_str(),strout.size());
 }

 unsigned int hex2int(char c)
 {
	 if( c >= '0' && c <= '9')
	 {
		 return (unsigned int) (c -48);
	 }
	 else if( c >= 'A' && c <= 'F')
	 {
		 return (unsigned int) (c -55);
	 }
	 else if( c >= 'a' && c <= 'f')
	 {
		 return (unsigned int) (c - 87);
	 }
	 else
	 {
		 return 0;
	 }
 }
 std::wstring Hex2String( const char* szHex )
 {
	 int result_index = 0;
	 std::string strString;
	 strString.resize(strlen(szHex));
	 for(int j = 0; j < strlen(szHex) - 1; )
	 {
		 unsigned int high = hex2int(szHex[j++]);
		 unsigned int low = hex2int(szHex[j++]);
		 strString[result_index++] = char(high *16 + low);
	 }
	 strString[result_index] = '\0';
	 return Utf82W(strString.c_str());
 }

 std::wstring String2Hex( const char* szString )
 {
	 std::string strString = szString;
	 std::string strHex;
	 strHex.resize(2*strString.size() + 1);
	 const char* szHexBase = "0123456789abcdef";
	 int result_index = 0;
	 for(int i = 0; i< strString.size(); i++)
	 {
		 unsigned char cHex = (unsigned char)strString[i];
		 strHex[result_index++] = szHexBase[(cHex&0xf0)>>4];
		 strHex[result_index++] = szHexBase[(cHex&0x0f)];
	 }
	 strHex[result_index] = '\0';
	 return Utf82W(strHex.c_str());
 }
