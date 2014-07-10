#include "stdafx.h"
#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_array.hpp>
#include "sqlite3.h"
#include "FxDb.h"
//#include "TxtUtil.h"
//#include "fxconfig.h"


namespace fxsys
{
	namespace fxsqlite3
	{
		void  convert_wcs_to_utf16(const std::wstring& wcs, unsigned short* out)
		{
			unsigned int i =0;
			while( i < wcs.length())
			{
				out[i] = (unsigned short)wcs[i];
				++i;
			}
			out[wcs.length()] = 0;
		}

		std::wstring convert_utf16_wcs( const unsigned short* in )
		{
			std::wstring ret;
			const unsigned short* p = in;
			for( ; *p; p++ )
			{
				wchar_t ch = wchar_t(*p);
				ret += ch;
			}
			return ret;
		}

		const fxdb_error & getFxdbError(fxdb_error_id val)
		{
			static boost::array<fxdb_error,FX_DBERR_HOLDER> err_info =
			{
				fxdb_error( OPENDB_FAILED, L"unable to open database" ),
				fxdb_error( TRANS_WITHOUT_DBIMPL, L"a transaction operation can not be finished because the db instance killed" )
			};
			return err_info[val];
		}

		fxdb_error generateFxdbError(sqlite3* stat)
		{
			std::wstring msg = reinterpret_cast< const wchar_t* >( sqlite3_errmsg16( stat ) );
			return fxdb_error( INNER_DB_ERROR, msg );
		}

		//implement of fxDbImpl------------------------------------------------------------------------------
		fxDbImpl::fxDbImpl(std::wstring const&wdbname):dberror_flag(false)
		{
			sqlite3* pSqlite;
			boost::shared_array<unsigned short> utf16(new unsigned short[wdbname.size() + 1]);
			convert_wcs_to_utf16(wdbname,utf16.get());
			if(sqlite3_open16( utf16.get(),&pSqlite)!=SQLITE_OK)throw getFxdbError(OPENDB_FAILED);
			dbinstance_ = boost::shared_ptr<sqlite3>(pSqlite,sqlite3_close);
		}
		int fxDbImpl::execDML(const std::wstring& sql, fxDbBinder const& fxbinder)
		{
			execQuery(sql,fxbinder);
			return sqlite3_changes(dbinstance_.get());
		}
		data_iterator fxDbImpl::execQuery(const std::wstring& sql, fxDbBinder const& fxbinder)
		{
			const wchar_t *tail = NULL;
			sqlite3_stmt* pstmt;
			boost::shared_array<unsigned short> utf16(new unsigned short[sql.size() + 1]);
			convert_wcs_to_utf16(sql,utf16.get());

			if (sqlite3_prepare16(dbinstance_.get(), utf16.get(), (int)sql.length()*2, &pstmt, (const void**)&tail)!=SQLITE_OK)	inner_throw(generateFxdbError(dbinstance_.get()));
            boost::shared_ptr<sqlite3_stmt> spstmt(pstmt,sqlite3_finalize);
			if(!fxbinder.dobind(pstmt))inner_throw(generateFxdbError(dbinstance_.get()));
			switch (sqlite3_step(pstmt))
			{
			case SQLITE_DONE:return data_iterator();
			case SQLITE_ROW:return data_iterator(shared_from_this(),spstmt);
			default:inner_throw (generateFxdbError(dbinstance_.get()));
			}
			return data_iterator();//avoid compile warning
		}
		bool fxDbImpl::tableExist(std::wstring const&wtablename)
		{
			return execQuery(L"select count(*) from sqlite_master where type='table' and name=?",fxDbBinder(wtablename))->getField<int>(0)>0;
		}

		void fxDbImpl::inner_throw( fxdb_error const& err )
		{
			dberror_flag = true;
			throw err;
		}

		bool fxDbImpl::setKey( const void* key,int nKeysize )
		{
			bool success = false;
#ifdef SQLITE_HAS_CODEC
			success = sqlite3_key( dbinstance_.get(), key,nKeysize)== SQLITE_OK;
#endif
			return success;
		}

