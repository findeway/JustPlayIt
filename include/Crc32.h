#pragma once

#include "SohuToolExport.h"

const DWORD CRC32_NEGL = 0xffffffffL;

namespace SohuTool
{
	class SOHUTOOL_API CCrc32
	{
	public:
		CCrc32();
		~CCrc32();
		unsigned int GetCrc32( const BYTE *input, unsigned int length );
	private:
		void TruncatedFinal( BYTE *hash, unsigned int size );
		unsigned int DigestSize() const;
		void UpdateByte( BYTE b );
		BYTE GetCrcByte( unsigned int i ) const;

		void Reset();
		unsigned int CRC32_INDEX ( unsigned int c );
		unsigned int CRC32_SHIFTED ( unsigned int c );
		BOOL IsLittleEndianCpu();
	private:
		static const unsigned int *s_tab  ;
		unsigned int m_crc;

	};

}