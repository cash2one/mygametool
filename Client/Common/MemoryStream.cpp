#include "precompile.h"
#include "MemoryStream.h"
#include "../zlib/zlib.h"
#include "crc.h"

#define n_assert void;

MemoryStream::MemoryStream(void)
: capacity(0),
  size(0),
  buffer(0),
  position(0)
{
}

MemoryStream::~MemoryStream(void)
{
	if(buffer)
	{
		delete[] buffer;
		buffer = NULL;
	}
}

void MemoryStream::SetSize(Size size)
{
	if(size > this->capacity)
	{
		Realloc(size);
	}
	this->size = size;
}

//read numBytes to ptr
Stream::Size MemoryStream::Read(void* ptr, Size numBytes)
{
	n_assert( (ReadAccess == m_eAccessMode) || (ReadAndWriteAccess == m_eAccessMode));
	n_assert( (this->position >= 0) && (this->position <= this->size));

	Size leftSize = this->size - this->position;
	Size readsize = numBytes > leftSize ? leftSize : numBytes;

	if(readsize > 0)
	{
		memcpy(ptr, buffer+this->position, readsize);
		this->position = this->position + readsize;
		return readsize;
	}
	return 0;
}

//write data to this
void MemoryStream::Write(const void* ptr, Size numBytes)
{
	//AppendAccess只对FileStream有效
	n_assert( (WriteAccess == m_eAccessMode) || (ReadAndWriteAccess == m_eAccessMode)
		|| ( AppendAccess == m_eAccessMode));
	n_assert( (this->position >= 0) && (this->position <= this->size));
	
	if(!HasRoom(numBytes))
	{
		MakeRoom(numBytes);
	}

	memcpy(buffer+this->position, ptr, numBytes);
	this->position += numBytes;
	if(this->position > this->size)
	{
		this->size = this->position;
	}
}

void MemoryStream::Realloc(Size newsize)
{
	//Realloc：将原来的内存释放掉，在新分配一段内存
	unsigned char* pNewBuffer = new unsigned char[newsize];
	unsigned char* pEndBuffer = pNewBuffer + newsize;
	n_assert(pNewBuffer);

	//真正需要移动的数据
	Size realsize = newsize > this->size ? this->size : newsize; 
	memcpy(pNewBuffer, buffer, realsize);
	delete[] buffer;
	buffer = pNewBuffer;

	this->capacity = newsize;
	this->size = realsize;
	if(this->position > realsize)
	{
		this->position = realsize;
	}
}

bool MemoryStream::HasRoom(Size numBytes)
{
	return this->size + numBytes <= this->capacity;
}

void MemoryStream::MakeRoom(Size numBytes)
{
	n_assert(numBytes > 0);
	n_assert( this->size + numBytes > this->capacity);

	Size oneDotFiveCurrentSize = this->capacity + (this->capacity >> 1);
	Size newCapacity = this->size + numBytes;
	if(oneDotFiveCurrentSize > newCapacity)
	{
		newCapacity = oneDotFiveCurrentSize;
	}
	if(newCapacity < 16)
	{
		newCapacity = 16;
	}

	n_assert( newCapacity > this->capacity);
	this->Realloc(newCapacity);
}

void* MemoryStream::GetRawPointer()
{
	n_assert( this->buffer);
	return this->buffer;
}

void MemoryStream::WriteToFile(const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if(file)
	{
		fwrite(buffer, size, 1, file);
		fclose(file);
	}

}

void MemoryStream::Seek(Offset offset, SeekOrigin origin)
{
	if(origin == SeekBegin)
		this->position = offset;
	else if(origin == SeekCurrent)
		this->position = this->position + offset;
	else if(origin  == SeekEnd)
		this->position = this->size + offset;
	
}

void MemoryStream::Clear()
{
	ZeroMemory(buffer, size);
	this->position = 0;
}

unsigned long MemoryStream::GetCRC32()
{
	Position pos = this->GetPosition();

	this->Seek(0, SeekBegin);
	unsigned long crc = GenerateCrc32Stream(this, this->size);
	this->Seek(pos, SeekBegin);

	return crc;
}