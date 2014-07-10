#pragma once


namespace SohuTool
{
	template<int v>
	class IntToType
	{
	public:
		enum {value = v};
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

	//��ʱ���ڵ���ʱ bCreateNew Ϊ true������ true �� false������
	template<typename T,bool bCreateNew = false>
	class SingletonImpl
	{
	public:
		virtual ~SingletonImpl()
		{
			_pInstance = NULL;
		}
	protected:
		SingletonImpl(){};
	private:
		SingletonImpl& operator=(const SingletonImpl&);
		SingletonImpl(const SingletonImpl&);	
	public:
		static T& Instance()
		{
			return Instance(IntToType<(int)bCreateNew>());
		}
		static bool Existed()
		{
			return _pInstance != NULL;
		}
	protected:
		static T& Instance(const IntToType<0>&)
		{
			if(_pInstance == NULL)
			{
				static T t;
				if(_pInstance == NULL)
				{
					_pInstance = &t;
				}
			}
			return *(T*)_pInstance;
		}
		static T& Instance(const IntToType<1>&)
		{
			if(_pInstance == NULL)
			{
				_pInstance = new T();
			}
			return *(T*)_pInstance;
		}
		static volatile T* _pInstance;
	};
	template<typename T,bool bCreateNew>
	volatile T* SingletonImpl<T,bCreateNew>::_pInstance = NULL;

}