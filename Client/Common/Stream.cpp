#include "precompile.h"
#include "Stream.h"

Stream::Stream(void)
: m_eAccessMode(ReadAndWriteAccess),
  m_eAccessPattern(Sequential),
  m_bMapped(false)
{
}

Stream::~Stream(void)
{
}

void Stream::SetSize(Size size)
{

}

Stream::Size Stream::GetSize()
{
	return 0;
}

Stream::Position Stream::GetPosition() const
{
	return 0;
}

void Stream::Write(const void* ptr, Size numBytes)
{
}

Stream::Size Stream::Read(void* ptr, Size numBytes)
{
	return 0;
}

void Stream::Seek(Offset offset, SeekOrigin origin)
{

}

void Stream::Flush()
{

}

bool Stream::Eof() const 
{
	return true;
}

void* Stream::Map()
{
	return NULL;
}

void Stream::Unmap()
{

}