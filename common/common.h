// common.h 公用头文件
//

#pragma once
#include "../include/ISuperUpload.h"
/*
----------------------------------------------------------
	Created:	2008/8/26
	
	File base:	common.h	
	Author:		feiwang@sohu-inc.com
	
	Purpose:	
				定义公用结构之处，适用于整个工程。
----------------------------------------------------------
*/

// 上载信息消息 (WPARAM: 百分比进度(1~100),LPARAM: NULL)
//
#define WM_UPLOAD_INFO		WM_USER + 112
#define ID_TASKBARICON		100
#define WM_ICONNOTIFY       WM_USER + 101 
#define WM_CUS_SHOW			WM_USER + 102
#define WM_EXIT_THREAD		WM_USER + 103	// 退出线程消息
#define WM_GETBLOCK			WM_USER + 104	// 获得文件块个数
#define WM_HYPERLINK		WM_USER + 105	// 点击热连接通知
#define WM_UPLOAD_OK		WM_USER + 106	// 上传成功标记


typedef struct tagPingbackInfo
{
	int		nErrorCode; //返回错误码，t=? 104-打开， 105-成功退出， 106-异常退出，107-出错退出，108-用户主动退出，5XX-上传出错, 4XX-转码出错，3XX-截图出错，7XX-用户取消
	int		nVideoId;		//视频ID i=?
	char	szPrivateVersion[1024]; //软件版本号 v=?
	char	szHWID[1024]; //硬件版本号 h=?
	char	szSource[100]; //上传来源 s=?
	int		nUserPos; //用户所在位置 p=?
	int		nLabel; //上传方式 m=1（上传固定为1）
	int		nVCodec; //视频编码 c=?
	int		nACodec; //音频编码 a=?
	int		nPercent; //百分比 b=?
	int		nUpratio; //上传速度 u=?
	char	szPassport[1024]; //passport e=?  视频格式
	char	szOsversion[1024]; //操作系统版本 os=?
	char	szIEversion[1024]; //IE版本 ie=?
	int		nSize; //源视频大小 size=?
	int		nDuration; //源视频时长 dur=?  当前分片
	float	fSize;		// 文件尺寸
} PingbackInfo, *PPingbackInfo;

/*
	服务器信息结构
*/
typedef struct _UPLOAD_SERVER_INFO
{
	char  szID[256];
	int	  nUploaded;
	char  szFWD[512];
	char  szPTO[512];
	char  szVTO[512];
	int	  nErrCode;
    int   nSubCode;//此字段用于区分当nErrCode相同时，当前上传视频应设置为何种上传状态
	char  szErrMsg[512];
}UPLOAD_SERVER_INFO, *PUPLOAD_SERVER_INFO;

/*
	上载信息结构
*/

typedef struct _UPLOAD_INFO
{
	char  szFile[520];
	char  szInfo[4096];
	char  szProfile[MAX_PATH];
	char  szVersion[128];
	
	BOOL bFirst;
	int	  nThreads;			// 上传线程个数
	long  nId;				// 上传唯一标识
	HWND pSink;
	UINT  uMsg;				// 进度消息
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
#define SEND_TAIL				_T("\r\n----FILE-FILE-FORM-DATA-BOUNDARY--\r\n\r\n")		// 文件发送尾部结束符

#define PARTIAL_SIZE			(1024 * 100)	// 每个数据包的大小为500k。
#define SEND_BUF_SIZE			(1024 * 20)

#define MAX_THREADS				5	// 最大线程数
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
#define		MAX_PIC_FILE	1024 * 10	// 上传图片协议的尺寸

#define		URL_ACTIVE		_T("http://blog.sohu.com/login/activation.do")
#define		LOOK_VIDEO		_T("http://my.tv.sohu.com/u/vw/%s")
#define		USER_VIDEO		_T("http://my.tv.sohu.com/u/us/%s")

#define		TRAY_TEXT		_T("搜狐视频上传\n\n上传进度：%d%%\n上传速度：%.1fk/s\n")

#define		AUTOCOMPLE		_T("SOFTWARE\\SohuBlogVideoUpload\\AutoComple")
#define		SETTINGS		_T("SOFTWARE\\SohuBlogVideoUpload\\Settings")

#define		KEYI_CLASS_NAME	L"keyi_video_download"
