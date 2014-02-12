#pragma once

class Stream
{
public:
	//typedefs
	typedef int Position;
	typedef int Offset;
	typedef int Size;

	//access mode
	enum AccessMode
	{
		ReadAccess,
		WriteAccess,
		ReadAndWriteAccess,
		AppendAccess,
	};

	//access pattern
	enum AccessPattern
	{
		Random,
		Sequential,
	};

	//seek origins
	enum SeekOrigin
	{
		SeekBegin,
		SeekCurrent,
		SeekEnd,
	};

public:

	Stream(void);
	virtual ~Stream(void);

	virtual bool CanRead() const { return true; }
	virtual bool CanWrite() const { return true; }
	virtual bool CanSeek() const { return true; }
	virtual bool CanBeMapped() const { return false; }

	virtual void SetSize(Size size);
	virtual Size GetSize();
	virtual Position GetPosition() const;
	
	void SetAccessMode(AccessMode mode) { m_eAccessMode = mode; }
	AccessMode GetAccessMode() const { return m_eAccessMode; }
	void SetAccessPattern(AccessPattern pattern) { m_eAccessPattern = pattern; }
	AccessPattern GetAccessPatern() const { return m_eAccessPattern; }

	virtual void Write(const void* ptr, Size numBytes);
	virtual Size Read(void* ptr, Size numBytes);
	virtual void Seek(Offset offset, SeekOrigin origin);
	virtual void Flush();
	virtual bool Eof() const;
	virtual void* Map();
	virtual void Unmap();
	bool IsMapped() const { return m_bMapped; }

	virtual void* GetRawPointer(){ return 0;}
	virtual unsigned long GetCRC32() { return 0; }


protected:
	AccessMode			m_eAccessMode;
	AccessPattern		m_eAccessPattern;
	//ÄÚ´æÓ³Éä
	bool				m_bMapped;
};
