#ifndef DEFERREDRENDER_H
#define DEFERREDRENDER_H


//Just using it for specific files that i cant be bothered to include lol
#include "ParticleEmitter.h"
#include "FBXFile.h"
#include "FlyCamera.h"

class DeferredRender
{
public:
	DeferredRender();
	~DeferredRender();

	void Draw(FlyCamera* camera);
	void CreateBuffers();
	void CreateShaders();
	
	void drawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse, FlyCamera* camera);

	int loadShader(unsigned int type, const char* path);

private:
	unsigned int m_gpassFBO, m_albedoTexture, m_positionTexture
				 , m_normalTexture, m_gpassDepth, m_quadVAO ;

	unsigned int m_lightFBO, m_lightTexture,
				m_gpassShader, m_dirlightShader, m_compositeShader;



	FBXFile* m_fbx;
};

#endif