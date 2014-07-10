#pragma once
#include <string>
#include "wtypes.h"
#include "Util.h"
#include "SHCommonDef.h"
#include "SHPlayerDef.h"
#include "SHDefine.h"

//��ȡ�ļ���HASHֵ
std::wstring	GetFileHash(LPCTSTR szFilePath);
//���ܻ����ļ�
BOOL			EncryptSohuFile(LPCTSTR szFile);
void			GetSohuFileData(FILE* pfile,string& data);
//��ȡ�Ѻ�Ӱ���汾��
__int64			GetSHPlayerVersionNumber();
std::wstring	GetSHPlayerVersionString();
//��ȡ�Ѻ�Ӱ������Ŀ¼
inline std::wstring	GetFeihuPersonalDir()
{
	wstring strPath = GetPersonalDir();
	strPath += _T("\\�Ѻ�Ӱ��");
	if(_taccess(strPath.c_str(),0) != 0)
		CreateDirectory(strPath.c_str(),NULL);
	return strPath;
}
//��ȡ�Ѻ�Ӱ�������ļ�·��
std::wstring	GetSohuVAIniFilePath(void);
//���ÿ���������
void			EnableAutoRun(bool enable);
//��ȡ������
unsigned int	GetChannelNum(void);
//��ȡ������
std::wstring	GetReferID(void);
//��������״̬
int				CheckiResearchiClick();
void			ReadReg(LPCTSTR key, wstring &Value); 
//�жϿ�ݷ�ʽ�Ƿ����
BOOL			IsHaveShortcut(int nPlayListId, LPCTSTR szShPlayerPath, eSrcShortcut eSrc = SRC_CLIENT, std::wstring  *pShortcutFilePath = NULL);
//��ȡע��ID
int				GetRegisterId();
//URL���ܣ�����ͨ��UrlEncode��ͬ
std::wstring	SHPlayerURLEncode(LPCTSTR url);

std::wstring	SHPlayerGuangSUURLEncode(LPCTSTR url);//�����ڹ������뷨

std::wstring GetExeFilePath(void);
bool ShareFile(LPCTSTR szPath,LPCTSTR szDesc,LPCTSTR szHashId,int num,SHDType shtype);
std::wstring  OpenSelectedFile(HWND wndParent);

std::wstring  NatTypeDesc(SHNatType type);

std::wstring BuildMd5Url( LPCTSTR str );

bool EnableAutoRunWithUpdate( bool enable );
void ChangSHPlayerRelationWithFileAss(BOOL isReleation);
ATL::CString GetSohuVACfgPath();

//HOTVRS������ת��Ϊ�����е�������
int DefinitionHotVrsToSetting(int nHotDefinition);
//�����е�������ת��ΪHOTVRS������
int SettingToDefinitionHotVrs(int nSettingDefinition);

//��ȡstunip
wstring GetStunIp();