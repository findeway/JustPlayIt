#ifndef __DLL_CHECK_H__
#define __DLL_CHECK_H__

#include "detours.h"
#pragma comment(lib, "detours.lib")

//以下适用于Detours Express 3.0 版本
static HMODULE (WINAPI* OldLoadLibraryA)(LPCSTR lpFileName) = LoadLibraryA;
HMODULE WINAPI NewLoadLibraryA(LPCSTR lpFileName)
{
	char szPath[MAX_PATH] = {0}; 
	if (strstr(lpFileName, "ieframe.dll") != NULL)
	{
		GetSystemDirectoryA(szPath, MAX_PATH);
		strcat(szPath, "\\ieframe.dll");
		lpFileName = szPath;
	}

	return OldLoadLibraryA(lpFileName);
}

void ProcessAttach(void)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)OldLoadLibraryA, NewLoadLibraryA);
	DetourTransactionCommit();
}

void ProcessDetach(void)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)OldLoadLibraryA, NewLoadLibraryA);
	DetourTransactionCommit();
}

#if 0
//Detours Express 2.1版本
typedef HMODULE (WINAPI* LoadLibrarySys)(LPCSTR lpFileName);
LoadLibrarySys SystemLoadLibraryA = NULL;
LoadLibrarySys RealSystemLoadLibraryA = NULL;

HMODULE WINAPI LoadLibraryAMine(LPCSTR lpFileName)
{
	char szPath[MAX_PATH] = {0};
	if (strstr(lpFileName, "ieframe.dll") != NULL)
	{
		GetSystemDirectoryA(szPath, MAX_PATH);
		strcat(szPath, "\\ieframe.dll");
		lpFileName = szPath;
	}

	return RealSystemLoadLibraryA(lpFileName);
}

BOOL ProcessAttach(void) 
{
	SystemLoadLibraryA = (LoadLibrarySys)DetourFindFunction("Kernel32.dll", "LoadLibraryA");
	if (SystemLoadLibraryA == NULL)
	{
		return FALSE;
	}

	RealSystemLoadLibraryA = (LoadLibrarySys)DetourFunction((PBYTE)SystemLoadLibraryA, (PBYTE)LoadLibraryAMine);

	return TRUE; 
} 
#endif


#endif //__DLL_CHECK_H__