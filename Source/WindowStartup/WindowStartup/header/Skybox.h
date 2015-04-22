#ifndef SKYBOX_H
#define SKYBOX_H


#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <gl_core_4_4.h>
#include <iostream>
#include "FlyCamera.h"

using namespace glm;

class Skybox
{
public:
	Skybox();
	~Skybox();


	unsigned int loadCubeMap(std::vector<const char*> faces);
	int loadShader(unsigned int type, const char* path);
	void CreateDrawShaders();
	void CreateBuffers();

	void Draw(FlyCamera *camera);

private:

	mat3 view;
	mat4 viewer;
	
	std::vector<const char*> images;
	unsigned int cubeMapTexture;
	unsigned int m_drawShader, m_vao, m_vbo, m_ibo,m_fbo;
};




#endif