/************************************************************************/
/* ͨ�ú��� 
 *
 */
/************************************************************************/

#ifndef  __UTILS_H__
#define  __UTILS_H__

#include <string>

NAMESPACE_BEGIN


/**
 * ת��Ϊ�ַ���
 */
template<typename T>
extern std::string toString(T arg);

/**
 * �ַ�����ʽ��
 */
extern std::string format(const char* format, ...);

/**
 * log���
 */
extern void log(const char* format, ...);

NAMESPACE_END


#endif