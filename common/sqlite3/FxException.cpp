#include "stdafx.h"
#include "FxException.h"
#include <boost/format.hpp>


fx_exception::fx_exception( unsigned short code_category, unsigned short code_error )
{
	m_code_category = code_category;
	m_code_error = code_error;
}

std::wstring fx_exception::get_full_msg() const
{
	boost::wformat format( L"[%4x][%4x]%s" );
	return boost::str( format % m_code_category % m_code_error % get_msg() );
}

fx_exception::~fx_exception()
{

}

unsigned short fxerror_xml = 0x0101;
unsigned short fxerror_db = 0x0102;
