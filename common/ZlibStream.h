#pragma once
//#define ZLIB_WINAPI
#include "zlib.h"
#include <string>

namespace fxsys
{
	namespace zip
	{

		class CZlibStream
		{
		public:
			CZlibStream( int nCompressLevel = Z_DEFAULT_COMPRESSION ,
				int nWBits = -MAX_WBITS , int nMemLevel = MAX_MEM_LEVEL, int nStrategyType = Z_DEFAULT_STRATEGY , int nMethod = Z_DEFLATED );
			~CZlibStream( void );
		private:
			CZlibStream( const CZlibStream &other );
			CZlibStream& operator=( const CZlibStream &other );

			//GZip 格式压缩
		public:
			bool CompressGZip( const char *src , int len , std::string &dstData );
			bool DecompressGZip( const char *src , unsigned int len , std::string &dstData );
			bool CompressGZip( const std::string& srcData , std::string &dstData );
			bool DecompressGZip( const std::string& srcData , std::string &dstData );

			//原始deflate 格式压缩
		public:
			bool Compress( const char *src , int len , std::string &dstData );
			bool Decompress( const char *src , int len , std::string &dstData );
			bool Compress( const std::string& srcData , std::string &dstData );
			bool Decompress( const std::string& srcData , std::string &dstData );

		private:
			z_stream m_zInflate;
			z_stream m_zDeflate;

			bool m_bInitInflate;
			bool m_bInitDeflate;

			//#define Z_NO_COMPRESSION         0
			//#define Z_BEST_SPEED             1
			//#define Z_BEST_COMPRESSION       9
			//#define Z_DEFAULT_COMPRESSION  (-1)
			int m_nCompressLevel;

			//#define Z_DEFLATED 8
			int m_nMethod;

			//#define MAX_WBITS 15
			//#define DEF_WBITS MAX_WBITS
			int m_nWBits;

			//#define MAX_MEM_LEVEL
			//#define DEF_MEM_LEVEL 8
			int m_nMemLevel;

			//#define Z_FILTERED            1
			//#define Z_HUFFMAN_ONLY        2
			//#define Z_RLE                 3
			//#define Z_FIXED               4
			//#define Z_DEFAULT_STRATEGY    0
			int m_nStrategyType;
		};
	}
}
