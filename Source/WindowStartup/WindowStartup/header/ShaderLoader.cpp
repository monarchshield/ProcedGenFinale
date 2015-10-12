
#include "ShaderLoader.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>

ShaderLoader::ShaderLoader()
{
	CreateDefaultShader();
	CreateLightingShader();
	CreateFlatLitShader();
	CreateSkyboxShader();

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


#pragma region CreateShaderFunctions

void ShaderLoader::CreateDefaultShader()
{
	int success = GL_FALSE;

	//Creating the shaders here
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/Default.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/Default.frag");

	m_defaultShader = glCreateProgram();
	glAttachShader(m_defaultShader, vs);
	glAttachShader(m_defaultShader, fs);
	glLinkProgram(m_defaultShader);

	glGetProgramiv(m_defaultShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_defaultShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_defaultShader, infoLogLength, 0, infoLog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);

		delete[] infoLog;
	}

	//Removing crappy handles here
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void ShaderLoader::CreateLightingShader()
{
	//FBXLoaderShader

	int success = GL_FALSE;

	//Creating the shaders here
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/LightingShader.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/LightingShader.frag");

	m_LightingShader = glCreateProgram();
	glAttachShader(m_LightingShader, vs);
	glAttachShader(m_LightingShader, fs);
	glLinkProgram(m_LightingShader);

	glGetProgramiv(m_LightingShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_LightingShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_LightingShader, infoLogLength, 0, infoLog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);

		delete[] infoLog;
	}

	//Removing crappy handles here
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void ShaderLoader::CreateDefferedRenderShader()
{

}
void ShaderLoader::CreateSkyboxShader()
{
	//FBXLoaderShader

	int success = GL_FALSE;

	//Creating the shaders here
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/Skybox.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/Skybox.frag");

	m_SkyBoxShader = glCreateProgram();
	glAttachShader(m_SkyBoxShader, vs);
	glAttachShader(m_SkyBoxShader, fs);
	glLinkProgram(m_SkyBoxShader);

	glGetProgramiv(m_SkyBoxShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_SkyBoxShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_SkyBoxShader, infoLogLength, 0, infoLog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);

		delete[] infoLog;
	}

	//Removing crappy handles here
	glDeleteShader(vs);
	glDeleteShader(fs);
}
void ShaderLoader::CreateFlatLitShader()
{
	int success = GL_FALSE;

	//Creating the shaders here
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/FlatLit.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/FlatLit.frag");

	m_flatLitShader = glCreateProgram();
	glAttachShader(m_flatLitShader, vs);
	glAttachShader(m_flatLitShader, fs);
	glLinkProgram(m_flatLitShader);

	glGetProgramiv(m_flatLitShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_flatLitShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_flatLitShader, infoLogLength, 0, infoLog);

		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);

		delete[] infoLog;
	}

	//Removing crappy handles here
	glDeleteShader(vs);
	glDeleteShader(fs);
}

#pragma endregion

