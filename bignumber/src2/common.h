#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <string>
#include <assert.h>

//#define max(a,b) ((a)>(b)?(a):(b))
//#define min(a,b) ((a)<(b)?(a):(b))
#define ERR_GENERIC -1
#define ERR_VERIFY -2
#define uchar unsigned char
#define uint unsigned int

#ifdef FORCE_USE_EXCEPTION
#ifdef assert
#undef assert
#endif
#define USE_EXCEPTION
#else
#ifndef assert
#define USE_EXCEPTION
#endif
#endif

#define EXCEPTION(E,C) if(!(E))throw AgError(__FILE__,__LINE__,(C))

#ifdef USE_EXCEPTION
#define assert(E) EXCEPTION(E,ERR_GENERIC)
#endif

#define vassert(E) EXCEPTION(E,ERR_VERIFY)

struct AgError
{
	char* file;
	int line;
	int id;
	AgError(char* _file,int _line,int _id)
	{
		file=_file,line=_line,id=_id;
	}
};

#endif