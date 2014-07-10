#pragma once
#include <string>
#include "wtypes.h"
#include "Util.h"
#include "SHCommonDef.h"
#include "SHPlayerDef.h"
#include "SHDefine.h"

//获取文件的HASH值
std::wstring	GetFileHash(LPCTSTR szFilePath);
//加密缓存文件
BOOL			EncryptSohuFile(LPCTSTR szFile);
void			GetSohuFileData(FILE* pfile,string& data);
//获取搜狐影音版本号
__int64			GetSHPlayerVersionNumber();
std::wstring	GetSHPlayerVersionString();
//获取搜狐影音配置目录
inline std::wstring	GetFeihuPersonalDir()
{
	wstring strPath = GetPersonalDir();
	strPath += _T("\\搜狐影音");
	if(_taccess(strPath.c_str(),0) != 0)
		CreateDirectory(strPath.c_str(),NULL);
	return strPath;
}
//获取搜狐影音配置文件路径
std::wstring	GetSohuVAIniFilePath(void);
//设置开机自启动
void			EnableAutoRun(bool enable);
//获取渠道号
unsigned int	GetChannelNum(void);
//获取邀请码
std::wstring	GetReferID(void);
//检测软件的状态
int				CheckiResearchiClick();
void			ReadReg(LPCTSTR key, wstring &Value); 
//判断快捷方式是否存在
BOOL			IsHaveShortcut(int nPlayListId, LPCTSTR szShPlayerPath, eSrcShortcut eSrc = SRC_CLIENT, std::wstring  *pShortcutFilePath = NULL);
//获取注册ID
int				GetRegisterId();
//URL加密，和普通的UrlEncode不同
std::wstring	SHPlayerURLEncode(LPCTSTR url);

std::wstring	SHPlayerGuangSUURLEncode(LPCTSTR url);//仅用于光速输入法

std::wstring GetExeFilePath(void);
bool ShareFile(LPCTSTR szPath,LPCTSTR szDesc,LPCTSTR szHashId,int num,SHDType shtype);
std::wstring  OpenSelectedFile(HWND wndParent);

std::wstring  NatTypeDesc(SHNatType type);

std::wstring BuildMd5Url( LPCTSTR str );

bool EnableAutoRunWithUpdate( bool enable );
void ChangSHPlayerRelationWithFileAss(BOOL isReleation);
ATL::CString GetSohuVACfgPath();

//HOTVRS清晰度转化为设置中的清晰度
int DefinitionHotVrsToSetting(int nHotDefinition);
//设置中的清晰度转化为HOTVRS清晰度
int SettingToDefinitionHotVrs(int nSettingDefinition);

//获取stunip
wstring GetStunIp();