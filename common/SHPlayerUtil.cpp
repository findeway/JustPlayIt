#include "SHPlayerUtil.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlbase.h>
#include <atltrace.h>
#include <iostream>
#include <atlstr.h>
#include <algorithm>
#include <atlcrypt.h>
#include <atlstr.h>
#include <dbt.h>						// For DeviceChange.
#include <winioctl.h>					// For DeviceIOCtl.
#include <TLHELP32.h>
#include <Iphlpapi.h>
#include <shlobj.h>
#include "Base64.h"
#include "SohuToolBase.h"
#include "atlapp.h"
#include "atldlgs.h"
#include "LogWrapper.h"
#include "EncryptExport.h"

extern string WMI_MachineCodeQuery();

#pragma warning(disable:4996)
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Version.lib")
using namespace std;

BOOL IsRuning(LPCTSTR softname);

std::wstring GetFileHash( LPCTSTR szFilePath )
{
	FILE* pfile		= _wfopen(szFilePath,L"rb");
	string hashId	= "";
	if(pfile)
	{
		string data;
		fseek(pfile,0,SEEK_END);
		GetSohuFileData(pfile,data);
		fclose(pfile);
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
			return L"";
		ATL::CCryptSHAHash hashSha1;
		hr = hashSha1.Initialize( prov );
		if( FAILED( hr ) )
			return L"";
		hr = hashSha1.AddData((const BYTE*)data.data() , data.size());
		if( FAILED( hr ) ) 
			return L"";
		DWORD len = 21;
		hashId.resize(len);
		hr = hashSha1.GetValue((BYTE *)hashId.data() ,&len);
		if( FAILED( hr ) )
			return L"";
		return hash_to_string(hashId);
		//
	}
	return L"";
}

BOOL EncryptSohuFile( LPCTSTR szFile )
{
	try
	{
		//
		FILE *pFile = _wfopen(szFile,_T("rb"));
		if(pFile == NULL)
			return FALSE;
		int len = 0;
		fseek(pFile,0,SEEK_END);
		len = ftell(pFile);
		fseek(pFile,0,SEEK_SET);
		string data;
		data.resize(len);
		int readLen = fread((LPVOID)data.data(),1,len,pFile);
		DWORD error = GetLastError();
		fclose(pFile);
		//
		pFile = _wfopen(szFile,_T("wb"));
		if(pFile == NULL)
			return FALSE;
		//
		fwrite(SH_FILE_MARK,1,strlen(SH_FILE_MARK),pFile);
		__int32 version = SH_FILE_VERSION;
		fwrite((char *)&version,1,sizeof(version),pFile);
		//
		__int32 reLen = min(32*1024,len);
		reverse(data.begin(),data.begin()+reLen);
		fwrite((char *)&reLen,1,sizeof(reLen),pFile);
		fwrite(data.data(),1,data.size(),pFile);
		fclose(pFile);
		return TRUE;
	}
	catch (...)
	{
		return FALSE;
	}
}

void GetSohuFileData( FILE* pfile,string& data )
{
	try
	{
		data.clear();
		char szMark[MAX_PATH] = "";
		int  markLen		  = strlen(SH_FILE_MARK);
		fseek(pfile,0,SEEK_END);
		int size  = ftell(pfile);
		int offset= 0; 
		fseek(pfile,0,SEEK_SET);
		if(fread(szMark,1,markLen,pfile) != markLen)
			return;
		if(stricmp(szMark,SH_FILE_MARK) == 0)
		{
			offset += strlen(SH_FILE_MARK);
			__int32 version = 0;
			if(fread((char *)&version,1,sizeof(version),pfile) != sizeof(version) || version != SH_FILE_VERSION)
				return;		
			offset += sizeof(__int32);
			//
			__int32 rlen = 0;
			if(fread((char *)&rlen,1,sizeof(rlen),pfile) != sizeof(rlen))
				return;
			offset += sizeof(__int32);
			data.resize(rlen);
			if(fread((char *)data.data(),1,rlen,pfile) != rlen)
				return;
			reverse(data.begin(),data.end());
		}
		else
		{
			fseek(pfile,0,SEEK_SET);
		}
		int		readLen = size - data.size() - offset;
		int     dataLen = data.size();
		data.resize(readLen + dataLen);
		if(fread((char *)(data.data()+dataLen),1,readLen,pfile) != readLen)
			return;
	}
	catch (...)
	{
	}

}

