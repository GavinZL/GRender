/************************************************************************/
/* 获取系统时间 
* 
*/
/************************************************************************/

#ifndef  __TIMEUTILS_H__
#define  __TIMEUTILS_H__


#include "Macors.h"

NAMESPACE_BEGIN

extern int getTimeOfDay(struct timeval *, struct timezone *);

NAMESPACE_END



#endif