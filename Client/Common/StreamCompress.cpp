#include "precompile.h"
#include "StreamCompress.h"
#include "../zlib/zlib.h"

#ifdef _DEBUG
	#pragma comment(lib, "../lib/zlib.lib")
#else
	#pragma comment(lib, "zlib.lib")
#endif


int StreamCompress::uncompressStream(Stream& i_stream, Stream& o_stream, size_t size /* = -1 */)
{
	const unsigned long  c_inlen = (1 << 12);
	const unsigned long c_outlen = (c_inlen *40);
	static unsigned char in_data[c_inlen];
	static unsigned char out_data[c_outlen];

	z_stream_s zipStream;
	zipStream.zalloc = Z_NULL;
	zipStream.zfree = Z_NULL;
	zipStream.opaque = Z_NULL;

	zipStream.next_out = NULL;
	zipStream.avail_out = 0;
	zipStream.total_out = 0;

	zipStream.next_in = NULL;
	zipStream.avail_in = 0;
	zipStream.total_in = 0;

	size_t inLen = ( size == -1) ? i_stream.GetSize() : size;

	int err = inflateInit2(&zipStream, -MAX_WBITS);
	if(err != Z_OK)	return err;

	while(inLen)
	{
		size_t readlen;
		if(inLen > c_inlen)
			readlen = c_inlen;
		else
			readlen = inLen;
		inLen -= readlen;

		i_stream.Read(in_data, readlen);
		
		zipStream.next_in = (unsigned char*)in_data;
		zipStream.total_in = 0;
		zipStream.avail_in =readlen;

		while(zipStream.avail_in)
		{
			do 
			{
				zipStream.next_out = out_data;
				zipStream.avail_out = c_outlen;
				zipStream.total_out = 0;

				err = inflate(&zipStream, Z_SYNC_FLUSH);
				if(err != Z_OK && err != Z_STREAM_END)
					return err;

				o_stream.Write(&out_data, zipStream.total_out);

				if(err == Z_STREAM_END)
					break;
			} while (zipStream.avail_out == 0);

			if(err == Z_STREAM_END)
				break;

		}

		if(err != Z_OK && err != Z_STREAM_END)
			return err;

		if(err == Z_STREAM_END)
			break;
	}

	err = inflateEnd(&zipStream);
	return err;
}

int StreamCompress::compress_data(unsigned char* pOut, unsigned long* pOutlen, unsigned char* pIn, unsigned long nInLen, int level)
{
	static z_stream_s zipStream;
	zipStream.zalloc = Z_NULL;
	zipStream.zfree = Z_NULL;
	zipStream.opaque = Z_NULL;

	zipStream.next_in = pIn;
	zipStream.avail_in = nInLen;
	zipStream.total_in = 0;
	zipStream.next_out = pOut;
	zipStream.avail_out = *pOutlen;
	zipStream.total_out = 0;

	int err = deflateInit2(&zipStream, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	if(err != Z_OK)		return err;
	err = deflate(&zipStream, Z_FINISH);
	if(err != Z_STREAM_END)
	{
		deflateEnd(&zipStream);
		return err == Z_OK ? Z_BUF_ERROR : err;
	}
	*pOutlen = zipStream.total_out;
	err = deflateEnd(&zipStream);

	return err;
}

//Ω‚—πÀı
 int StreamCompress::uncompress_data(unsigned char* pOut, unsigned long* pOutLen, unsigned char* pIn, unsigned long nInLen)
{
	z_stream_s zipStream;
	ZeroMemory(&zipStream, sizeof(zipStream));
	zipStream.next_in = (BYTE*)pIn;
	zipStream.avail_in = nInLen;
	zipStream.next_out = pOut;
	zipStream.avail_out = *pOutLen;
	zipStream.opaque = Z_NULL;

	int err = inflateInit2(&zipStream, -MAX_WBITS);
	if(err != Z_OK)
		return err;

	err = inflate(&zipStream, Z_FINISH);
	assert(err == Z_OK || err == Z_STREAM_END);

	err = inflateEnd(&zipStream);

	return err;
}