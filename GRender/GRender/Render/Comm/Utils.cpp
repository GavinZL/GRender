/************************************************************************/
/* ͨ�ú��� 
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

	va_list vArgList;									//����һ��va_list�͵ı���,���������ָ�������ָ��.
	va_start(vArgList, format);							//��va_start���ʼ������,�����ĵڶ��������ǵ�һ���ɱ������ǰһ������,��һ���̶��Ĳ���
	i = _vsnprintf(str_tmp, 1024, format, vArgList);	//ע��,��Ҫ©��ǰ���_
	va_end(vArgList);									//��va_end������ɱ�����Ļ�ȡ
		
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