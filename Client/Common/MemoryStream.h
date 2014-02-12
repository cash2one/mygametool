#pragma once
#include "stream.h"

class MemoryStream :
	public Stream
{
public:
	MemoryStream(void);
	virtual ~MemoryStream(void);

	virtual bool CanRead() const { return true; }
	virtual bool CanWrite() const { return true; }
	virtual bool CanSeek() const { return true; }
	virtual bool CanBeMapped() const { return false; }
	
	virtual Size GetSize()  { return size; }
	virtual void SetSize(Size size);
	virtual Position GetPosition() const { return position; }

	virtual Size Read(void* ptr, Size numBytes);
	virtual void Write(const void* ptr, Size numBytes);
	void* GetRawPointer();

	virtual void Seek(Offset offset, SeekOrigin origin);

	void WriteToFile(const char* filename);
	void Clear();
	
	unsigned long GetCRC32();
private:
	//re-allocate the buffer
	void Realloc(Size newsize);
	bool HasRoom(Size numBytes);
	void MakeRoom(Size numBytes);

protected:
	Size capacity;
	Size size;
	Position position;
	unsigned char* buffer;
};
