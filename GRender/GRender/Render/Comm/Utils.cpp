/************************************************************************/
/* 通用函数 
 *
 */
/************************************************************************/
#include <string>
#include <cstdarg>
#include <cstdio>
#include "Macors.h"

NAMESPACE_BEGIN

template<typename T>
std::string toString(T arg)
{
	std::stringstream ss;
	ss << arg;
	return ss.str();
}

std::string format(const char* format, ...)
{
	char str_tmp[1024] = { 0 };	
	int i = 0;

	va_list vArgList;									//定义一个va_list型的变量,这个变量是指向参数的指针.
	va_start(vArgList, format);							//用va_start宏初始化变量,这个宏的第二个参数是第一个可变参数的前一个参数,是一个固定的参数
	i = _vsnprintf(str_tmp, 1024, format, vArgList);	//注意,不要漏掉前面的_
	va_end(vArgList);									//用va_end宏结束可变参数的获取
		
	return std::string(str_tmp);
}

void log(const char* format, ...)
{
	char str_tmp[1024] = { 0 };
	int i = 0;

	va_list vArgList;
	va_start(vArgList, format);
	i = _vsnprintf(str_tmp, 1024, format, vArgList);
	va_end(vArgList);

	printf(str_tmp);
}

NAMESPACE_END