__int64 GetSHPlayerVersionNumber()
{
	TCHAR szPath[MAX_PATH];
	memset(szPath, 0, sizeof(szPath));
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	::PathRemoveFileSpec(szPath);
	_tcscat(szPath, _T("\\SHVersion.dll"));
	return GetVersionNumber(szPath);
}

std::wstring GetSHPlayerVersionString()
{
	return GetVersionString((DWORD)GetSHPlayerVersionNumber());
}

std::wstring GetSohuVAIniFilePath(void)
{
	ATL::CString strPath = GetFeihuPersonalDir().c_str();
	strPath += _T("\\config\\SohuVA.ini");
	return (LPCTSTR)strPath;
}

void EnableAutoRun( bool enable )
{
	HKEY hKey = NULL;
	//写入是否自动启动
	if(::RegOpenKey(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"),&hKey) == ERROR_SUCCESS)
	{
		if(enable)
		{
			char	szOldPath[MAX_PATH] = "";
			DWORD	dwLen  = MAX_PATH;
			DWORD	dwType = 0;
			::RegQueryValueExA(hKey,("SohuVA"),NULL,&dwType,(LPBYTE)szOldPath,&dwLen);
			char szPath[MAX_PATH] = "";
			GetModuleFileNameA(NULL,szPath,MAX_PATH);
			*strrchr(szPath,'\\') = '\0';
			strcat(szPath,"\\SHPlayer.exe");
			char szCommand[MAX_PATH*2] = "";
			sprintf(szCommand,"\"%s\" /auto",szPath);
			if(_stricmp(szOldPath,szCommand) != 0)
				::RegSetValueExA(hKey,("SohuVA"),NULL,REG_SZ,(LPBYTE)szCommand,strlen(szCommand));
		}
		else
		{
			::RegDeleteValue(hKey,_T("SohuVA"));
		}
		::RegCloseKey(hKey);
	}
}

unsigned int GetChannelNum( void )
{
	TCHAR szPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	::PathRemoveFileSpec(szPath);
	_tcscat(szPath, _T("\\sohu.cfg"));
	if (_taccess(szPath, 0) == -1)
	{
		return 1011;
	}
	return GetPrivateProfileInt(_T("channel"), _T("num"), 1011, szPath);
}

std::wstring GetReferID( void )
{
	TCHAR szPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	::PathRemoveFileSpec(szPath);
	_tcscat(szPath, _T("\\sohu.cfg"));
	if (_taccess(szPath, 0) == -1)
	{
		return _T("");
	}
	//
	TCHAR szreferID[MAX_PATH] = _T("");
	GetPrivateProfileString(_T("channel"),_T("referID"),_T(""),szreferID,MAX_PATH,szPath);
	return szreferID;
}

static BOOL IsInstalliClick()
{
	//若判断操作系统类型，性能会降低。
	HKEY hkey;
	//win7、Vista
	LPCWSTR RegDir = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\艾瑞调研通 安装程序");
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,RegDir,0,KEY_READ,&hkey)==ERROR_SUCCESS)
	{
		RegCloseKey(hkey); 
		return TRUE;
	}
	//xp 、2003
	RegDir = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{4A6C97A5-5C5T-434E-B5C3-E881726FAEE5}_is1");
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,RegDir,0,KEY_READ,&hkey)==ERROR_SUCCESS)
	{
		RegCloseKey(hkey); 
		return TRUE;
	}
	return FALSE;
}