		bool fxDbImpl::resetKey( const void* key,int nKeysize )
		{
			bool success = false;
#ifdef SQLITE_HAS_CODEC
			success = sqlite3_rekey( dbinstance_.get(), key,nKeysize)== SQLITE_OK;
#endif
			return success;
		}

		//implement of fxDbTrans-------------------------------------------------------------------------
		fxDbTrans::fxDbTrans( boost::shared_ptr<fxDbImpl> impl ) : dbimpl_( impl )
		{
			if(!impl)throw getFxdbError(TRANS_WITHOUT_DBIMPL);
			impl->dberror_flag = false;
			impl->execDML(L"begin transaction",fxDbBinder());
		}
		fxDbTrans::~fxDbTrans()
		{
			boost::shared_ptr<fxDbImpl> tmpsp(dbimpl_);
			if(!tmpsp)throw getFxdbError(TRANS_WITHOUT_DBIMPL);
			if(tmpsp->dberror_flag)tmpsp->execDML(L"rollback transaction",fxDbBinder());
			else tmpsp->execDML(L"commit transaction",fxDbBinder());
		}
		//implement of fxDbBinder------------------------------------------------------------------------------
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, std::string value)
		{
			int nRes = sqlite3_bind_text(pstmt, nIndex, value.c_str(), -1, SQLITE_TRANSIENT);
			if (nRes != SQLITE_OK) return false;
			return true;
		}
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, const char* value)
		{
			int nRes = sqlite3_bind_text(pstmt, nIndex, value, -1, SQLITE_TRANSIENT);
			if (nRes != SQLITE_OK) return false;
			return true;
		}
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, std::wstring value)
		{
			boost::shared_array<unsigned short> utf16(new unsigned short[value.size() + 1]);
			convert_wcs_to_utf16(value,utf16.get());

			int nRes = sqlite3_bind_text16(pstmt, nIndex, utf16.get(), -1, SQLITE_TRANSIENT);
			if (nRes != SQLITE_OK) return false;
			return true;
		}
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, const wchar_t* value)
		{
			return bind_element(pstmt,nIndex,std::wstring(value));
		}
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, int value)
		{
			int nRes = sqlite3_bind_int(pstmt, nIndex, value);
			if (nRes != SQLITE_OK) return false;
			return true;
		}
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, double value)
		{
			int nRes = sqlite3_bind_double(pstmt, nIndex, value);
			if (nRes != SQLITE_OK) return false;
			return true;
		}
		template<> bool fxDbBinder::bind_element(sqlite3_stmt* pstmt,int nIndex, fxDbBinder::blob value)
		{
			int nRes = sqlite3_bind_blob(pstmt, nIndex, value.get_ptr(), value.length(), SQLITE_TRANSIENT);
			if (nRes != SQLITE_OK) return false;
			return true;
		}

		//implement of data_iterator---------------------------------------------------------------------
		data_iterator::data_iterator( boost::shared_ptr<fxDbImpl> pimpl, boost::shared_ptr<sqlite3_stmt> stmt ) : dbimpl_(pimpl), dbstmt_(stmt) {}
		data_iterator::data_iterator(){}
		data_iterator::data_iterator(data_iterator const& dst)
		{
			dbimpl_ = dst.dbimpl_;
			dbstmt_ = dst.dbstmt_;
		}
		bool data_iterator::operator==(data_iterator const& dst){return dbstmt_ == dst.dbstmt_;}
		bool data_iterator::operator!=(data_iterator const& dst) {return !(*this == dst);}
		data_iterator& data_iterator::operator++()
		{
			int nRet = sqlite3_step(dbstmt_.get());
			if (nRet == SQLITE_DONE)dbstmt_.reset();
			else if(nRet==SQLITE_ROW){}
			else boost::shared_ptr<fxDbImpl>(dbimpl_)->inner_throw(generateFxdbError(boost::shared_ptr<fxDbImpl>(dbimpl_)->dbinstance_.get()));
			return *this;
		}
		template<> void data_iterator::getField(int nCol,int& val)
		{
			val=sqlite3_column_int(dbstmt_.get(),nCol);
		}
        template<> void data_iterator::getField(int nCol,__int64& val)
        {
            val=sqlite3_column_int64(dbstmt_.get(),nCol);
        }
		template<> void data_iterator::getField(int nCol,double& val)
		{
			val=sqlite3_column_double(dbstmt_.get(),nCol);
		}
		template<> void data_iterator::getField(int nCol,std::string& val)
		{
			val=(char*)sqlite3_column_text(dbstmt_.get(),nCol);
		}
		template<> void data_iterator::getField(int nCol,std::wstring&val)
		{
			const unsigned short* value = (unsigned short*)sqlite3_column_text16(dbstmt_.get(),nCol);
			if ( value)
			{
				val = convert_utf16_wcs(value);
			}
		}
		template<> void data_iterator::getField(int nCol,fxDbBinder::blob&val)
		{
			val= fxDbBinder::blob(sqlite3_column_blob(dbstmt_.get(),nCol),sqlite3_column_bytes(dbstmt_.get(),nCol));
		}

		//implement of fxDb------------------------------------------------------------------------------
		fxDb::fxDb(std::wstring const&wdbname):dbimpl_(new fxDbImpl(wdbname)){}
		bool fxDb::tableExists(std::wstring const&wtablename)
		{
			return dbimpl_->tableExist(wtablename);
		}
		int fxDb::execDML(const std::wstring& sql, fxDbBinder fxbinder)
		{
			return dbimpl_->execDML(sql,fxbinder);
		}
		data_iterator fxDb::execQuery(const std::wstring& sql, fxDbBinder fxbinder)
		{
			return dbimpl_->execQuery(sql,fxbinder);
		}
		std::wstring fxDb::fxDbVersion()
		{
			return L"v1.0.00build0001";
		}
		boost::shared_ptr<fxDbTrans> fxDb::beginTrans()
		{
			return boost::shared_ptr<fxDbTrans>(new fxDbTrans(dbimpl_));
		}

		bool fxDb::setKey( const void* key,int nKeysize )
		{
			return dbimpl_->setKey(key,nKeysize);
		}

		bool fxDb::resetKey( const void* key,int nKeysize )
		{
			return dbimpl_->resetKey(key,nKeysize);
		}
	}
}

