#ifndef PROCEDURALGEN_H
#define PROCEDURALGEN_H


#include "Application.h"
#include "stb-master\stb_image.h"

struct Vertex3
{
	vec4 position;
	vec4 colour;
	vec2 UV;
	vec3 Normal;
};


class ProceduralGen
{
public:
	ProceduralGen();
	~ProceduralGen();
	
	void CreateBuffers();
	void GenerateTexture(float seed, float Amp, float persis);
	void CreateShaders();
	void GenerateNormals(Vertex3 *vert1, Vertex3 *vert2, Vertex3 *vert3);


	void GenerateGrid(unsigned int rows, unsigned int cols);

	void Draw(FlyCamera *camera, vec3 LightDir, vec3 AmbientColor);

	unsigned int loadTexture(const char* texture);


	int loadShader(unsigned int type, const char* path);

	int scalesize;
	float amplitude;
	float persistence;

private:

	Vertex3* aoVertices;
	unsigned int* auiIndices;
	vec3 LightColour;

	unsigned int total;

	int imageWidth, imageHeight, imageFormat, loc;
	unsigned char* data;
	
	
	//Used for textures
	unsigned int m_grass, m_water, m_texture, m_sand;

	float m_seed;

	unsigned int indexcount;

	unsigned int m_perlin_texture, m_drawshader, m_indexCount;
	unsigned int m_vao, m_vbo, m_ibo;
};



#endif