#include <Windows.h>
#include <WinDef.h>
#include <tchar.h>
#include <atlconv.h>
#include "Util.h"
#include "LighteningUtil.h"
#include <comutil.h>
#include <WinIoCtl.h>
#include <Wbemidl.h>
#include <strsafe.h>
#include <algorithm>
#include <atlconv.h>
#include <ntddndis.h>
#include <atltrace.h>
#include <atlstr.h>
#include "Base64.h"
#include "LogWrapper.h"
#include <winnt.h>
#include <winbase.h>
#include <atltime.h>
#include <wininet.h>

#pragma comment (lib, "comsuppw.lib")
#pragma comment (lib, "wbemuuid.lib")
#pragma comment (lib, "Wininet.lib")

#define PROPERTY_MAX_LEN	128	// �����ֶ���󳤶�
typedef struct _T_DEVICE_PROPERTY
{
	TCHAR szProperty[PROPERTY_MAX_LEN];
} T_DEVICE_PROPERTY;

typedef struct _T_WQL_QUERY
{
	CHAR*	szSelect;		// SELECT���
	WCHAR*	szProperty;		// �����ֶ�
} T_WQL_QUERY;

// WQL��ѯ���
const T_WQL_QUERY szWQLQuery[] = {
	// ����ԭ��MAC��ַ
	"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))",
	L"PNPDeviceID",

	// Ӳ�����к�SerialNumber
	"SELECT * FROM Win32_PhysicalMedia WHERE (SerialNumber IS NOT NULL)",
	L"SerialNumber",

	// �������к�
	"SELECT * FROM Win32_BaseBoard WHERE (SerialNumber IS NOT NULL)",
	L"SerialNumber",	

	//// ������ID
	//"SELECT * FROM Win32_Processor WHERE (ProcessorId IS NOT NULL)",
	//L"ProcessorId",

	//BIOS���к�
	"SELECT * FROM Win32_BIOS WHERE (SerialNumber IS NOT NULL)",
	L"SerialNumber",

	//// �����ͺ�
	//"SELECT * FROM Win32_BaseBoard WHERE (Product IS NOT NULL)",
	//L"Product",

	// ������ǰMAC��ַ
	//"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))",
	//L"MACAddress",
};
//
const T_WQL_QUERY szWQLQuery_Win7[] = {
	// ����ԭ��MAC��ַ
	"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))",
	L"PNPDeviceID",

	// Ӳ�����к�SerialNumber
	"SELECT * FROM Win32_DiskDrive WHERE (SerialNumber IS NOT NULL) AND (MediaType LIKE 'Fixed hard disk%')",
	L"SerialNumber",

	// �������к�
	"SELECT * FROM Win32_BaseBoard WHERE (SerialNumber IS NOT NULL)",
	L"SerialNumber",	

	//// ������ID
	//"SELECT * FROM Win32_Processor WHERE (ProcessorId IS NOT NULL)",
	//L"ProcessorId",

	//BIOS���к�
	"SELECT * FROM Win32_BIOS WHERE (SerialNumber IS NOT NULL)",
	L"SerialNumber",

	//// �����ͺ�
	//"SELECT * FROM Win32_BaseBoard WHERE (Product IS NOT NULL)",
	//L"Product",

	// ������ǰMAC��ַ
	//"SELECT * FROM Win32_NetworkAdapter WHERE (MACAddress IS NOT NULL) AND (NOT (PNPDeviceID LIKE 'ROOT%'))",
	//L"MACAddress",
};

static BOOL WMI_DoWithPNPDeviceID( const TCHAR *PNPDeviceID, TCHAR *MacAddress, UINT uSize )