static BOOL IsRuningiClick()
{
	WCHAR *finder=_T("iResearchiClick.exe");

	PROCESSENTRY32 pe32 ={0}; 
	pe32.dwSize = sizeof(pe32); 

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	BOOL bProcessExist = Process32First(hProcessSnap, &pe32); 
	while (bProcessExist)
	{ 
		if (!wcsicmp(pe32.szExeFile,finder))
		{
			break;
		}
		bProcessExist = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	return bProcessExist;
}


int CheckiResearchiClick()
{
	int iClickState = 0;	
	if (IsInstalliClick())
	{
		iClickState = 1; 
	} 
	if (IsRuningiClick())
	{
		iClickState = 2;
	} 
	return iClickState;
}

void ReadReg( LPCTSTR key, wstring &Value )
{
	HKEY hkey;
	DWORD dwRet;
	TCHAR tcPath[MAX_PATH] = {0};
	DWORD dwLen = MAX_PATH * sizeof(TCHAR);

	dwRet= RegOpenKey(HKEY_CURRENT_USER,_T("Software\\SOHU\\SoHuVA"),&hkey);
	if (dwRet ==ERROR_SUCCESS)
	{
		dwRet = RegQueryValueEx(hkey,key,NULL,NULL,(BYTE *)tcPath,&dwLen);  
	}
	if (dwRet == ERROR_SUCCESS)
	{
		RegCloseKey(hkey);
	}
	Value.append(tcPath) ;
}

ATL::CString GetSohuVACfgPath()
{
	ATL::CString sohuvaCfg;
	sohuvaCfg.Format(_T("%s\\config\\SohuVA.ini"), GetFeihuPersonalDir().c_str());
	return sohuvaCfg;
}

bool EnableAutoRunWithUpdate( bool enable )
{
	using namespace SohuTool;
	TCHAR szPath[MAX_PATH] = _T("");
	GetModuleFileName(NULL,szPath,MAX_PATH);
	*_tcsrchr(szPath,TCHAR('\\')) = TCHAR('\0');
	_tcscat(szPath,_T("\\SHUpdate.exe"));
	if(_taccess(szPath,0) != 0)
	{
		SohuTool::CSHLogWrapper::Instance().WriteLog(SHLog_Error,_T("更新程序不存在或无权限访问"));
		return false;
	}
	ATL::CString strcmdLine;
	if(enable)
		strcmdLine = _T("/EnableAutoStart");
	else
		strcmdLine = _T("/DisableAutoStart");

	SHELLEXECUTEINFO ShellInfo;
	memset(&ShellInfo, 0, sizeof(ShellInfo));
	ShellInfo.cbSize = sizeof(ShellInfo);
	ShellInfo.hwnd = NULL;
	ShellInfo.lpVerb = _T("open");
	ShellInfo.lpFile = szPath;
	ShellInfo.lpParameters = strcmdLine;
	ShellInfo.nShow  = SW_HIDE;
	ShellInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;

	BOOL bResult = ShellExecuteEx(&ShellInfo);
	if(!bResult)
		CSHLogWrapper::Instance().WriteLog(SHLog_Error,_T("启动SHUpdate错误 path = %s,%d"),szPath,GetLastError());
	if(bResult && ShellInfo.hProcess)
	{
		CloseHandle(ShellInfo.hProcess);
	}
	//保存到本地配置文件
	WritePrivateProfileString(_T("System"),_T("AutoRun"),enable?_T("1"):_T("0"),GetSohuVACfgPath());
	return true;
}



BOOL IsHaveShortcut( int nPlayListId, LPCTSTR szShPlayerPath, eSrcShortcut eSrc /*= SRC_CLIENT*/, std::wstring *pShortcutFilePath /*= NULL*/ )
{
	BOOL bResult = FALSE;
	HRESULT hResult; 
	IShellLink *pShellLink = NULL;
	IPersistFile *pPersistFile = NULL;

	//创建 COM 对象并获取其实现的接口
	hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pShellLink);
	if (FAILED(hResult))
	{
		return FALSE;
	}

	hResult = pShellLink->QueryInterface(IID_IPersistFile,(void **)&pPersistFile);
	if (FAILED(hResult))
	{
		pShellLink->Release();
		return FALSE;
	}

	//遍历桌面快捷方式目录
	std::wstring strPath;
	GetDesktopPath(strPath);
	std::wstring strDesktop(strPath);
	strDesktop += _T("\\*.*");

	WIN32_FIND_DATA FindData; 
	HANDLE hFile = FindFirstFile(strDesktop.c_str(), &FindData);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	std::wstring strFilePath;
	while (FindNextFile(hFile, &FindData)) 
	{ 
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		//判断是否是快捷方式文件
		std::wstring strFileName = FindData.cFileName;
		int nPos = strFileName.rfind(_T(".lnk"));
		if (nPos == std::wstring::npos)
		{
			continue;
		}

		//判断快捷方式文件的执行路径是否是SHPlayer.exe
		strFilePath = strPath;
		strFilePath += _T("\\");
		strFilePath += strFileName;
		TCHAR szExePath[MAX_PATH] = {0};
		hResult = pPersistFile->Load(strFilePath.c_str(), 0);
		hResult = pShellLink->GetPath(szExePath, MAX_PATH, NULL, SLGP_RAWPATH);
		if (_wcsnicmp(szExePath, szShPlayerPath, wcslen(szShPlayerPath)) != 0)
		{
			continue;			
		}

		//快捷方式的命令行参数
		TCHAR szParam[MAX_PATH] = {0};
		hResult = pShellLink->GetArguments(szParam, MAX_PATH);
		std::wstring strParam(szParam);

		//判断快捷方式标识
		wstring strQuickLunchFlag = _T("");
		switch (eSrc)
		{
		case SRC_CLIENT:
			strQuickLunchFlag = _T("\\QuickLunch=");
			break;
		case SRC_WEBPAGE:
			strQuickLunchFlag = _T("\\UrlQuickLunch=");
			break;
		case SRC_WHOLE_NETWORK:
			strQuickLunchFlag = _T("\\WholeNetworkQuickLunch=");
			break;
		case SRC_JINGXUAN:
			strQuickLunchFlag = _T("\\JingXuanQuickLunch=");
			break;
		default:
			break;
		}

		if (!strQuickLunchFlag.empty() && (strParam.find(strQuickLunchFlag.c_str()) == std::wstring::npos))
		{
			continue;
		}

		//PlayListID
		if (eSrc == SRC_WHOLE_NETWORK)
		{
			nPos = strParam.rfind(WHOLE_NETWOR_FLAG);
		}
		else
		{
			nPos = strParam.rfind(',');
		}
		if (nPos == std::wstring::npos)
		{
			continue;
		}
		std::wstring strPlayId = strParam.substr(nPos + (eSrc == SRC_WHOLE_NETWORK ? wcslen(WHOLE_NETWOR_FLAG) : 1));
		if (nPlayListId == _wtoi(strPlayId.c_str()))
		{
			bResult = TRUE;
			if (pShortcutFilePath != NULL)
			{
				*pShortcutFilePath = strFilePath;
			}
			break;
		}
	} 

	FindClose(hFile); 
	pPersistFile->Release();
	pShellLink->Release();
	return bResult;
}

