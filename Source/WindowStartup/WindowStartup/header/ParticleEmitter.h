#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>



struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 colour;
	float size;
	float lifetime;
	float lifespan;
};

struct ParticleVertex 
{
	glm::vec4 position;
	glm::vec4 colour;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void ParticleShader();

	virtual void initialise(unsigned int a_maxParticles, unsigned int a_emitRate,
					float a_lifetimeMin, float a_lifetimeMax,
					float a_velocityMin, float a_velocityMax,
					float a_startSize, float a_endSize,
					const glm::vec4& a_startColour, const glm::vec4& a_endColour);
	
	void Emit();
	void update(float a_deltatime, const glm::mat4& a_cameraTransform);

	virtual void Draw();


protected:
	Particle * m_particles;
	unsigned int m_firstDead;
	unsigned int m_maxParticles;



	glm::vec3 m_position;
	
	float m_emitTimer;
	float m_emitRate;

	float m_lifeSpanMin;
	float m_lifeSpanMax;

	float m_velocityMin;
	float m_velocityMax;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColour;
	glm::vec4 m_endColour;

	unsigned int m_vao, m_vbo, m_ibo;
	ParticleVertex* m_vertexData;
	int m_programID;

};




#endif