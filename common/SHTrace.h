#pragma once
#include <set>
#include <windows.h>
using namespace std;

class CSHCriticalSection
{
private:
	CRITICAL_SECTION m_sec;
public:
	CSHCriticalSection(){InitializeCriticalSection(&m_sec);}
	~CSHCriticalSection(){DeleteCriticalSection(&m_sec);}
	void Lock(){EnterCriticalSection(&m_sec);}
	void UnLock(){LeaveCriticalSection(&m_sec);}
	CRITICAL_SECTION& operator()(){return m_sec;}
};


template<typename T>
class CScopeLock
{
};



template<>
class CScopeLock<CRITICAL_SECTION>
{	
public:
	CScopeLock(CRITICAL_SECTION& sec):m_sec(&sec)
	{
		EnterCriticalSection(m_sec);
	}
	~CScopeLock()
	{
		LeaveCriticalSection(m_sec);
	}
private:
	CRITICAL_SECTION*	m_sec;
};

typedef CScopeLock<CRITICAL_SECTION> CCSScopeLock;

//mutex
template<>
class CScopeLock<HANDLE>
{
public:
	CScopeLock(HANDLE mutex):m_mutex(mutex)
	{
		WaitForSingleObject(m_mutex,INFINITE);
	}
	~CScopeLock()
	{
		ReleaseMutex(m_mutex);
	}
private:
	HANDLE	m_mutex;
};

typedef CScopeLock<HANDLE> CMutexScopeLock;

class CSHTrace
{
public:
	CSHTrace(void);
	virtual ~CSHTrace(void);
	virtual bool IsValid()const;
protected:
	static set<CSHTrace*>				m_objset;
	static auto_ptr<CSHCriticalSection>	m_objsec;
};

template<typename T>
class CSHSingle
{
public:
	virtual ~CSHSingle(){};
protected:
	CSHSingle(){};
	CSHSingle(const CSHSingle&);
	CSHSingle& operator=(const CSHSingle&);
public:
	static T& Instance()
	{
		static T _Instance;
		return _Instance;
	}
};

#define DECLARE_SH_SINGLE(classname) \
	private:	\
		classname(); \
		classname(const classname&); \
		classname& operator=(const classname&); \
	public: \
		static classname& Instance(){ static classname _Instance; return _Instance;}

#define DECLARE_SH_SINGLE_SIMPLE(classname) \
	private:	\
		classname(){}; \
		classname(const classname&); \
		classname& operator=(const classname&); \
	public: \
		static classname& Instance(){ static classname _Instance; return _Instance;}