#include "GL/glew.h"
#include "Picker.h"
#include "Renderer.h"
#include "Engine.h"
#include "Scene.h"
#include "MeshNode.h"
#include "Mesh.h"
#include "Camera.h"
#include "../Comm/Utils.h"

USING_NAMESPACE_G

Picker* Picker::create(int w, int h)
{
	static Picker _instance;
	_instance.resize(w, h);
	return &_instance;
}

Picker::Picker()
	: Node()
	, m_isRectDirty(false)
{
	m_nodeFlagMask = _PICKER;

	memset(m_winSize, 0, sizeof(int) * 2);
	memset(m_oriPos, 0, sizeof(int) * 2);
	memset(m_curPos, 0, sizeof(int) * 2);
	memset(m_rectangleV, 0, sizeof(float) * 12);
}

Picker::~Picker()
{

}

void Picker::resize(int w, int h)
{
	m_winSize[0] = w;
	m_winSize[1] = h;
}

void Picker::draw(Renderer* renderer, const Mat4& transform, unsigned int flags)
{
	if (m_isRectDirty){
		m_customCommand.init(1);
		m_customCommand.func = G_CALLBACK_0(Picker::renderRectangle, this, transform, flags);
		renderer->addCommand(&m_customCommand);

		//m_isRectDirty = false;
	}
}

void Picker::setRectangle(const Vec2& origin, const Vec2& dest)
{
	m_oriPos[0] = origin[0];
	m_oriPos[1] = origin[1];

	m_curPos[0] = dest[0];
	m_curPos[1] = dest[1];

	m_rectangleV[0] = (2 * m_oriPos[0] - m_winSize[0]) / float(m_winSize[0]);
	m_rectangleV[1] = (2 * (m_winSize[1] - m_oriPos[1]) - m_winSize[1]) / float(m_winSize[1]);
	m_rectangleV[2] = 0;
	m_rectangleV[3] = (2 * m_curPos[0] - m_winSize[0]) / float(m_winSize[0]);
	m_rectangleV[4] = (2 * (m_winSize[1] - m_oriPos[1]) - m_winSize[1]) / float(m_winSize[1]);
	m_rectangleV[5] = 0;

	m_rectangleV[6] = (2 * m_curPos[0] - m_winSize[0]) / float(m_winSize[0]);
	m_rectangleV[7] = (2 * (m_winSize[1] - m_curPos[1]) - m_winSize[1]) / float(m_winSize[1]);
	m_rectangleV[8] = 0;

	m_rectangleV[9] = (2 * m_oriPos[0] - m_winSize[0]) / float(m_winSize[0]);
	m_rectangleV[10] = (2 * (m_winSize[1] - m_curPos[1]) - m_winSize[1]) / float(m_winSize[1]);
	m_rectangleV[11] = 0;

	m_isRectDirty = true;
}

void Picker::renderRectangle(const Mat4& transform, unsigned int flags)
{
	glUseProgram(0);

	glDisable(GL_LIGHTING);
	glPushMatrix();
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -10, 10);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glViewport(0, 0, m_winSize[0], m_winSize[1]);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1.0f, 0.0f, 0.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(m_rectangleV[0], m_rectangleV[1], m_rectangleV[2]);
	glVertex3f(m_rectangleV[3], m_rectangleV[4], m_rectangleV[5]);
	glVertex3f(m_rectangleV[6], m_rectangleV[7], m_rectangleV[8]);
	glVertex3f(m_rectangleV[9], m_rectangleV[10], m_rectangleV[11]);
	glEnd();

	//glRectf(m_rectangleV[0],m_rectangleV[1], m_rectangleV[6], m_rectangleV[7]);
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
}

//  [12/12/2017 Administrator]

