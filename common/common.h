// common.h ����ͷ�ļ�
//

#pragma once
#include "../include/ISuperUpload.h"
/*
----------------------------------------------------------
	Created:	2008/8/26
	
	File base:	common.h	
	Author:		feiwang@sohu-inc.com
	
	Purpose:	
				���幫�ýṹ֮�����������������̡�
----------------------------------------------------------
*/

// ������Ϣ��Ϣ (WPARAM: �ٷֱȽ���(1~100),LPARAM: NULL)
//
#define WM_UPLOAD_INFO		WM_USER + 112
#define ID_TASKBARICON		100
#define WM_ICONNOTIFY       WM_USER + 101 
#define WM_CUS_SHOW			WM_USER + 102
#define WM_EXIT_THREAD		WM_USER + 103	// �˳��߳���Ϣ
#define WM_GETBLOCK			WM_USER + 104	// ����ļ������
#define WM_HYPERLINK		WM_USER + 105	// ���������֪ͨ
#define WM_UPLOAD_OK		WM_USER + 106	// �ϴ��ɹ����


typedef struct tagPingbackInfo
{
	int		nErrorCode; //���ش����룬t=? 104-�򿪣� 105-�ɹ��˳��� 106-�쳣�˳���107-�����˳���108-�û������˳���5XX-�ϴ�����, 4XX-ת�����3XX-��ͼ����7XX-�û�ȡ��
	int		nVideoId;		//��ƵID i=?
	char	szPrivateVersion[1024]; //����汾�� v=?
	char	szHWID[1024]; //Ӳ���汾�� h=?
	char	szSource[100]; //�ϴ���Դ s=?
	int		nUserPos; //�û�����λ�� p=?
	int		nLabel; //�ϴ���ʽ m=1���ϴ��̶�Ϊ1��
	int		nVCodec; //��Ƶ���� c=?
	int		nACodec; //��Ƶ���� a=?
	int		nPercent; //�ٷֱ� b=?
	int		nUpratio; //�ϴ��ٶ� u=?
	char	szPassport[1024]; //passport e=?  ��Ƶ��ʽ
	char	szOsversion[1024]; //����ϵͳ�汾 os=?
	char	szIEversion[1024]; //IE�汾 ie=?
	int		nSize; //Դ��Ƶ��С size=?
	int		nDuration; //Դ��Ƶʱ�� dur=?  ��ǰ��Ƭ
	float	fSize;		// �ļ��ߴ�
} PingbackInfo, *PPingbackInfo;

/*
	��������Ϣ�ṹ
*/
typedef struct _UPLOAD_SERVER_INFO
{
	char  szID[256];
	int	  nUploaded;
	char  szFWD[512];
	char  szPTO[512];
	char  szVTO[512];
	int	  nErrCode;
    int   nSubCode;//���ֶ��������ֵ�nErrCode��ͬʱ����ǰ�ϴ���ƵӦ����Ϊ�����ϴ�״̬
	char  szErrMsg[512];
}UPLOAD_SERVER_INFO, *PUPLOAD_SERVER_INFO;

/*
	������Ϣ�ṹ
*/

typedef struct _UPLOAD_INFO
{
	char  szFile[520];
	char  szInfo[4096];
	char  szProfile[MAX_PATH];
	char  szVersion[128];
	
	BOOL bFirst;
	int	  nThreads;			// �ϴ��̸߳���
	long  nId;				// �ϴ�Ψһ��ʶ
	HWND pSink;
	UINT  uMsg;				// ������Ϣ
	FunLog funLog;
}UPLOAD_INFO, *PUPLOAD_INFO;

#define WEBPAGEINFO_SIZE	(4*1024)

typedef struct tagWebPageInfo
{
	TCHAR url[WEBPAGEINFO_SIZE];
	TCHAR flashvars[WEBPAGEINFO_SIZE];
	tagWebPageInfo()
	{
		memset(url,0,WEBPAGEINFO_SIZE*sizeof(TCHAR));
		memset(flashvars,0,WEBPAGEINFO_SIZE*sizeof(TCHAR));
	}
}DNetWebPageInfo;


#define HTTP_ACCEPT_TYPE		"*/*"
#define HTTP_ACCEPT				"Accept: */*\r\n"
#define DEFAULT_AGENT			"Mozilla/4.0 (compatible; )"
#define DEFAULT_BOUNDRY_TAG		"--FILE-FILE-FORM-DATA-BOUNDARY"
#define QUERY_SERVER			"my.tv.sohu.com"
#define PINGBACK_SERVER			"up.hd.sohu.com.cn"
#define CONTENT_TYPE_BDY		_T("Content-Type: multipart/form-data; boundary=--FILE-FILE-FORM-DATA-BOUNDARY\r\n")
#define CONTENT_TYPE			_T("Content-Type: application/x-www-form-urlencoded\r\n")
#define CONTENT_LENGTH			_T("Content-Length: 0\r\n")

//#define OBJECTS_NAME			_T("/v.jhtml?m=add&outType=xml&uploadFrom=7")
#define OBJECTS_NAME			_T("/user/a/wvideo/save.do?uploadFrom=7&")
#define PINGBACK_NAME			_T("/stat.gif?")
#define SEND_TAIL				_T("\r\n----FILE-FILE-FORM-DATA-BOUNDARY--\r\n\r\n")		// �ļ�����β��������

#define PARTIAL_SIZE			(1024 * 100)	// ÿ�����ݰ��Ĵ�СΪ500k��
#define SEND_BUF_SIZE			(1024 * 20)

#define MAX_THREADS				5	// ����߳���
#define MAX_BUFFER				4096

#define FILEMAXSIZE				2000*1024*1024
#define MD5MAXSIZE				200*1024*1024

#define		LOGIN_URL		_T("http://passport.sohu.com/sso/login.jsp?userid=%s&password=%s&appid=1000&persistentcookie=%d&pwdtype=1")
#define     TOKEN_URL		_T("http://passport.sohu.com/sso/gettoken.jsp?userid=%s&password=%s&appid=1044&livetime=20")
#define		RU_URL			_T("http://passport.sohu.com/sso/authtoken.jsp?userid=%s&appid=1044&token=%s&ru=%s&livetime=20")
#define     LOGOUT_URL		_T("http://passport.sohu.com/sso/logout.jsp?s=%d&appid=1000")

#define		URL_RESULT		_T("login_status=\'success\'")

#define		URL_AUTHEN		_T("http://my.tv.sohu.com/register.jhtml?m=sInfo&outType=3&c=%d")
#define		MAX_URL_BUF		1024 * 4
#define		MAX_PIC_FILE	1024 * 10	// �ϴ�ͼƬЭ��ĳߴ�

#define		URL_ACTIVE		_T("http://blog.sohu.com/login/activation.do")
#define		LOOK_VIDEO		_T("http://my.tv.sohu.com/u/vw/%s")
#define		USER_VIDEO		_T("http://my.tv.sohu.com/u/us/%s")

#define		TRAY_TEXT		_T("�Ѻ���Ƶ�ϴ�\n\n�ϴ����ȣ�%d%%\n�ϴ��ٶȣ�%.1fk/s\n")

#define		AUTOCOMPLE		_T("SOFTWARE\\SohuBlogVideoUpload\\AutoComple")
#define		SETTINGS		_T("SOFTWARE\\SohuBlogVideoUpload\\Settings")

#define		KEYI_CLASS_NAME	L"keyi_video_download"
