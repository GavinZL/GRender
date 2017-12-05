/************************************************************************/
/* 获取系统时间 
* 
*/
/************************************************************************/


#include "Macors.h"

#if WIN32
#include <WinSock2.h>
#endif

NAMESPACE_BEGIN

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

struct timeval {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};

int getTimeOfDay(struct timeval * val, struct timezone *)
{
	if (val){
		LARGE_INTEGER liTime, liFreq;

		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liTime);

		val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
		val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
	}
	return 0;
}


NAMESPACE_END