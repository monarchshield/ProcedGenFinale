#ifndef TEXTURE_H
#define TEXTURE_H


#include "Application.h"
#include "FlyCamera.h"

class Texture
{
public:
	Texture();
	Texture(const char* filepath, int Width, int height, int format);
	Texture(const char* filepath, const char* filepath2,  int Width, int height, int format);


	void Draw(FlyCamera *camera);

	void CreateShader();

	~Texture();

private:
	int imageWidth;
	int imageHeight;
	int imageFormat;

	const char* vsSource;
	const char* fsSource;

	unsigned int m_texture;
	unsigned int m_program;

	unsigned int m_vao, m_vbo, m_ibo;
	unsigned int m_normal;

	const char* filename;
	unsigned char* data;



};





#endif