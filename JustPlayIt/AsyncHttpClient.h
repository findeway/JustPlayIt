#pragma once

#include <curl/curl.h>

typedef void (*OnResponseCallback)(unsigned int statusCode, unsigned int errCode, const std::string& response, void* userdata);
typedef void (*OnReceiveCallback)(unsigned int statusCode, unsigned int errCode, void* buffer, size_t size, void* userdata);

enum EHttp_Request
{
    EHttp_Get = 0,
    EHttp_Post = 1
};

class CAsyncHttpClient
{
public:
    static bool Init();
    static bool UnInit();

    CAsyncHttpClient(void);
    ~CAsyncHttpClient(void);

    //����һЩ����ѡ��
    void AddHeader(const std::wstring& strHeader);
    void SetUserAgent(const wchar_t* szUserAgent);
    void SetFollowLocation(bool bFollow);
    void SetProxy(const wchar_t* szProxy);

    /*
     *	����ȡ��Ӧ�壬���ڲ�ѯ���ļ�����Ӧͷ��Ϣ
     */
    void SetNoBody(bool bNoBody);

    /*
     *	������Ӧ�ص�
     */
    void SetRecvCallback(OnReceiveCallback callback, void* userdata);

    /*
     *	�첽��ʼ����
     */
    void Request(const wchar_t* szUrl, EHttp_Request requestMethod, OnResponseCallback callback, void* userdata);

    /*
     *	ִ�н������ݻص�
     */
    void DoRecvCallback(void* buffer, size_t size);

    void InnerRequest();
protected:
    static DWORD __stdcall StartRequest(LPVOID param);
    static size_t OnRecvData(void* buffer, size_t size, size_t nmemb, void* userp);

protected:
    CURL*						m_handleCurl;
    OnResponseCallback			m_responseCallback;				//������ɻص�
    OnReceiveCallback			m_receiveCallback;				//�������ݻص�

    std::wstring				m_strUseragent;
    bool						m_bFollowLocation;
    std::wstring				m_strProxy;
    std::wstring				m_strUrl;
    EHttp_Request				m_requestMethod;
    curl_slist*					m_httpHeaders;
    bool						m_bNoBody;

    unsigned int				m_statusCode;
    unsigned int				m_errCode;
    __int64						m_contentSize;
    std::string					m_strResponse;

    void*						m_recvUserData;
    void*						m_respUserData;

    HANDLE						m_hRequestThread;
};
