#include "Thread.h"
#include <process.h>


CThread::CThread(void)
: m_handle(NULL),
m_bEnd(false),
m_id(0)
{
}

CThread::~CThread(void)
{
}

bool CThread::BeginThread(t_start_address startAddress, 
						  unsigned int stack_size /* = 0 */, 
						  void* arglist /* = NULL */, 
						  enumThreadInit initflag /* = ThreadInit_Normal */)
{
	int iInitflag = 0;
	if(initflag == ThreadInit_Suspend)
		iInitflag = CREATE_SUSPENDED;
	m_bEnd = false;
	m_handle = (HANDLE)_beginthreadex(NULL, stack_size, startAddress, arglist, iInitflag, &m_id);

	if(m_handle)
	{
		return true;
	}

	return false;
}

bool CThread::ResumeThread()
{
	return ::ResumeThread(m_handle) != 0;
}

bool CThread::EndThread()
{
	if(!m_handle)	return false;
	m_bEnd = true;
	return true;
}

bool CThread::WaitEndThread()
{
	if(!m_handle)	return false;
	m_bEnd = true;
	::WaitForSingleObject(m_handle, -1);
	::CloseHandle(m_handle);
	m_handle = NULL;
	return true;
}