{
	TCHAR	DevicePath[MAX_PATH];
	HANDLE	hDeviceFile;	
	BOOL	isOK = FALSE;

	StringCchCopy( DevicePath, MAX_PATH, TEXT("\\\\.\\") );
	StringCchCat( DevicePath, MAX_PATH, PNPDeviceID );
	StringCchCat( DevicePath, MAX_PATH, TEXT("#{ad498944-762f-11d0-8dcb-00c04fc3358c}") );

	std::replace( DevicePath + 4, DevicePath + 4 + _tcslen(PNPDeviceID), TEXT('\\'), TEXT('#') ); 

	hDeviceFile = CreateFile( DevicePath,
		0,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if( hDeviceFile != INVALID_HANDLE_VALUE )
	{	
		ULONG	dwID;
		BYTE	ucData[8];
		DWORD	dwByteRet;		

		dwID = OID_802_3_PERMANENT_ADDRESS;
		isOK = DeviceIoControl( hDeviceFile, IOCTL_NDIS_QUERY_GLOBAL_STATS, &dwID, sizeof(dwID), ucData, sizeof(ucData), &dwByteRet, NULL );
		if( isOK )
		{	
			for( DWORD i = 0; i < dwByteRet; i++ )
			{
				StringCchPrintf( MacAddress + (i << 1), uSize - (i << 1), TEXT("%02X"), ucData[i] );
			}
		}
		CloseHandle( hDeviceFile );
	}
	return isOK;
}

static BOOL WMI_DoWithHarddiskSerialNumber( TCHAR *SerialNumber, UINT uSize )
{
	UINT	iLen;
	UINT	i;
	SohuTool::CSHLogWrapper::Instance().WriteLog(SohuTool::SHLog_Desc,_T("Original HDSerialNumber = %s"),SerialNumber);
	iLen = _tcslen( SerialNumber );
	if( iLen == 40 )	// InterfaceType = "IDE"
	{	// ��Ҫ��16���Ʊ��봮ת��Ϊ�ַ���
		TCHAR ch, szBuf[32];
		BYTE b;		

		for( i = 0; i < 20; i++ )
		{	// ��16�����ַ�ת��Ϊ��4λ
			ch = SerialNumber[i * 2];
			if( (ch >= '0') && (ch <= '9') )
			{
				b = ch - '0';
			}
			else if( (ch >= 'A') && (ch <= 'F') )
			{
				b = ch - 'A' + 10;
			}
			else if( (ch >= 'a') && (ch <= 'f') )
			{
				b = ch - 'a' + 10;
			}
			else
			{	// �Ƿ��ַ�
				break;
			}

			b <<= 4;

			// ��16�����ַ�ת��Ϊ��4λ
			ch = SerialNumber[i * 2 + 1];
			if( (ch >= '0') && (ch <= '9') )
			{
				b += ch - '0';
			}
			else if( (ch >= 'A') && (ch <= 'F') )
			{
				b += ch - 'A' + 10;
			}
			else if( (ch >= 'a') && (ch <= 'f') )
			{
				b += ch - 'a' + 10;
			}
			else
			{	// �Ƿ��ַ�
				break;
			}

			szBuf[i] = b;
		}

		if( i == 20 )
		{	// ת���ɹ�
			szBuf[i] = L'\0';
			StringCchCopy( SerialNumber, uSize, szBuf );
			iLen = _tcslen( SerialNumber );
		}
	}

	// ÿ2���ַ�����λ��
	for( i = 0; i < iLen; i += 2 )
	{
		std::swap( SerialNumber[i], SerialNumber[i+1] );
	}

	// ȥ���ո�
	std::remove( SerialNumber, SerialNumber + _tcslen(SerialNumber) + 1, L' ' );
	SohuTool::CSHLogWrapper::Instance().WriteLog(SohuTool::SHLog_Desc,_T("After Conversion HDSerialNumber = %s"),SerialNumber);
	return TRUE;
}

static BOOL WMI_DoWithProperty( INT iQueryType, TCHAR *szProperty, UINT uSize )
{
	BOOL isOK = TRUE;

	switch( iQueryType )
	{
	case 0:		// ����ԭ��MAC��ַ		
		isOK = WMI_DoWithPNPDeviceID( szProperty, szProperty, uSize );
		break;

	case 1:	
		isOK = WMI_DoWithHarddiskSerialNumber( szProperty, uSize );
		break;
	default:
		// ȥ���ո�
		std::remove( szProperty, szProperty + _tcslen(szProperty) + 1, L' ' );
	}
	return isOK;
}

bool CheckMachineCode(CString &pmac, CString &phdSN, CString &pBIOS, CString &pBaseBoard, CString &pUserName, CString &pinstallTime)
{
	TCHAR  TempSN[MAX_PATH] = _T("");
	DWORD  dwlen			= MAX_PATH*sizeof(TCHAR);
	DWORD  dwType		    = REG_SZ;
	HKEY   subKey			= NULL;
	bool   IsChange		= false;
	CString szTemp = _T("");
	if(::RegOpenKey(HKEY_LOCAL_MACHINE,_T("Software\\SOHU\\SoHuVA"),&subKey) == ERROR_SUCCESS)
	{
		ZeroMemory(TempSN, sizeof(TempSN));
		if (RegQueryValueEx(subKey,_T("MAC"),NULL,&dwType,(LPBYTE)TempSN,&dwlen) == ERROR_SUCCESS)
		{
			if (_tcscmp(pmac,TempSN) != 0)
			{
				dwType		    = REG_SZ;
				::RegSetValueEx(subKey,_T("MAC"),NULL,dwType,(LPBYTE)pmac.GetBuffer(),pmac.GetLength()*2);
				IsChange = true;
			}
		}
		else
		{
			dwType		    = REG_SZ;
			::RegSetValueEx(subKey,_T("MAC"),NULL,dwType,(LPBYTE)pmac.GetBuffer(),pmac.GetLength()*2);
		}
		//
		ZeroMemory(TempSN, sizeof(TempSN));
		dwlen			= MAX_PATH*sizeof(TCHAR);
		dwType		    = REG_SZ;
		if (RegQueryValueEx(subKey,_T("hdSN"),NULL,&dwType,(LPBYTE)TempSN,&dwlen) == ERROR_SUCCESS)
		{
			if (_tcscmp(phdSN,TempSN) != 0)
			{
				dwType		    = REG_SZ;
				::RegSetValueEx(subKey,_T("hdSN"),NULL,dwType,(LPBYTE)phdSN.GetBuffer(),phdSN.GetLength()*2);
				IsChange = true;
			}
		}
		else
		{
			dwType		    = REG_SZ;
			::RegSetValueEx(subKey,_T("hdSN"),NULL,dwType,(LPBYTE)phdSN.GetBuffer(),phdSN.GetLength()*2);
		} 
		//
		ZeroMemory(TempSN, sizeof(TempSN));
		dwlen			= MAX_PATH*sizeof(TCHAR);
		dwType		    = REG_SZ;
		if (RegQueryValueEx(subKey,_T("BIOSSN"),NULL,&dwType,(LPBYTE)TempSN,&dwlen) == ERROR_SUCCESS)
		{
			if (_tcscmp(pBIOS,TempSN) != 0)
			{
				dwType		    = REG_SZ;
				::RegSetValueEx(subKey,_T("BIOSSN"),NULL,dwType,(LPBYTE)pBIOS.GetBuffer(),pBIOS.GetLength()*2);
				IsChange = true;
			}
		}
		else
		{
			dwType		    = REG_SZ;
			::RegSetValueEx(subKey,_T("BIOSSN"),NULL,dwType,(LPBYTE)pBIOS.GetBuffer(),pBIOS.GetLength()*2);
		}
		//
		ZeroMemory(TempSN, sizeof(TempSN));
		dwlen			= MAX_PATH*sizeof(TCHAR);
		dwType		    = REG_SZ;
		if (RegQueryValueEx(subKey,_T("BaseBoardSN"),NULL,&dwType,(LPBYTE)TempSN,&dwlen) == ERROR_SUCCESS)
		{
			if (_tcscmp(pBaseBoard,TempSN) != 0)
			{
				dwType		    = REG_SZ;
				::RegSetValueEx(subKey,_T("BaseBoardSN"),NULL,dwType,(LPBYTE)pBaseBoard.GetBuffer(),pBaseBoard.GetLength()*2);
				IsChange = true;
			}
		}
		else
		{
			dwType		    = REG_SZ;
			::RegSetValueEx(subKey,_T("BaseBoardSN"),NULL,dwType,(LPBYTE)pBaseBoard.GetBuffer(),pBaseBoard.GetLength()*2);
		}
		//
		ZeroMemory(TempSN, sizeof(TempSN));
		dwlen			= MAX_PATH*sizeof(TCHAR);
		dwType		    = REG_SZ;
		if (RegQueryValueEx(subKey,_T("UserName"),NULL,&dwType,(LPBYTE)TempSN,&dwlen) == ERROR_SUCCESS)
		{
			if (_tcscmp(pUserName,TempSN) != 0)
			{
				dwType		    = REG_SZ;
				::RegSetValueEx(subKey,_T("UserName"),NULL,dwType,(LPBYTE)pUserName.GetBuffer(),pUserName.GetLength()*2);
				IsChange = true;
			}
		}
		else
		{
			dwType		    = REG_SZ;
			::RegSetValueEx(subKey,_T("UserName"),NULL,dwType,(LPBYTE)pUserName.GetBuffer(),pUserName.GetLength()*2);
		}
		//
		ZeroMemory(TempSN, sizeof(TempSN));
		dwlen			= MAX_PATH*sizeof(TCHAR);
		dwType		    = REG_SZ;
		if (RegQueryValueEx(subKey,_T("InstallTimer"),NULL,&dwType,(LPBYTE)TempSN,&dwlen) == ERROR_SUCCESS)
		{
			if (_tcscmp(pinstallTime,TempSN) != 0)
			{
				dwType		    = REG_SZ;
				::RegSetValueEx(subKey,_T("InstallTimer"),NULL,dwType,(LPBYTE)pinstallTime.GetBuffer(),pinstallTime.GetLength()*2);
				IsChange = true;
			}
		}
		else
		{
			dwType		    = REG_SZ;
			::RegSetValueEx(subKey,_T("InstallTimer"),NULL,dwType,(LPBYTE)pinstallTime.GetBuffer(),pinstallTime.GetLength()*2);
		}
		::RegCloseKey(subKey);
	}
	return IsChange;
}
string WMI_MachineCodeQuery()
{
	static string machineCode;
	if (!machineCode.empty())
	{
		return machineCode;
	}
	string old_machinecode = SohuTool::easy_convert_to_base64(GetMachineCode());
	HRESULT hres;
	hres = CoInitializeSecurity(NULL, -1, NULL,	NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
	if( hres != RPC_E_TOO_LATE && FAILED(hres) )
	{
		return old_machinecode;
	}
	// ���WMI����COM�ӿ�
	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator,reinterpret_cast<LPVOID*>(&pLoc)); 
	if( FAILED(hres) )
	{
		return  old_machinecode;
	}
	// ͨ�����ӽӿ�����WMI���ں˶�����"ROOT\\CIMV2"
	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer( _bstr_t( L"ROOT\\CIMV2" ), NULL, NULL,  NULL, 0, NULL, NULL, &pSvc);    
	if( FAILED(hres) )
	{
		pLoc->Release(); 
		return old_machinecode;
	}
	// �����������İ�ȫ����
	hres = CoSetProxyBlanket(pSvc,	RPC_C_AUTHN_WINNT,	RPC_C_AUTHZ_NONE,NULL,
		RPC_C_AUTHN_LEVEL_CALL,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);
	if( FAILED(hres) )
	{
		pSvc->Release();
		pLoc->Release();     
		return  old_machinecode;
	}
	//�ж��Ƿ���vista ���ϰ汾
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	wstring strVer = _T("");

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *) &osvi)) 
			return FALSE;
	}
	bool Isvista = osvi.dwMajorVersion >= 6 ?true:false;
	//
	T_DEVICE_PROPERTY devpro[4];
	memset(devpro,0,sizeof(devpro));
	IEnumWbemClassObject *pEnumerator = NULL;
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;
	const T_WQL_QUERY const *pWQL = Isvista ? szWQLQuery_Win7 : szWQLQuery;
	for (int i= 0; i<4 ; i++)
	{
		// ͨ�������������WMI��������
		pEnumerator = NULL;
		hres = pSvc->ExecQuery(bstr_t("WQL"), bstr_t( pWQL[i].szSelect ),	
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumerator);

		if( FAILED(hres) )
		{
			pSvc->Release();
			pLoc->Release();
			continue;
		}

		pclsObj = NULL;
		uReturn = 0;
		pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj,	&uReturn);
		if( uReturn == 0 )
		{
			pEnumerator->Release();
			continue;
		}
		VARIANT vtProperty;
		VariantInit( &vtProperty );	
		pclsObj->Get( pWQL[i].szProperty, 0, &vtProperty, NULL, NULL );
		StringCchCopy( devpro[i].szProperty, PROPERTY_MAX_LEN-1, (wchar_t *)(_bstr_t)_variant_t(vtProperty) );
		VariantClear( &vtProperty );
		WMI_DoWithProperty( i, devpro[i].szProperty, PROPERTY_MAX_LEN-1 );
		pclsObj->Release();
		pEnumerator->Release();
	}
	pSvc->Release();
	pLoc->Release();

	CString  mac = devpro[0].szProperty;
	CString  hdSN = devpro[1].szProperty;
	CString  BaseBoardSN = devpro[2].szProperty;
	CString  BIOSSN	= devpro[3].szProperty;
	CString MachineCode;

	if (mac.GetLength() > 0)
	{
		MachineCode += mac;
		MachineCode += _T("#");
	}
	if (hdSN.GetLength() > 0)
	{
		MachineCode += hdSN;
		MachineCode += _T("#");
	}
	if (BIOSSN.GetLength() > 0)
	{
		MachineCode += BIOSSN;
	}
	MachineCode += _T("~");
	if (BaseBoardSN.GetLength() > 0)
	{
		MachineCode += BaseBoardSN;
		MachineCode += _T("#");
	}
	//
	CString username = OS_GetUserName().c_str();
	CString installTime = OS_GetSysInstallTime().c_str();

	CheckMachineCode(mac,hdSN,BIOSSN,BaseBoardSN,username,installTime);
	return machineCode = MachineCode.IsEmpty() ? old_machinecode: W2Utf8(MachineCode) + W2Utf8(username) + W2Utf8(installTime);
}
