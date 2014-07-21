#pragma once

#include "SingletonImpl.h"
#include <string>
#include <vector>

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

class CHLSMetaParser:
	public SingletonImpl<CHLSMetaParser>
{
public:
	CHLSMetaParser(void);
	~CHLSMetaParser(void);
	HLSMetaData parseHLSStream(const wchar_t* m3u8_address);
	HLSMetaData parseHLSMeta(std::string response);
};
