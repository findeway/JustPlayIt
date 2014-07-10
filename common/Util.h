#pragma once
#include "string"
#include "list"
#include "vector"
#include <WinBase.h>
using namespace std;

namespace UtilBase64
{
	std::string convert_to_base64( const std::string& buf );
	std::string convert_from_base64(const std::string& base64);
}
//
vector<wstring> SpliterString(const wstring& data,const wstring& spliter);
vector<string>  SpliterString(const string& data,const string& spliter);
void	trim_left(wstring& data,const wstring& str = L" ");
void	trim_right(wstring& data,const wstring& str = L" ");
void	trim_str(wstring& data,const wstring& str = L" ");
void	trim_left(string& data,const string& str = " ");
void	trim_right(string& data,const string& str = " ");
void	trim_str(string& data,const string& str = " ");
wstring Utf82W(LPCSTR szContent,int size = -1);
string  W2Utf8(LPCWSTR szContent,int size = -1);
wstring ANSIToUnicode(LPCSTR szContent,int size = -1);
wstring GBKToUnicode(LPCSTR szContent,int size = -1);
//
void trace(wchar_t *fmt,...);
//
std::pair<wstring,wstring> ParseHeader(const wstring& header);
wstring urldecode(wstring& url,bool bUtf8 = true);
wstring	urlencode(wstring& url,bool bUtf8 = true);
wstring urldecode(const wstring& url,bool bUtf8 = true);
wstring	urlencode(const wstring& url,bool bUtf8 = true);
//
wstring urlescape(const wstring& str);
wstring urlunescape(const wstring& str);
//
wstring hash_to_string(const string& hashin);
string  hash_from_string(const wstring& hashstr);
wstring	GetMd5String(const string& buf);
wstring	Md52String(const string& md5);
string	GetMd5Org(const string& buf);

time_t	   StringToTime(wstring time);
time_t     SystemTimeToTime(SYSTEMTIME time);
SYSTEMTIME TimeToSystemTime(time_t time);
//
wstring GetMaxFreeDisk(__int64& capacity);
__int64 GetFreeCapacity(const wstring& disk);
//
DWORD	GetVersionNumber(LPCTSTR szFilePath);
wstring GetVersionString(DWORD dwVersion);
//
wstring GetSizeDesc(__int64 size);
//
int		GetHostLocalIp();
wstring GetIpStr(int ip);
//
BOOL	CopyText(HWND hWnd,LPCTSTR szText);
//
wstring GetUuid();

void	WaitForThreadExit(HANDLE hThread,DWORD timeOut = 3000);
//设置线程名字，szThreadName不能超过8个字符
void	SetThreadName(DWORD dwThreadID, LPCSTR szThreadName);
wstring GetPersonalDir();
wstring GetCurrentProcessPersonalDir();
//
int		CheckSoftState(LPCTSTR softname);
//
vector<DWORD> GetProcessList(LPCTSTR name);
BOOL	KillProcess(DWORD dwProcessID);
//
string	GetMachineCode();
BOOL	GetDesktopPath(wstring &strPath);   //桌面路径
//XXTEA 加密
void btea(UINT32 *v, int n, UINT32 const key[4]);

wstring OS_GetUserName();
wstring OS_GetSysInstallTime();
wstring AddSlash(LPCTSTR szStr);

//16进制和字符串转换
std::wstring Hex2String(const char* szHex);
std::wstring String2Hex(const char* szString);

#ifdef _DEBUG
#define SHTRACE  trace
#else
#define SHTRACE  (void)
#endif

