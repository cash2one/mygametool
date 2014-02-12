
#pragma once


#define SAFE_DELETE(p) (if(p) { delete (p); (p)=0;})
#define SAFE_RELEASE(p) (if(p) { (p)->Release(); (p)=0;})
#define SAFE_DELETE_ARRAY(p) (if(p){ delete[] (p); ([p)=0;})

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))


//���ļ�·���ķ�б�ܡ�/��ת��Ϊ��\\��
inline void public_backslash(char* str)
{
	while(*str)
	{
		if(*str == '/')
			*str = '\\';
		str++;
	}
}

//���ļ�·���ķ�б�ܡ�\\��ת��Ϊ��/��
inline void public_forwardslash(char* str)
{
	while(*str)
	{
		if(*str == '\\')
			*str = '/';
		str++;
	}
}