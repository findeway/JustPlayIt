/**
 * @file   SingletonImpl.h
 * @author findeway <findewayz@gmail.com>
 * @date   Sat Apr 26 16:59:06 2014
 * 
 * @brief  单例模式模板实现的线程安全版本
 * 
 * 
 */
#ifndef _SINGLETONIMPL_H_
#define _SINGLETONIMPL_H_

#ifdef _WIN32
	#include <atlbase.h>
#else
	#include <pthread.h>
#endif

template <typename T>
class SingletonImpl
{
public:
    virtual ~SingletonImpl()
	{
		_pInstance = NULL;
	}
    static T* Instance()
    {
		/// DCL 提高性能
		if(_pInstance == NULL)
		{
#ifdef _WIN32
			m_criticalSection.Lock();
#else
			pthread_mutex_lock(&g_mutex);
#endif

			if(_pInstance == NULL)
			{
				_pInstance = new T();
			}

#ifdef _WIN32
			m_criticalSection.Unlock();
#else
			pthread_mutex_unlock(&g_mutex);
#endif
		}
		return _pInstance;
    }
protected:
    SingletonImpl(){};
private:
/// 注意volatile,防止多线程环境获取到未初始化完全的实例.
    static T* volatile _pInstance;

#ifdef _WIN32
	static ATL::CComAutoCriticalSection m_criticalSection;
#else
    //static pthread_mutex_t g_mutex;
#endif

};

template <typename T>
T* volatile SingletonImpl<T>::_pInstance = NULL;

#ifndef _WIN32
template <typename T>
pthread_mutex_t SingletonImpl<T>::g_mutex = PTHREAD_MUTEX_INITIALIZER;
#else
template <typename T>
ATL::CComAutoCriticalSection SingletonImpl<T>::m_criticalSection;
#endif

#endif /* _SINGLETONIMPL_H_ */



















