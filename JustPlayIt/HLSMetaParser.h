#pragma once

#include "SingletonImpl.h"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "AsyncHttpClient.h"

typedef struct tagM3U8Segment
{
	int duration;
	std::wstring url;
}M3U8Segment;

typedef struct tagHLSMetaData
{
	__int64 duration;
	int		playlist_count;
	std::vector<M3U8Segment> segments;
}HLSMetaData;

typedef void (*OnGetHlsMeta)(const HLSMetaData& metaData,void* userdata);

class CHLSMetaParser:
	public SingletonImpl<CHLSMetaParser>
{
public:
	CHLSMetaParser(void);
	~CHLSMetaParser(void);
	void parseHLSStream(const wchar_t* m3u8_address,OnGetHlsMeta callback,void* userdata);
	HLSMetaData parseHLSMeta(std::string response);

	static void OnResponse(unsigned int statusCode,unsigned int errCode,const std::string& response,void* userdata);
	void OnResponseCallback(const HLSMetaData& metaData);
protected:
	OnGetHlsMeta		m_callback;
	void*				m_callbackUserdata;
protected:
	boost::shared_ptr<CAsyncHttpClient>	m_pClient;
};
