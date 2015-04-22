#ifndef FBXLOADER_H
#define FBXLOADER_H

#include "FBXFile.h"
#include "Application.h"
#include "stb-master\stb_image.h"

class FBXFile;


class FbxLoader
{
public:


	FbxLoader(const char *path, vec4 Position);
	FbxLoader(const char *path, const char *textureloc, vec4 Position);
	~FbxLoader();

	void CreateShaders();
	void CreateBuffers();
	void Draw(FlyCamera *camera, vec3 LightDir, vec3 AmbientColor);
	int loadShader(unsigned int type, const char* path);

private:

	unsigned int m_texture;
	int imageWidth, imageHeight, imageFormat;

	vec4 m_position;

	bool UniqueTexture;
	FBXFile* m_fbx;
	unsigned int m_drawShader;
	unsigned int m_VAO, m_VBO, m_IBO;
	vec3 LightColour;
};


#endif