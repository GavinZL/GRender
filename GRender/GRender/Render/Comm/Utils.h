/************************************************************************/
/* 通用函数 
 *
 */
/************************************************************************/

#ifndef  __UTILS_H__
#define  __UTILS_H__

#include <string>

NAMESPACE_BEGIN


/**
 * 转换为字符串
 */
template<typename T>
extern std::string toString(T arg);

/**
 * 字符串格式化
 */
extern std::string format(const char* format, ...);

/**
 * log输出
 */
extern void log(const char* format, ...);

NAMESPACE_END


#endif