#pragma once


//#include <sqlite3.h>
#include <boost/assert.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/tuple/tuple.hpp>
#include "FxDbMacro.h"
#include "FxException.h"
struct sqlite3_stmt;
struct sqlite3;
#define CONSTRUCT_FXBINDER_IMPL_MACRO(x,y,z,w) template< EXPAND(x) > fxDbBinder( EXPAND(y) ):binderImpl_(new fxDbBinderImpl<typename boost::tuples::detail::make_tuple_mapper< EXPAND(w) >::type>(boost::make_tuple( EXPAND(z) ))){}
#define REAL_CONSTRUCT_FXBINDER_IMPL(n,...) CONSTRUCT_FXBINDER_IMPL_MACRO(RBIND(n,COMMA,TYPE_PARAM,__VA_ARGS__),RBIND(n,COMMA,FUN_PARAM_MAKETUPLE,__VA_ARGS__),RBIND(n,COMMA,FUN_IMPL_MAKETUPLE,__VA_ARGS__),RBIND(n,COMMA,TYPE_IMPL_PARAM,__VA_ARGS__))
#define MAKEFXBINDER_IMPL_ONELINE(x,y) EXPAND(REAL_CONSTRUCT_FXBINDER_IMPL(x))
extern unsigned short fxerror_db;
namespace fxsys
{
	namespace fxsqlite3
	{
		enum fxdb_error_id {OPENDB_FAILED,TRANS_WITHOUT_DBIMPL, FX_DBERR_HOLDER, INNER_DB_ERROR};
		class fxdb_error:public fx_exception
		{
		public:
			fxdb_error(fxdb_error_id val, std::wstring const& errMsg):fx_exception(fxerror_db, val) { }
			~fxdb_error() { }
			virtual std::wstring get_msg() const { return m_msg; }
		private:
			std::wstring m_msg;
		};
		const fxdb_error &getFxdbError(fxdb_error_id val);
		template<int n>struct Int2Type
		{
			enum {val=n};
		};

		class fxDbBinder;
		class data_iterator;
		class fxDbImpl:public boost::enable_shared_from_this<fxDbImpl>
		{
			friend class data_iterator;
			friend class fxDbTrans;
		public:
			fxDbImpl(std::wstring const&wdbname);
			bool tableExist(std::wstring const&wtablename);
			int execDML(const std::wstring& szSQL, fxDbBinder const &fxbinder);
			data_iterator execQuery(const std::wstring& szSQL, fxDbBinder const &fxbinder);
			bool setKey(const void* key,int nKeysize);
			bool resetKey(const void* key,int nKeysize);

		private:
			void inner_throw(fxdb_error const& err);
			bool dberror_flag;
			boost::shared_ptr<sqlite3> dbinstance_;
		};

		class fxDbBinder
		{
		private:
			template<class T> static bool bind_element(sqlite3_stmt* pstmt,int nCol, T val);
			struct fxDbBinderImplBase{virtual bool dobind(sqlite3_stmt* pstmt)=0;virtual ~fxDbBinderImplBase(){}};
			template<class tuple_type> struct fxDbBinderImpl:public fxDbBinderImplBase
			{
				tuple_type bindObjs_;
				fxDbBinderImpl(tuple_type const& val):bindObjs_(val){}
				virtual bool dobind(sqlite3_stmt* pstmt){return inner_bind(pstmt,Int2Type<boost::tuples::length<tuple_type>::value>());}
			private:
				template<class IntType> bool inner_bind(sqlite3_stmt* pstmt, IntType)
				{
//#ifdef _WIN32 // 在GCC下编译不过，不知道为什么
					if(!fxDbBinder::bind_element(pstmt,IntType::val, boost::tuples::get< IntType::val - 1 >( bindObjs_ )))return false;
//#endif
					return inner_bind(pstmt,Int2Type<IntType::val-1>());
				}
                bool inner_bind(sqlite3_stmt* pstmt, Int2Type<1>)
                {
//#ifdef _WIN32 // 在GCC下编译不过，不知道为什么
					return fxDbBinder::bind_element( pstmt, 1, boost::tuples::get<0>(bindObjs_) );
//#else
//                    return true;
//#endif
                }
			};
		public:
			struct blob:public std::string
			{
				blob(){}
				blob(const void* ptr, int len):std::string((char*)ptr,len){}
				void* get_ptr() const{return (void*)c_str();}
			};
			fxDbBinder(){}
			CONSTRUCT(MAX_MEMBER_NUMBER,MAKEFXBINDER_IMPL_ONELINE);
			bool dobind(sqlite3_stmt* pstmt) const {if(binderImpl_) return binderImpl_->dobind(pstmt);return true;}
		private:
			boost::shared_ptr<fxDbBinderImplBase> binderImpl_;
		};
		
		class fxDbTrans
		{
		public:
			fxDbTrans( boost::shared_ptr<fxDbImpl> impl );
			~fxDbTrans();
		private:
			boost::weak_ptr<fxDbImpl> dbimpl_;
		};
		class data_iterator
		{
		public:
			data_iterator();
			data_iterator( boost::shared_ptr<fxDbImpl> pimpl, boost::shared_ptr<sqlite3_stmt> stmt );
			data_iterator(data_iterator const& dst);
			template<class T>T getField(int nCol)
			{
				T val;
 				getField(nCol,val);
				return val;
			}
			template<class T>void getField(int nCol, T&val);
			bool operator==(data_iterator const& dst);
			bool operator!=(data_iterator const& dst);
			data_iterator* operator->(){return this;}
			data_iterator& operator++();
		private:
			boost::weak_ptr<fxDbImpl> dbimpl_;
			boost::shared_ptr<sqlite3_stmt> dbstmt_;
		};
		class fxDb
		{
		public:
			fxDb(std::wstring const&wdbname);
			bool tableExists(std::wstring const&wtablename);
			boost::shared_ptr<fxDbTrans> beginTrans();
			int execDML(const std::wstring& sql, fxDbBinder fxbinder = fxDbBinder());
			data_iterator execQuery(const std::wstring& sql, fxDbBinder fxbinder = fxDbBinder());
			std::wstring fxDbVersion();
			bool setKey(const void* key,int nKeysize);
			bool resetKey(const void* key,int nKeysize);

		private:
			boost::shared_ptr<fxDbImpl> dbimpl_;
		};
	}
}
