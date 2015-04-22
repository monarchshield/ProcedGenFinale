#include "AdvancedNav.h"

AdvancedNav::AdvancedNav()
{
	m_sponza = new FBXFile();
	
	m_sponza->load("./Resources/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER);

	CreateShader();
	CreateBuffers();

}


AdvancedNav::~AdvancedNav()
{

}

int AdvancedNav::loadShader(unsigned int type, const char* path)
{

	FILE* file = fopen(path, "rb");

	if(file == nullptr)
	{
		std::cout << "Failed to Find Shader:" << path << "\n";
		return 0;
	}
	//Read the shader source
	fseek(file,0,SEEK_END);
	unsigned int length = ftell(file);
	fseek(file,0,SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0 , length + 1);
	fread(source,sizeof(char),length,file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader,1, &source, 0);
	glCompileShader(shader);
	delete[] source;

	return shader;


}

void AdvancedNav::CreateShader()
{

}

void AdvancedNav::CreateBuffers()
{

}

void AdvancedNav::Draw(glm::mat4 &projectview)
{

}