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
	
	void createOpenGLBuffers(FBXFile* fbx);

	void loadFbx(const char *path);
	bool loadObj(const char *path, const char *textureloc, vec4 &SetPosition ,std::vector<Vertex> &Vertices, std::vector<vec2> &uvs, std::vector< vec3 > &normals, std::vector< unsigned int> &Elements,float &Facenum);

	void CreateShaders();
	void CreateShaderFBX();
	


private:
	
	// our vertex and index buffers
	unsigned int m_VAO;	// Vertex Array Object (Just winging it)
	unsigned int m_VBO; // Vertex Buffer Object (Stores vertex Information)
	unsigned int m_IBO;	// Index Buffer Object (Stores Indicies)?
	unsigned char* data;
	int m_rows;
	int m_collumns;

	
	const char *filepath;
	
	const char *vsSource;
	const char *fsSource;

	TwBar* m_bar;
	vec4 m_clearcolour;

	bool res;

	FBObject* plane;

	unsigned int m_programID;

	unsigned int vertexShader;
	unsigned int fragmentShader;


	int vertcount;
	unsigned int* indiciesCount;

	unsigned int m_indexCount;

	float m_timer;

	int success;
	//FBXFile file;


	FbxLoader* m_EnemyElite;
	FbxLoader* m_Pyro;
	FbxLoader* m_Tree;

	Skybox* m_skybox;

	FBXFile* m_fbx;

	ParticleEmitter *m_emitter;
	ObjLoader* ObjModel;
	//Stuff to do with textures
	Texture *image;
	unsigned int m_texture;
	unsigned int imageWidth, imageHeight, imageFormat, loc;
	
	ProceduralGen* m_terrain;
	

	GLFWwindow * window;
	FlyCamera * camera;
	float previousTime;
	float currentTime;
	float DeltaTime;

	ShadowObj * ShadowPlane;

	int m_seed;
	float m_amp , m_persis;
	

	//Used for storing Vertex, Uvs and Normals of our Obj model :)
	std::vector< unsigned int> vertexIndices, uvIndices, normalIndices;


	int m_vertexCount;

	vec3 m_LightDirection;
	vec3 m_AmbientColour;

	DeferredRender* render;
	vec4 m_startcolour, m_endcolour;


	GPUParticleEmitter* m_GPUParticles;


	unsigned int m_ModelIndexCount;
	std::vector<unsigned int> m_elements;
	std::vector<Vertex> ModelVerts;
	std::vector<vec2> ModelUV;
	std::vector<vec3> ModelNormals;

};







#endif