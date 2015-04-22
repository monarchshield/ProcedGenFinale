#ifndef SHADOWOBJ_H
#define SHADOWOBJ_H

#include "GPUParticleEmitter.h"
#include "FBXFile.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class ShadowObj
{
public:
	ShadowObj();
	~ShadowObj();

	void CreateBuffers();
	int loadShader(unsigned int type, const char* path);
	void CreateDrawShader();
	void Draw(glm::mat4 &ProjectView);

private:

	unsigned int m_shadowGenProgram , m_useShadowProgram; // our shader 



	unsigned int m_fbo; //frame buffer object handle
	unsigned int m_fboTexture; 
	unsigned int m_fboDepth;
	
	FBXFile* m_fbx;
	unsigned int m_vao, m_vbo, m_ibo;
	
	vec3 m_lightDirection;
	mat4 m_lightMatrix;
	
};


#endif