int GetRegisterId()
{
	DWORD dwClientID = 0;
	HKEY hKey = NULL;
	LONG nResult = ::RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\SOHU\\SohuVA"), 0, KEY_READ, &hKey);
	if (nResult == ERROR_SUCCESS)
	{
		DWORD dwLen = sizeof(DWORD), dwType = 0;
		nResult = ::RegQueryValueEx(hKey, _T("RegisterId"), NULL, &dwType, (LPBYTE)&dwClientID, &dwLen);
		::RegCloseKey(hKey);
	}
	return dwClientID;
}

std::string InnerGetMachineCode()
{
	if(GetModuleHandle(_T("SHPlayerCommon.DLL")) != NULL)
	{
		LPCSTR (* QueryMachineCodeFunction)();
		QueryMachineCodeFunction = (LPCSTR (*)())GetProcAddress(GetModuleHandle(_T("SHPlayerCommon.DLL")),"QueryMachineCode");
		return QueryMachineCodeFunction();
	}
	else
	{
		return WMI_MachineCodeQuery();
	}
}

std::wstring SHPlayerURLEncode( LPCTSTR url )
{
#ifndef _NO_USE_ENCRPYT
	USES_CONVERSION;
	ATL::CString strUrl = url;
	ATL::CString tmp;

	if(strUrl.Find(_T("&uid=")) == -1)
	{
		tmp.Format(_T("&uid=%d"),GetRegisterId());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&LocalIp=")) == -1)
	{
		tmp.Format(_T("&LocalIp=%s"),GetIpStr(GetHostLocalIp()).c_str());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&MashCode=")) == -1)
	{
		string machineCode = SohuTool::easy_convert_to_base64(GetMachineCode());
		tmp.Format(_T("&MashCode=%s"),A2W(machineCode.c_str()));
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&NewMashCode=")) == -1)
	{
		string machineCode = InnerGetMachineCode();
		tmp.Format(_T("&NewMashCode=%s"),A2W(SohuTool::easy_convert_to_base64(machineCode).c_str()));
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&ChannelID=")) == -1)
	{
		tmp.Format(_T("&ChannelID=%d"),GetChannelNum());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&v=")) == -1)
	{
		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(szPath));
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		::PathRemoveFileSpec(szPath);
		_tcscat(szPath, _T("\\SHVersion.dll"));
		tmp.Format(_T("&v=%s"),GetVersionString(GetVersionNumber(szPath)).c_str());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&timestamp=")) == -1)
	{
		tmp.Format(_T("&timestamp=%d"),GetTickCount());
		strUrl.Append(tmp);		
	}
	if (strUrl.Find(_T("&btea=")) == -1)
	{
		string myurl = W2A(strUrl.GetBuffer());
		//
		myurl = easy_SHEncrypt((char*)myurl.data(), myurl.size(), Key3);

		tmp.Format(_T("&btea=%s"), GetMd5String(myurl).c_str());
		strUrl += tmp;
	}
	return (LPCTSTR)strUrl;
#else
	return L"";
#endif
}

std::wstring SHPlayerGuangSUURLEncode( LPCTSTR url )
{
#ifndef _NO_USE_ENCRPYT
	USES_CONVERSION;
	ATL::CString strUrl = url;
	ATL::CString tmp;

	if(strUrl.Find(_T("&uid=")) == -1)
	{
		int uid = GetRegisterId();
		if(uid == 0)
			uid = -1;
		tmp.Format(_T("&uid=%d"),uid);
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&LocalIp=")) == -1)
	{
		tmp.Format(_T("&LocalIp=%s"),GetIpStr(GetHostLocalIp()).c_str());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&MashCode=")) == -1)
	{
		string machineCode = SohuTool::easy_convert_to_base64(GetMachineCode());
		tmp.Format(_T("&MashCode=%s"),A2W(machineCode.c_str()));
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&NewMashCode=")) == -1)
	{
		string machineCode = InnerGetMachineCode();
		tmp.Format(_T("&NewMashCode=%s"),A2W(SohuTool::easy_convert_to_base64(machineCode).c_str()));
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&ChannelID=")) == -1)
	{
		tmp.Format(_T("&ChannelID=%d"),GetChannelNum());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&v=")) == -1)
	{
		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(szPath));
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		::PathRemoveFileSpec(szPath);
		_tcscat(szPath, _T("\\SHVersion.dll"));
		tmp.Format(_T("&v=%s"),GetVersionString(GetVersionNumber(szPath)).c_str());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&timestamp=")) == -1)
	{
		tmp.Format(_T("&timestamp=%d"),GetTickCount());
		strUrl.Append(tmp);		
	}
	if (strUrl.Find(_T("&btea=")) == -1)
	{
		string myurl = W2A(strUrl.GetBuffer());
		//
		myurl = easy_SHEncrypt((char*)myurl.data(), myurl.size(), Key3);

		tmp.Format(_T("&btea=%s"), GetMd5String(myurl).c_str());
		strUrl += tmp;
	}
	return (LPCTSTR)strUrl;
#else
	return L"";
#endif
}

std::wstring GetExeFilePath(void)
{
	TCHAR szPath[MAX_PATH];
	memset(szPath, 0, sizeof(szPath));
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	return szPath;
}

bool ShareFile(LPCTSTR szPath,LPCTSTR szDesc,LPCTSTR szHashId,int num,SHDType shtype)
{
	HWND hWnd = FindWindow(_T("{BC0FD8A0-6DE4-45e3-A0AC-D882F1C0962C}"),NULL);
	BOOL bTrue = (::IsWindow(hWnd) && szPath != NULL && szDesc != NULL && szHashId != NULL);
	if(bTrue)
	{
		SHDownloadCompleteInfo info = {0};
		_tcscpy(info.szPath,szPath);
		_tcscpy(info.szHash,szHashId);
		_tcscpy(info.szDesc,szDesc);
		info.num		= num;
		info.shtype		= shtype;
		COPYDATASTRUCT copydata = {0};
		copydata.dwData = 0;
		copydata.cbData = sizeof(info);
		copydata.lpData = &info;
		DWORD dwResult = 0;
		SendMessageTimeout(hWnd,WM_COPYDATA,NULL,(LPARAM)&copydata,SMTO_ABORTIFHUNG|SMTO_NORMAL,1000,&dwResult);
	}
	return bTrue;
}

std::wstring OpenSelectedFile(HWND wndParent)
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("媒体类型\0*.ifox; *.mp4; *.3gp; *.mov; *.qt; *.3gpp; *.avi; *.asf; *.wmv; *.rm; *.rmvb; *.ram; *.flv; *.f4v; *.flc; \
		   *.fli; *.mkv; *.mpg; *.ts; *.m2t; *.mpeg; *.m2v; *.m2p; *.m4v; *.vob;*.m2ts; *.mp3; *.ogg; *.wma; *.amr; *.wav; *.aac; *.flac; *.mp2\0")
		   _T("搜狐影音媒体(*.ifox)\0*.ifox\0")
		   _T("QT媒体(QuickTime)(*.mp4; *.3gp; *.mov; *.qt; *.3gpp)\0*.mp4; *.3gp; *.mov; *.qt; *.3gpp\0")
		   _T("Windows媒体(*.avi; *.asf; *.wmv)\0*.avi; *.asf; *.wmv\0")
		   _T("Real媒体(*.rm; *.rmvb; *.ram)\0*.rm; *.rmvb; *.ram\0")
		   _T("Flash媒体(*.flv; *.swf; *.f4v; *.flc; *.fli)\0*.flv; *.f4v; *.flc; *.fli\0")
		   _T("Matroska媒体(*.mkv)\0*.mkv\0")
		   _T("Mpeg媒体(*.mpg; *.ts; *.m2t; *.mpeg; *.m2v; *.m2p; *.m4v;)\0(*.mpg; *.ts; *.m2t; *.mpeg; *.m2v; *.m2p; *.m4v;\0")
		   _T("DVD和蓝光媒体(*.vob;*.m2ts)\0*.vob;*.m2ts\0")
		   _T("音频媒体(*.mp3; *.ogg; *.wma; *.amr; *.wav; *.aac; *.flac; *.mp2)\0*.mp3; *.ogg; *.wma; *.amr; *.wav; *.aac; *.flac; *.mp2\0")
		   _T("所有文件(*.*)\0*.*\0\0"),wndParent);
	if(dlg.DoModal() == IDOK)
	{
		return dlg.m_szFileName;
	}
	else
	{
		return L"";
	}
}


std::wstring  NatTypeDesc(SHNatType type)
{
	wstring desc;
	switch (type)
	{
	case NatType_ERROR:
		desc = (L"Error");
		break;
	case NatType_BLOCKED:
		desc = (L"Blocked");
		break;
	case NatType_OPEN_INTERNET:
		desc = (L"公网");
		break;
	case NatType_FULL_CONE:
		desc = (L"全CONE");
		break;
	case NatType_PORT_RESTRICTED:
		desc = (L"端口限制");
		break;
	case NatType_RESTRICTED:
		desc = (L"IP限制");
		break;
	case NatType_FIREWALL:
		desc = (L"防火墙");
		break;
	case NatType_SYMMETRIC_NAT:
		desc = (L"对称型");
		break;
	default:
		desc = (L"Unknown type");
		break;
	}
	return desc;
}


std::wstring BuildMd5Url( LPCTSTR str )
{
	USES_CONVERSION; 
	ATL::CString tmp;
	ATL::CString strUrl = str;
	if(strUrl.Find(_T("&uid=")) == -1)
	{
		tmp.Format(_T("&uid=%d"),GetRegisterId());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&LocalIp=")) == -1)
	{
		tmp.Format(_T("&LocalIp=%s"),GetIpStr(GetHostLocalIp()).c_str());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&MashCode=")) == -1)
	{
		string machineCode = SohuTool::easy_convert_to_base64(GetMachineCode());
		tmp.Format(_T("&MashCode=%s"),A2W(machineCode.c_str()));
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&NewMashCode=")) == -1)
	{
		string machineCode = InnerGetMachineCode();
		tmp.Format(_T("&NewMashCode=%s"),A2W(SohuTool::easy_convert_to_base64(machineCode).c_str()));
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&ChannelID=")) == -1)
	{
		tmp.Format(_T("&ChannelID=%d"),GetChannelNum());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&v=")) == -1)
	{
		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(szPath));
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		::PathRemoveFileSpec(szPath);
		_tcscat(szPath, _T("\\SHVersion.dll"));
		tmp.Format(_T("&v=%s"),GetVersionString(GetVersionNumber(szPath)).c_str());
		strUrl.Append(tmp);
	}
	if(strUrl.Find(_T("&timestamp=")) == -1)
	{
		tmp.Format(_T("&timestamp=%d"),GetTickCount());
		strUrl.Append(tmp);		
	}
	ATL::CString retStr;
	retStr.Format(_T("%s&key=%s"),strUrl,GetMd5String(T2A(strUrl+_T("t%#f+L97P6"))).c_str());
	return retStr.GetBuffer();
}

int DefinitionHotVrsToSetting(int nHotDefinition)
{
	int nDefiniton = 0;
	switch (nHotDefinition)
	{
	case VER_HIGH:
		nDefiniton = HIGH_DEFINITION;
		break;
	case VER_NORMAL:
		nDefiniton = NORM_DEFINITION;
		break;
	case VER_SUPER:
		nDefiniton = SUPER_DEFINITION;
		break;
	case VER_ORIGINAL:
		nDefiniton = ORIGI_DEFINITION;
		break;
	default:
		nDefiniton = AUTO_DEFINITION;
		break;
	}

	return nDefiniton;
}

int SettingToDefinitionHotVrs(int nSettingDefinition)
{
	int version = VER_HIGH;
	switch (nSettingDefinition)
	{
	case ORIGI_DEFINITION:
		version = VER_ORIGINAL;
		break;
	case SUPER_DEFINITION:
		version = VER_SUPER;
		break;
	case HIGH_DEFINITION:
		version = VER_HIGH;
		break;
	case NORM_DEFINITION:
		version = VER_NORMAL;
		break;
	}
	return version;
}

void ChangSHPlayerRelationWithFileAss(BOOL isReleation)
{
	using namespace SohuTool;
	TCHAR szPath[MAX_PATH] = _T("");
	GetModuleFileName(NULL,szPath,MAX_PATH);
	*_tcsrchr(szPath,TCHAR('\\')) = TCHAR('\0');
	_tcscat(szPath,_T("\\FileAssociationsTool.exe"));
	if(_taccess(szPath,0) != 0)
	{
		SohuTool::CSHLogWrapper::Instance().WriteLog(SHLog_Error,_T("更新程序不存在或无权限访问"));
		return;
	}
	ATL::CString strcmdLine;
	if(isReleation)
		strcmdLine = _T("/T");
	else
		strcmdLine = _T("/F");

	SHELLEXECUTEINFO ShellInfo;
	memset(&ShellInfo, 0, sizeof(ShellInfo));
	ShellInfo.cbSize = sizeof(ShellInfo);
	ShellInfo.hwnd = NULL;
	ShellInfo.lpVerb = _T("open");
	ShellInfo.lpFile = szPath;
	ShellInfo.lpParameters = strcmdLine;
	ShellInfo.nShow  = SW_HIDE;
	ShellInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;

	BOOL bResult = ShellExecuteEx(&ShellInfo);
	if(!bResult)
		CSHLogWrapper::Instance().WriteLog(SHLog_Error,_T("启动文件关联错误 path = %s,%d"),szPath,GetLastError());
	if(bResult && ShellInfo.hProcess)
	{
		CloseHandle(ShellInfo.hProcess);
	}
	return;
}

std::wstring GetStunIp()
{
    USES_CONVERSION;
    int i_stun_ip = 0;
    static ATL::CString stun_ip;
    if (!stun_ip.IsEmpty())
    {
        return (LPCTSTR)stun_ip;
    }
    hostent* hostInfo = gethostbyname("stun.p2p.hd.sohu.com");
    if (NULL != hostInfo)
    {
        //vector<string> ips;
        vector<int>ip_int;
        for(size_t i = 0; hostInfo->h_addr_list[i] != NULL; ++i)
        {
            //ips.push_back(inet_ntoa(*(in_addr *)hostInfo->h_addr_list[i]));
            ip_int.push_back(*(int*)hostInfo->h_addr_list[i]);
        }
        if (ip_int.size() >0 )
        {
            srand((unsigned)time(0));
            i_stun_ip = ip_int[rand() % (unsigned)ip_int.size()];
        }
        //memcpy(&i_stun_ip,hostInfo->h_addr,hostInfo->h_length);
        stun_ip.Format(_T("%s"), A2T(inet_ntoa(*(IN_ADDR*)&i_stun_ip)));
        return (LPCTSTR)stun_ip;
    }
    else
    {
        return _T("220.181.118.143");
    }
}