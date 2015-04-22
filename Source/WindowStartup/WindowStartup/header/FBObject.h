#ifndef FBOBJECT_H
#define BJOBJECT_H

#include "GPUParticleEmitter.h"


class FBObject
{
public:
	FBObject();
	~FBObject();

	void CreateBuffers();
	int loadShader(unsigned int type, const char* path);
	void CreateDrawShader();
	void Draw(glm::mat4 &ProjectView);

private:

	unsigned int m_drawShader; // our shader 
	unsigned int m_fbo; //frame buffer object handle
	unsigned int m_fboTexture; 
	unsigned int m_fboDepth;
	

	unsigned int m_vao, m_vbo, m_ibo;
};


#endif