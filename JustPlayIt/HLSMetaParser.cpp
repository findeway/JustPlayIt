#include "StdAfx.h"
#include "HLSMetaParser.h"
#include "SyncHttpClient.h"
#include "Util.h"

CHLSMetaParser::CHLSMetaParser(void)
{
}

CHLSMetaParser::~CHLSMetaParser(void)
{
}

HLSMetaData CHLSMetaParser::parseHLSStream( const wchar_t* m3u8_address )
{
	std::string response = CSyncHttpClient::Instance()->GetHttpResponse(m3u8_address);
	return parseHLSMeta(response);
}

HLSMetaData CHLSMetaParser::parseHLSMeta( std::string response )
{
	HLSMetaData metadata = {0};
	std::vector<std::string> vecM3U8Info = SpliterString(response,"\r");
	for(int index = 0; index < vecM3U8Info.size(); index++)
	{
		if(vecM3U8Info[index].find("EXTINF") != std::string::npos)
		{
			std::vector<std::string> vecKeyValue = SpliterString(vecM3U8Info[index],":");
			if(vecKeyValue.size() >= 2)
			{
				int endPos = vecKeyValue[1].find(",");
				if(endPos != std::string::npos)
				{
					vecKeyValue[1] = vecKeyValue[1].substr(0,endPos);
				}
				//记录时长和url
				M3U8Segment segment = {atoi(vecKeyValue[1].c_str()),Utf82W(vecM3U8Info[index+1].c_str())};
				metadata.segments.push_back(segment);
				metadata.playlist_count++;
				metadata.duration += segment.duration;
			}
		}
	}
	return metadata;
}
