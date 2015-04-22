#ifndef GPUPARTICLEMITTER_H
#define GPUPARTICLEMITTER_H

#include "ParticleEmitter.h"
#include "stb-master\stb_image.h"



struct GPUParticle 
{
	GPUParticle() : lifetime(1), lifespan(0) {}
	glm::vec3 position;
	glm::vec3 velocity;
	float lifetime;
	float lifespan;
};


class GPUParticleEmitter : ParticleEmitter
{
public:
	

	GPUParticleEmitter();
	GPUParticleEmitter(const char *textureloc);


	virtual ~GPUParticleEmitter();

	void Draw(float time, const glm::mat4& a_cameraTransform, const glm::mat4&a_projectionView);
	
	void initialise(unsigned int a_maxParticles,
					float a_lifespanMin, float a_lifespanMax,
					float a_velocityMin, float a_velocityMax,
					float a_startSize, float a_endSize,
					const glm::vec4& a_startColour,
					const glm::vec4& a_endColour);

	void GUIEndcolourchanger(glm::vec4 &Col);
	void GUIStartcolourchanger(glm::vec4 &Col);


	void CreateUpdateShader();
	void CreateTexture(const char *textureloc);


private:


	
	

	void CreateBuffers();
	
	void CreateDrawShader();
	GPUParticle* m_particles;

	unsigned int m_activeBuffer;
	unsigned int m_vao[2];
	unsigned int m_vbo[2];

	unsigned char* data;

	unsigned int m_drawShader, m_updateShader;
	int imageWidth, imageHeight, imageFormat, loc;

	//Used for textures
	unsigned int m_texture;


	float m_lastDrawTime;
	
	int loadShader(unsigned int type, const char* path);

};

#endif