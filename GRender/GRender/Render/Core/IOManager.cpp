
#include "IOManager.h"

#include "assimp/Importer.hpp"
#include "assimp/Exporter.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "../Comm/Utils.h"

USING_NAMESPACE_G;

IOManager* IOManager::getInstance()
{
	static IOManager _instance;
	return &_instance;
}

void IOManager::loadModelFromFile(const std::string& filepath,
	std::vector<Vec3>& vertics, std::vector<Vec3>& normals,
	std::vector<Vec4>& colors, std::vector<Vec2>& textures,
	std::vector<unsigned int>& indics, std::string& texPath)
{
	if (filepath.empty()){
		G::log("Warning : IOManager::loadModelFromFile () filepath is empty !");
		return;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, /*aiProcessPreset_TargetRealtime_Fast*/    
		aiProcess_Triangulate |	aiProcess_GenUVCoords  | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (nullptr == scene){
		G::log("Error : %s -> %s \n", filepath.c_str(), importer.GetErrorString());
		return;
	}

	if (scene->HasTextures()){
		G::log("Support for meshes with embedded textures is not implemented.");
		return;
	}

	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	for (unsigned int i = 0, cnt = scene->mNumMeshes; i < cnt; ++i){
		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += scene->mMeshes[i]->mNumFaces * 3;
	}

	vertics.resize(numVertices);
	colors.resize(numVertices);
	normals.resize(numVertices);
	textures.resize(numVertices);
	indics.resize(numIndices);

	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	const aiColor4D zeroColor(1.0f, 1.0f, 1.0f, 1.0f);
	int vCnt = 0;
	int iCnt = 0;
	for (unsigned int i = 0, cnt = scene->mNumMeshes; i < cnt; ++i){
		
		aiMesh *mesh = scene->mMeshes[i];
		for (unsigned int j = 0, jcnt = mesh->mNumVertices; j < jcnt; ++j){
			// vertex
			const aiVector3D* pos = &(mesh->mVertices[j]);
			vertics[vCnt + j] = Vec3(pos->x, pos->y, pos->z);
			// color
			const aiColor4D* col = mesh->HasVertexColors(0) ? &(mesh->mColors[0][j]) : &zeroColor;
			colors[vCnt + j] = Vec4(col->r, col->g, col->b, col->a);

			// texture
			if (mesh->HasTextureCoords(0)){
				const aiVector3D* tex = &(mesh->mTextureCoords[0][j]);
				textures[vCnt + j] = Vec2(tex->x, tex->y);
			}
			// normals
			if (mesh->HasNormals()){
				const aiVector3D* nor = &(mesh->mNormals[j]);
				normals[vCnt + j] = Vec3(nor->x, nor->y, nor->z);
			}
		}

		vCnt += mesh->mNumVertices;

		// index
		for (unsigned int k = 0, kcnt = mesh->mNumFaces; k < kcnt; ++k){
			
			const aiFace& face = mesh->mFaces[k];
			if (face.mNumIndices != 3){
				G::log("Warning : unsupported number of indices per face(%d)", face.mNumIndices);
				break;
			}

			indics[iCnt + k * 3 + 0] = face.mIndices[0];
			indics[iCnt + k * 3 + 1] = face.mIndices[1];
			indics[iCnt + k * 3 + 2] = face.mIndices[2];
		}

		iCnt += mesh->mNumFaces * 3;

		// texture
		aiMaterial* texture = scene->mMaterials[mesh->mMaterialIndex];
		std::string texturePath;
		std::string textureName;
		bool hasTexture = false;
		if (nullptr != texture){
			std::string dir;
			dir = filepath.substr(0, filepath.find_last_of('/'));

			aiString path;
			if (texture->GetTextureCount(aiTextureType_DIFFUSE) > 0){
				if (texture->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS){
					texturePath = dir + "/" + path.data;
					textureName = path.data;
					hasTexture = true;
				}
			}
			else if (texture->GetTextureCount(aiTextureType_OPACITY) > 0){
				if (texture->GetTexture(aiTextureType_OPACITY, 0, &path) == AI_SUCCESS){
					texturePath = dir + "/" + path.data;
					textureName = path.data;
					hasTexture = true;
				}
			}
		}

		if (hasTexture){
			texPath = texturePath;
		}
		else{
			texPath = "";
			textures.clear();
		}
	}
}