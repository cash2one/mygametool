#pragma once

#include <vector>
#include <algorithm>

/*****************************�¼���������*******************************
**
**�¼��������ı�����ʵ�����ں��ʵ�ʱ�������Ѿ�ע��ĺ�����
����һ����ʱ������ÿ��һ��ʱ�����һ���¼�����������,������ʱ��������¼���
�������Ҫ�����¼��ˡ�

1.������Sender(���¼�������)����������Ķ�ʱ��������---Senderֻ����Ϊ�����������ú�����
2.�¼�(Event): ��������
��˺����ļ������Ǻ����ĵ��ù����ˣ� FireEvent(Sender*, ....)
************************************************************************/


//�����͵��¼�������
class CNullSender;

/////////////////////////////����0���¼�������listener//////////////////////////////
template<class tSender>
class TBaseListener0Args
{
public:
	virtual ~TBaseListener0Args(){}

	//�����¼���Ӧ����[��һ������ �����ߣ������0������]
	virtual void FireEvent(tSender* sender) = 0;     
};

//��������¼������Ĳ�ͬ�����в�ͬ���ࡾGlobalFunctions, MemberFunctions, ...��

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


//����һ�㺯��������
template<class tSender, typename tRet>
TBaseListener0ArgsGF<tSender, tRet>* make_listener(tRet (*func)(tSender*))
{
	return new TBaseListener0ArgsGF<tSender, tRet>(func);
}

//�����ຯ��������
template<class tSender, class tCallee, typename tRet>
TBaseListener0ArgsMF<tSender, tCallee, tRet>* make_listener(tCallee* pCaller, tRet (tCallee::*func)(tSender*))
{
	return new TBaseListener0ArgsMF<tSender, tCallee, tRet>(pCaller, func);
}

//���ڿ���ʹ����

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
	//�����CTrigger�ں��ʵ�ʱ������FireEvent�Ϳ�����

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

//���Ͼ��Ǽ����¼��ĵ��ù��̣������и�������� CTrigger ��TriggerListenerֻ�ܼ���һ���¼�������˵
//ÿ��ֻ��һ���������ã�Ϊ�˽���������Ū������Handler�ܼ�������¼�
template<class tListener, class tChild>
class TEventHandlerBase
{
public:
	TEventHandlerBase() { }
	virtual ~TEventHandlerBase(){ Clear();}
	
	//֧��+=
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
								//Listenerָ��				//+= �ķ����������Լ�
{
public:
	void Notify(tSender* pSender /*0����*/)
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


//���ˣ����������TEventHandlerBase0Args ���ǾͿ��Լ������TBaseListener0ArgsGF��TBaseListener0ArgsMF
//���¼���	



namespace test2
{


	class CTrigger
	{
		typedef TEventHandlerBase0Args<CTrigger> TriggerListener;
	public:
		void FireEvent(){  m_pListener.Notify(this); }  //ע����Notify
		//void SetListener(TriggerListener* listener){ m_pListener = listener; }
		void AddEvent(TBaseListener0Args<CTrigger>* p)
		{
			m_pListener.Add(p);
		}
	private:
		TriggerListener	m_pListener;
	};
	//�����CTrigger�ں��ʵ�ʱ������FireEvent�Ϳ�����

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

//���� 0 ������ �������Ͳ���ˣ����ǻ��м���С����
//1. ������� tCallee* �ߣ����ָ��ע��ʱָ��� NULL ������FireEventʱΪNULL ��ΪҰָ�룬�϶����������Ҫ��ȡ��ʩ���
//2. Ŀǰֻ�Ƕ���0�����ģ����Ƕ�����1��2��3��4.�����Ĳ����ı���Ҫ����д��ô����룬����Ҳ��ˬ ����Ҳ�ѿ���
	//���Կ�����һ�� Eventֱ�Ӱ������еĲ��� ok