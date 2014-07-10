#pragma once
#include <xstring>
class fx_exception
{
public:
	fx_exception( unsigned short code_category, unsigned short code_error );
	virtual ~fx_exception();
	virtual std::wstring get_msg() const { return L""; }
	unsigned int get_full_code() const { return ( m_code_category << 16 ) + m_code_error; }
	std::wstring get_full_msg() const;
	unsigned short get_category_code() const { return m_code_category; }
	unsigned short get_error_code() const { return m_code_error; }
private:
	unsigned short m_code_category;
	unsigned short m_code_error;
};