void Picker::pickedRectangle(unsigned int nodeFlag)
{
	auto camera = Engine::getInstance()->getRunningScene()->getDefaultCamera();
	if (camera == nullptr){
		return;
	}

	G::Mat4 mat = camera->getViewProjectionMatrix();

	// pick regin
	int x = (m_oriPos[0] + m_curPos[0]) * 0.5f;
	int y = m_winSize[1] - (m_oriPos[1] + m_curPos[1]) * 0.5f; // height - y

	int w = std::abs(m_oriPos[0] - m_curPos[0]);
	int h = std::abs(m_oriPos[1] - m_curPos[1]);

	// 清理掉之前拾取的节点
	m_pickedNodes.clear();

	// visit nodes
	std::vector<Node*> outs;
	Engine::getInstance()->getRunningScene()->visit(Engine::getInstance()->getRunningScene(), outs, _MESH);

	if (outs.size() == 0){
		return;
	}

	// indics
	std::vector<unsigned int> indics;

	glPushMatrix();
	glMultMatrixf(mat.data());

	// select mode render
	long hits = 0;
	int sz = 0;

	// size count
	for (auto it = outs.begin(); it != outs.end(); ++it){
		MeshNode* mNode = dynamic_cast<MeshNode*>(*it);
		if (mNode == nullptr){
			continue;
		}

		sz += mNode->getMesh()->getVertexCount();
	}

	// ??
	sz *= 4;

	// select render
	GLuint *selectBuffer = new GLuint[sz];
	glSelectBuffer(sz, selectBuffer);

	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(-1);

	double mp[16] = { 0 };
	GLint viewport[4] = { 0 };

	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glGetDoublev(GL_PROJECTION_MATRIX, mp);

	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix(x, y, w, h, viewport);
	glMultMatrixd(mp);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// render points
	int vcnt = 0;
	for (auto it = outs.begin(); it != outs.end(); ++it){
		MeshNode* mNode = dynamic_cast<MeshNode*>(*it);
		if (mNode){
			Mesh* mesh = mNode->getMesh();
			if (mesh){
				for (int i = 0, cnt = mesh->getVertexCount(); i < cnt; ++i){
					glLoadName(vcnt);
					glBegin(GL_POINTS);
					glVertex3fv(mesh->getVertics()[i].data());
					glEnd();

					vcnt++;
				}
			}
		}
	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// return
	hits = glRenderMode(GL_RENDER);

	for (long ii = 0; ii < hits; ++ii){
		indics.push_back(selectBuffer[ii * 4 + 3]);
	}

	delete[]selectBuffer;

	glPopMatrix();

	G::log("---------------------------------\n");
	G::log("%d\n", indics.size());
	

	// ## to do mesh data...
	modifyMeshColor(outs, indics);


	// 拷贝拾取的节点用于存储
	m_pickedNodes.resize(outs.size());
	std::copy(outs.begin(), outs.end(), m_pickedNodes.begin());
}

void Picker::modifyMeshColor(std::vector<Node*>& nodes, std::vector<unsigned int>& indics)
{
	if (indics.size() == 0 || nodes.size() == 0){
		return;
	}

	// 1. 修改mesh颜色
	for (int i = 0, cnt = indics.size(); i < cnt; ++i){

		MeshNode* mNode = dynamic_cast<MeshNode*>(nodes[0]);
		if (mNode == nullptr || mNode->getMesh() == nullptr){
			break;
		}

		Mesh* mesh = mNode->getMesh();

		// ##表示在第一个模型上
		if (indics[i] < mesh->getVertexCount()){
			mesh->changeColor(indics[i]);
		}
		else{
			int sz = mesh->getVertexCount();
			int osz = sz;

			for (int n = 1, ncnt = nodes.size(); n < ncnt; ++n){
				MeshNode* nNode = dynamic_cast<MeshNode*>(nodes[n]);
				if (nNode == nullptr || nNode->getMesh() == nullptr){
					break;
				}

				Mesh *nMesh = nNode->getMesh();

				sz += nMesh->getVertexCount();
				if (indics[i] < sz){
					nMesh->changeColor(indics[i] - osz);
				}
				osz = sz;
			}
		}
	}

	//2. 刷新buffer
	for (int n = 0, ncnt = nodes.size(); n < ncnt; ++n){
		MeshNode* nNode = dynamic_cast<MeshNode*>(nodes[n]);
		if (nNode == nullptr || nNode->getMesh() == nullptr){
			break;
		}

		Mesh *nMesh = nNode->getMesh();
		nMesh->flashColor();
	}

}

void Picker::deleteVertics()
{
	for (int n = 0, ncnt = m_pickedNodes.size(); n < ncnt; ++n){
		MeshNode* nNode = dynamic_cast<MeshNode*>(m_pickedNodes[n]);
		if (nNode == nullptr || nNode->getMesh() == nullptr){
			break;
		}

		Mesh *nMesh = nNode->getMesh();
		nMesh->deleteVertics();
	}
}

void Picker::restoreVertices()
{
	for (int n = 0, ncnt = m_pickedNodes.size(); n < ncnt; ++n){
		MeshNode* nNode = dynamic_cast<MeshNode*>(m_pickedNodes[n]);
		if (nNode == nullptr || nNode->getMesh() == nullptr){
			break;
		}

		Mesh *nMesh = nNode->getMesh();
		nMesh->resetVertics();
	}
}

void Picker::restoreColor()
{
	for (int n = 0, ncnt = m_pickedNodes.size(); n < ncnt; ++n){
		MeshNode* nNode = dynamic_cast<MeshNode*>(m_pickedNodes[n]);
		if (nNode == nullptr || nNode->getMesh() == nullptr){
			break;
		}

		Mesh *nMesh = nNode->getMesh();
		nMesh->resetColor();
	}
}