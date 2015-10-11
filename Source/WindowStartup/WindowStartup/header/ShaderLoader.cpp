
#include "ShaderLoader.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>

ShaderLoader::ShaderLoader()
{
	CreateShaders(m_defaultShader, "./Shaders/Default.vert", "./Shaders/Default.vert");
	CreateShaders(m_LightingShader, "./Shaders/FbxShader.vert", "./Shaders/FbxShader.vert");
	CreateShaders(m_SkyBoxShader, "./Shaders/Skybox.vert", "./Shaders/Skybox.vert");

	//TODO: Deffered render
	//CreateShaders(m_defferedRenderingShader, "./Shaders/Default.vert", "./Shaders/Default.vert");
}

ShaderLoader::~ShaderLoader()
{
	glDeleteProgram(m_defaultShader);
	glDeleteProgram(m_LightingShader);
	glDeleteProgram(m_SkyBoxShader);
}

int ShaderLoader::loadShader(unsigned int type, const char* path)
{
	FILE* file = fopen(path, "rb");

	if (file == nullptr)
	{
		std::cout << "Failed to Find Shader:" << path << "\n";
		return 0;
	}
	//Read the shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;

	return shader;
}

void ShaderLoader::CreateShaders(unsigned int shader, const char* VertShaderLoc, const char* FragShaderLoc)
{
	int success = GL_FALSE;

	//Creating the shaders here
	unsigned int vs = loadShader(GL_VERTEX_SHADER, VertShaderLoc);
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, FragShaderLoc);

	shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(shader, infoLogLength, 0, infoLog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);

		delete[] infoLog;
	}

	//Removing crappy handles here
	glDeleteShader(vs);
	glDeleteShader(fs);
}