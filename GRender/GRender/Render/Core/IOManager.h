/************************************************************************/
/* 
* 文件处理管理类
*/
/************************************************************************/

#ifndef  __IOMANAGER_H__
#define  __IOMANAGER_H__

#include "../Comm/Macors.h"
#include "../Comm/Math.h"


NAMESPACE_BEGIN

class IOManager
{
public:
	static IOManager* getInstance();

	/**load model from file */
	void loadModelFromFile(const std::string& filepath,
		std::vector<Vec3>& vertics, std::vector<Vec3>& normals,
		std::vector<Vec4>& colors, std::vector<Vec2>& textures, 
		std::vector<unsigned int>& indics, std::string& texturePath);



};

NAMESPACE_END

#endif