//implement of fxdbtest----------------------------------------------------------------------------
#ifdef _DEBUG
#include <iostream>
void testfxdb()
{
// 	using namespace fxsys::fxsqlite3;
// 	using namespace std;
// 	fxDb db(L"test.db");
// 	fxDb db2(L"test.db");
// 
// 	if(!db.tableExists(L"yctest")) db.execDML(L"create table yctest (name varchar, age integer, usrdata blob)");
// 	//transactions block
// 	{
// 		boost::shared_ptr<fxDbTrans> tmpsp = db.beginTrans();
// 		wstring name1 = L"john's mother";int age1 = 200;
// 		db.execDML(L"insert into yctest(name,age) values ('mike',20)");
// 		char* pdata= "hello";
// 		db.execDML(L"insert into yctest values (?,? ,?)", fxDbBinder(name1, age1, fxDbBinder::blob((void*)pdata,strlen(pdata))));
// 	}
// 	//transactions finished.
// 	data_iterator it = db.execQuery(L"select age,name,usrdata from yctest");
// 	while(it!=data_iterator())
// 	{
// 		int qryAge = it->getField<int>(0);
// 		wstring qryName = it->getField<wstring>(1);
// 		fxDbBinder::blob qryData = it->getField<fxDbBinder::blob>(2);
// 		wcout<<qryAge<<"\t"<<qryName<<endl;
// 		it=data_iterator();//test the reader release so the db can be write again.
// 		break;
// 	}
// 	db2.execDML(L"update yctest set age=55");

}
#endif