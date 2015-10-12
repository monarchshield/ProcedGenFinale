#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Application.h"
#include "stb-master\stb_image.h"


struct Vertex;

class ObjLoader
{
public:
	ObjLoader();
	
	ObjLoader(const char *path, const char *textureloc, float Faces);
	~ObjLoader();


	void Initialise(const char *path, const char *textureloc);
	void CreateShaders();
	void CreateBuffers();
	void Draw(unsigned int shader, mat4 &ProjectionView);
	int loadShader(unsigned int type, const char* path);
	
private:
	std::vector< Vertex > Vertices;
	std::vector< vec2 > ModelUV;
	std::vector< vec3 > normals;
	std::vector< unsigned int> Elements;
	float Facenum;

	unsigned int m_programID; //this is the shader id;
	int imageWidth, imageHeight, imageFormat, loc;
	
	unsigned char* data;
	unsigned int m_vertexCount, m_ModelIndexCount;
	unsigned int m_VAO, m_VBO, m_IBO;
	unsigned int m_texture;
};




#endif