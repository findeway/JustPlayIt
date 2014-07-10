#include "stdafx.h"
#include "ZlibStream.h"
#include <boost/thread/mutex.hpp>
#include "smart_ptr/scoped_array.hpp"

namespace fxsys
{
	namespace zip
	{
		namespace detail
		{
			#pragma pack (push,1)

			struct GZipHeader
			{
				GZipHeader();
				unsigned char ID1;	//fixed:0x1F
				unsigned char ID2;	//fixed:0x8B
				unsigned char CM;	//fixed:8
				unsigned char FLG;	//bit 0 FTEXT - 指示文本数据
				//bit 1 FHCRC - 指示存在CRC16头校验字段
				//bit 2 FEXTRA - 指示存在可选项字段
				//bit 3 FNAME - 指示存在原文件名字段
				//bit 4 FCOMMENT - 指示存在注释字段
				//bit 5-7 保留
				int MTIME;	//更改时间
				unsigned char XFL;	//1字节。附加的标志。当CM = 8时，XFL = 2 - 最大压缩但最慢的算法；XFL = 4 - 最快但最小压缩的算法
				unsigned char OS;	//1字节。操作系统，确切地说应该是文件系统。有下列定义：
				//0 - FAT文件系统 (MS-DOS, OS/2, NT/Win32)
				//1 - Amiga
				//2 - VMS/OpenVMS
				//3 - Unix
				//4 - VM/CMS
				//5 - Atari TOS
				//6 - HPFS文件系统 (OS/2, NT)
				//7 - Macintosh
				//8 - Z-System
				//9 - CP/M
				//10 - TOPS-20
				//11 - NTFS文件系统 (NT)
				//12 - QDOS
				//13 - Acorn RISCOS
				//255 - 未知

			};

			#pragma pack (pop)

			//额外的头字段：
			//(若 FLG.FEXTRA = 1)
			//
			//+---+---+---+---+===============//================+
			//|SI1|SI2|  XLEN |      长度为XLEN字节的可选项     |
			//+---+---+---+---+===============//================+

			struct GZHFlgExtra
			{
				unsigned char SI1;
				unsigned char SI2;
				unsigned int XLEN;
			};

			//(若 FLG.FNAME = 1)
			//
			//+=======================//========================+
			//|               原文件名(以NULL结尾)              |
			//+=======================//========================+
			//(若 FLG.FCOMMENT = 1)
			//
			//+=======================//========================+
			//|   注释文字(只能使用iso-8859-1字符，以NULL结尾)  |
			//+=======================//========================+
			//(若 FLG.FHCRC = 1)
			//
			//+---+---+
			//| CRC16 |
			//+---+---+

			struct GZipTailer
			{
				int CRC32;
				int ISIZE;
			};

			static const unsigned char GZH_FLG_FTEXT = 0x01;
			static const unsigned char GZH_FLG_FHCRC = 0x02;
			static const unsigned char GZH_FLG_FEXTRA = 0x04;
			static const unsigned char GZH_FLG_FNAME = 0x08;
			static const unsigned char GZH_FLG_FCOMMENT = 0x10;


			static const unsigned char GZH_OS_FAT = 0;				//0 - FAT文件系统 (MS-DOS, OS/2, NT/Win32)
			static const unsigned char GZH_OS_AMIGA = 1;				//1 - Amiga
			static const unsigned char GZH_OS_VMS = 2;				//2 - VMS/OpenVMS
			static const unsigned char GZH_OS_UNIX = 3;				//3 - Unix
			static const unsigned char GZH_OS_CMS = 4;				//4 - VM/CMS
			static const unsigned char GZH_OS_ATARITOS = 5;				//5 - Atari TOS
			static const unsigned char GZH_OS_HPFS = 6;				//6 - HPFS文件系统 (OS/2, NT)
			static const unsigned char GZH_OS_MACINTOSH = 7;				//7 - Macintosh
			static const unsigned char GZH_OS_ZSYSTEM = 8;				//8 - Z-System
			static const unsigned char GZH_OS_CP = 9;				//9 - CP/M
			static const unsigned char GZH_OS_TOPS20 = 10;			//10 - TOPS-20
			static const unsigned char GZH_OS_NTFS = 11;				//11 - NTFS文件系统 (NT)
			static const unsigned char GZH_OS_QDOS = 12;				//12 - QDOS
			static const unsigned char GZH_OS_ACORN = 13;			//13 - Acorn RISCOS
			static const unsigned char GZH_OS_UNKNOWN = 255;			//255 - 未知

			GZipHeader::GZipHeader()
			{
				ID1 = 0x1F;
				ID2 = 0x8B;
				CM = 8;
				FLG = 0;
				MTIME = (int)time( NULL );
				XFL = 4;
				OS = 0;
			}

			boost::mutex g_compress_mutex;
		}


		CZlibStream::CZlibStream( int nCompressLevel,
			int nWBits, int nMemLevel, int nStrategyType , int nMethod )
			: m_nMethod( nMethod ),
			m_nCompressLevel( nCompressLevel ),
			m_nWBits( nWBits ),
			m_nMemLevel( nMemLevel ),
			m_nStrategyType( nStrategyType ),
			m_bInitInflate( false ),
			m_bInitDeflate( false )
		{
			::memset( &m_zInflate , 0 , sizeof( m_zInflate ) );
			::memset( &m_zDeflate , 0 , sizeof( m_zDeflate ) );
		}

		CZlibStream::~CZlibStream( void )
		{
			inflateEnd( &m_zInflate );
			deflateEnd( &m_zDeflate );
		}

