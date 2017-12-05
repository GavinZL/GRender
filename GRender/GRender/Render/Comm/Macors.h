/************************************************************************/
/* 宏定义文件 
 * 
 */
/************************************************************************/

#ifndef  __MACROS_H__
#define  __MACROS_H__

#include <functional>

/**
 * @ 命名空间宏定义
 */

#define  NAMESPACE_BEGIN	namespace G {
#define  NAMESPACE_END		}

#define  USING_NAMESPACE_G	using namespace G;


// new callbacks based on C++11
#define G_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define G_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define G_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define G_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)


#endif