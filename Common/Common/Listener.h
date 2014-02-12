#pragma once

#include <vector>
#include <algorithm>

/*****************************事件监听器类*******************************
**
**事件监听器的本质其实就是在合适的时机调用已经注册的函数。
比如一个定时器程序每隔一段时间调用一个事件（即函数）,并且随时可以添加事件，
这个就需要监听事件了。

1.监听者Sender(即事件发送者)：就是上面的定时器程序了---Sender只是作为参数传给调用函数的
2.事件(Event): 即函数了
因此函数的监听就是函数的调用过程了： FireEvent(Sender*, ....)
************************************************************************/


//无类型的事件发送者
class CNullSender;

/////////////////////////////定义0个事件参数的listener//////////////////////////////
template<class tSender>
class TBaseListener0Args
{
public:
	virtual ~TBaseListener0Args(){}

	//调用事件响应函数[第一个参数 发送者，后面带0个参数]
	virtual void FireEvent(tSender* sender) = 0;     
};

//下面根据事件函数的不同类型有不同的类【GlobalFunctions, MemberFunctions, ...】

//GlobalFunctions
template<class tSender, typename tRet>
class TBaseListener0ArgsGF : public TBaseListener0Args<tSender>
{
	typedef tRet (*FUNC)(tSender*);
public:
	TBaseListener0ArgsGF(FUNC func)
		:m_pFunc(func)
	{
	}

	void FireEvent(tSender* sender)
	{
		if(m_pFunc)	m_pFunc(sender);
	}	
protected:
	FUNC	m_pFunc;
};

//MemberFunctions
template<class tSender, class tCallee, typename tRet>
class TBaseListener0ArgsMF : public TBaseListener0Args<tSender>
{
	typedef tRet (tCallee::*FUNC)(tSender*);
public:
	TBaseListener0ArgsMF(tCallee* pCalller, FUNC func)
		:m_pCaller(pCalller), m_pFunc(func)
	{}

	void FireEvent(tSender* sender)
	{
		if(m_pCaller && m_pFunc)
		{
			(m_pCaller->*m_pFunc)(sender);
		}
	}
protected:
	tCallee*	m_pCaller;
	FUNC		m_pFunc;
};


//创建一般函数监听器
template<class tSender, typename tRet>
TBaseListener0ArgsGF<tSender, tRet>* make_listener(tRet (*func)(tSender*))
{
	return new TBaseListener0ArgsGF<tSender, tRet>(func);
}

//创建类函数监听器
template<class tSender, class tCallee, typename tRet>
TBaseListener0ArgsMF<tSender, tCallee, tRet>* make_listener(tCallee* pCaller, tRet (tCallee::*func)(tSender*))
{
	return new TBaseListener0ArgsMF<tSender, tCallee, tRet>(pCaller, func);
}

//现在可以使用了

namespace test1
{
	class CTrigger
	{
	public:
		typedef TBaseListener0Args<CTrigger> TriggerListener;
	public:
		void FireEvent(){ if(m_pListener) m_pListener->FireEvent(this); }
		void SetListener(TriggerListener* listener){ m_pListener = listener; }
	private:
		TriggerListener*	m_pListener;
	};
	//上面的CTrigger在合适的时机调用FireEvent就可以了

	class CCaller
	{
	public:
		void OnCreate()
		{
			//typedef TBaseListener0ArgsMF<CTrigger, CCaller, int> TListener;
			//TListener* pListener = make_listener(this, &CCaller::Test0ArgsFunc);
			//m_trigger.SetListener(pListener);
			m_trigger.SetListener(make_listener(this, &CCaller::Test0ArgsFunc));
		}
		int Test0ArgsFunc(CTrigger* pCaller)
		{
			//Nothing
		}
	private:
		CTrigger	m_trigger;
	};
}

//以上就是监听事件的调用过程，但是有个问题就是 CTrigger 的TriggerListener只能监听一个事件，就是说
//每次只能一个函数调用，为了解决这个还得弄个监听Handler能监听多个事件
template<class tListener, class tChild>
class TEventHandlerBase
{
public:
	TEventHandlerBase() { }
	virtual ~TEventHandlerBase(){ Clear();}
	
	//支持+=
	tChild& operator +=(tListener* listener)
	{
		Add(listener);
		return *this;
	}

	bool Add(tListener* listener)
	{
		std::vector<tListener*>::const_iterator itBegin = m_vecListenerList.begin();
		std::vector<tListener*>::const_iterator itEnd = m_vecListenerList.end();
		if(listener && std::find(itBegin, itEnd, listener)== itEnd)
		{
			m_vecListenerList.push_back(listener);
			return true;
		}
		return false;
	}

	void Clear()
	{
		std::vector<tListener*>::iterator it = m_vecListenerList.begin();
		std::vector<tListener*>::iterator itEnd = m_vecListenerList.end();
		for( ; it != itEnd; ++it)
		{
			tListener* p = *it;
			if(p)
			{
				delete p;
				p = 0;
			}
		}
		m_vecListenerList.clear();
	}
protected:
	std::vector<tListener*> m_vecListenerList;
};

template<class tSender>
class TEventHandlerBase0Args : 
	public TEventHandlerBase< TBaseListener0Args<tSender>, TEventHandlerBase0Args<tSender> >
								//Listener指针				//+= 的返回类型是自己
{
public:
	void Notify(tSender* pSender /*0参数*/)
	{
		typedef TBaseListener0Args<tSender> tListener;
		std::vector<tListener*>::const_iterator it = m_vecListenerList.begin();
		std::vector<tListener*>::const_iterator itEnd = m_vecListenerList.end();
		while( it != itEnd)
		{
			if(*it) (*it)->FireEvent(pSender);
			++it;
		}
	}
};


//好了，有了上面的TEventHandlerBase0Args 我们就可以监听多个TBaseListener0ArgsGF和TBaseListener0ArgsMF
//的事件了	



namespace test2
{


	class CTrigger
	{
		typedef TEventHandlerBase0Args<CTrigger> TriggerListener;
	public:
		void FireEvent(){  m_pListener.Notify(this); }  //注意是Notify
		//void SetListener(TriggerListener* listener){ m_pListener = listener; }
		void AddEvent(TBaseListener0Args<CTrigger>* p)
		{
			m_pListener.Add(p);
		}
	private:
		TriggerListener	m_pListener;
	};
	//上面的CTrigger在合适的时机调用FireEvent就可以了

	double woyaojianting(CTrigger* pSender){  return 0.0; }
	float anoter(CTrigger* pSender){  return 0.0f; }

	class CCaller2
	{
	public:
		void OnCreate()
		{
			typedef TBaseListener0Args<CTrigger> BaseTrigger;
			m_trigger.AddEvent(make_listener(this, &CCaller2::Test0ArgsFunc));
			m_trigger.AddEvent(make_listener(&woyaojianting));
			m_trigger.AddEvent(make_listener(&anoter));
		}
		int Test0ArgsFunc(CTrigger* pCaller)
		{
			//Nothing
		}
	private:
		CTrigger	m_trigger;
	};
}

//至此 0 参数的 监听器就差不多了，但是还有几个小问题
//1. 对于类的 tCallee* 者，如果指针注册时指针非 NULL ，但是FireEvent时为NULL 即为野指针，肯定会出错。所以要采取措施解决
//2. 目前只是对于0参数的，但是对于有1、2、3、4.。。的参数的必须要重新写这么多代码，所以也不爽 代码也难看懂
	//所以可以用一个 Event直接包括所有的参数 ok