		bool CZlibStream::CompressGZip( const char *src , int len , std::string &dstData )
		{
			dstData.clear();
			detail::GZipHeader gzh;
			dstData.append( (char*)&gzh , sizeof(gzh) );
			bool bOK = Compress( src , len , dstData );
			if( bOK )
			{
				detail::GZipTailer gzt;
				gzt.ISIZE = len ;
				gzt.CRC32 = 0;
				dstData.append( (char*)&gzt , sizeof(gzt) );
			}
			return bOK;
		}

		bool CZlibStream::DecompressGZip( const char *src , unsigned int len , std::string &dstData )
		{
			if( len < sizeof( detail::GZipHeader ) + sizeof( detail::GZipTailer ) )
			{
				return false;
			}
			dstData.clear();
			detail::GZipHeader gzh;
			memcpy( &gzh , src , sizeof( gzh ) );
			src += sizeof( gzh );
			len -= sizeof( gzh );

			if( ( gzh.FLG &  detail::GZH_FLG_FEXTRA ) == 1 )
			{
				detail::GZHFlgExtra extra;
				memcpy( &extra , src , sizeof( extra ) );
				src += sizeof( extra );
				len -= sizeof( extra );
				src += extra.XLEN;
				len -= extra.XLEN;
			}

			if( ( gzh.FLG &  detail::GZH_FLG_FNAME ) == 1 )
			{
				int fnamelen = strlen( src) + 1;
				src += fnamelen;
				len -= fnamelen;
			}

			if( ( gzh.FLG &  detail::GZH_FLG_FCOMMENT ) == 1 )
			{
				int fcommentlen = strlen( src) + 1;
				src += fcommentlen;
				len -= fcommentlen;
			}

			if( ( gzh.FLG &  detail::GZH_FLG_FHCRC ) == 1 )
			{
				short crc  = 0;
				memcpy( &crc , src , sizeof( crc ) );
				src += sizeof( crc );
				len -= sizeof( crc );
			}
			len -= sizeof( detail::GZipTailer );

			return Decompress( src , len , dstData );

		}

		bool CZlibStream::CompressGZip( const std::string& srcData , std::string &dstData )
		{
			return CompressGZip( srcData.data() , srcData.size() , dstData );
		}

		bool CZlibStream::DecompressGZip( const std::string& srcData , std::string &dstData )
		{
			return DecompressGZip( srcData.data() , srcData.size() , dstData );
		}

		bool CZlibStream::Compress( const char *src , int len , std::string &dstData )
		{
			//dstData.clear();
			if ( len <= 0 )
			{
				return true;
			}

			if ( !m_bInitDeflate )
			{
				m_zDeflate.zalloc = Z_NULL;
				m_zDeflate.zfree = Z_NULL;
				m_zDeflate.opaque = Z_NULL;
				int ret = deflateInit2( &m_zDeflate, m_nCompressLevel, m_nMethod, m_nWBits, m_nMemLevel, m_nStrategyType );
				if ( ret != Z_OK )
				{
					return false;
				}
				m_bInitDeflate = true;
			}

			boost::mutex::scoped_lock lock( detail::g_compress_mutex );
			int CHUNK = len + ( len / 100 ) + 13;

			boost::scoped_array< unsigned char > sa( new unsigned char[ CHUNK ] );

			char *in = const_cast<char*>( src );
			m_zDeflate.avail_in = len;
			m_zDeflate.next_in = ( Bytef* )in;
			do
			{
				m_zDeflate.avail_out = CHUNK;
				m_zDeflate.next_out = ( Bytef* )sa.get();
				int ret = deflate( &m_zDeflate, Z_SYNC_FLUSH );
				if ( ret != Z_OK )
				{
					return false;
				}
				dstData.append( reinterpret_cast< char* >( sa.get() ), CHUNK - m_zDeflate.avail_out );
			} while ( m_zDeflate.avail_out == 0 );
			return true;
		}

		bool CZlibStream::Decompress( const char *src , int len , std::string &dstData )
		{
			//dstData.clear();
			if ( len <= 0 )
			{
				return false;
			}
			if ( !m_bInitInflate )
			{
				m_zInflate.zalloc = Z_NULL;
				m_zInflate.zfree = Z_NULL;
				m_zInflate.opaque = Z_NULL;
				m_zInflate.avail_in = 0;
				m_zInflate.next_in = Z_NULL;
				int ret = inflateInit2( &m_zInflate , m_nWBits );
				if ( ret != Z_OK )
				{
					return false;
				}
				m_zInflate.avail_in = 0;
				m_zInflate.next_in = Z_NULL;
				m_bInitInflate = true;
			}

			int CHUNK = 4096;
			boost::scoped_array< unsigned char > sa( new unsigned char[ CHUNK ] );
			char *in = const_cast<char*>( src );
			m_zInflate.avail_in = len;
			m_zInflate.next_in = ( Bytef* )in;
			do
			{
				m_zInflate.avail_out = CHUNK;
				m_zInflate.next_out = ( Bytef* )sa.get();
				int ret = inflate( &m_zInflate, Z_SYNC_FLUSH );
				if ( ret != Z_OK )
				{
					dstData.append( reinterpret_cast< char* >( sa.get() ), CHUNK - m_zInflate.avail_out );
					if( Z_STREAM_END == ret ) return true;
					return false;
				}
				dstData.append( reinterpret_cast< char* >( sa.get() ), CHUNK - m_zInflate.avail_out );
			}while ( m_zInflate.avail_out == 0 );
			return true;
		}


		bool CZlibStream::Compress( const std::string& srcData , std::string &dstData )
		{
			return Compress( srcData.data() , (int)srcData.size() , dstData );
		}

		bool CZlibStream::Decompress( const std::string& srcData , std::string &dstData )
		{
			return Decompress( srcData.data() , (int)srcData.size() , dstData );
		}

	}
}
