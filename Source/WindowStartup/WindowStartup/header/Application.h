#ifndef APPLICATION_H
#define APPLICATION_H

#include "Skybox.h"
#include "DeferredRender.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include "AntTweakBar.h"
#include "FBXFile.h"
#include "ParticleEmitter.h"
#include "GPUParticleEmitter.h"
#include "FBObject.h"
#include <iostream>
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include "FlyCamera.h"
#include <vector>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include "Texture.h"
#include "ShadowObj.h"
#include "ObjLoader.h"
#include "FBXLoader.h"
#include "ShaderLoader.h"



class Texture;
class ProceduralGen;
class Skybox;
class ObjLoader;
class FbxLoader;

struct Vertex2 {
float x, y, z, w;
float nx, ny, nz, nw;
float tx, ty, tz, tw;
float s, t;
};



struct Vertex
{
	vec4 position;
	vec4 colour;
	vec2 UV;
};

class Application
{
public:
	Application();
	~Application();
	
	void RunGame();
	void Update();
	void Draw();

	void GenerateGrid(unsigned int rows, unsigned int cols); //Reattempt
	
	

	

	void CreateShaders();
	void CreateShaderFBX();
	


private:
	
	// our vertex and index buffers


	int m_rows;
	int m_collumns;

	
	const char *filepath;
	


	TwBar* m_bar;
	vec4 m_clearcolour;

	bool res;

	FBObject* plane;

	unsigned int m_programID;

	unsigned int vertexShader;
	unsigned int fragmentShader;



	float m_timer;



	ShaderLoader* m_shaderloader;
	FbxLoader* m_EnemyElite;
	FbxLoader* m_Pyro;
	FbxLoader* m_Tree;
	FBXFile* m_fbx;

	ObjLoader* m_objModel;

	Skybox* m_skybox;



	ParticleEmitter *m_emitter;
	ObjLoader* ObjModel;
	//Stuff to do with textures

	ProceduralGen* m_terrain;
	

	GLFWwindow * window;
	FlyCamera * camera;
	float previousTime;
	float currentTime;
	float DeltaTime;

	ShadowObj * ShadowPlane;

	int m_seed;
	float m_amp , m_persis;
	

	int m_vertexCount;

	vec3 m_LightDirection;
	vec3 m_AmbientColour;

	DeferredRender* render;
	vec4 m_startcolour, m_endcolour;


	GPUParticleEmitter* m_GPUParticles;

};







#endif