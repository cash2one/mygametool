#pragma once


#include <Windows.h>

class CThread
{
public:
	CThread(void);
	~CThread(void);

	typedef unsigned int (__stdcall *t_start_address)( void* );
	
	enum enumThreadInit
	{
		ThreadInit_Normal,
		ThreadInit_Suspend,
	};

	bool BeginThread(t_start_address startAddress,
		unsigned int stack_size = 0,
		void* arglist = NULL,
		enumThreadInit initflag = ThreadInit_Normal);
	bool ResumeThread();
	bool EndThread();
	bool WaitEndThread();
	bool IsEndThread(){ return m_bEnd; }
private:

	HANDLE			m_handle;
	unsigned int	m_id;
	bool			m_bEnd;
};
