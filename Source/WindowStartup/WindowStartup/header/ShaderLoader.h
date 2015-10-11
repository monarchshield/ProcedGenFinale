//Author: Anthony Bogli
//Description: Creating a shader loader class to fix up terrible code 


#ifndef SHADERLOADER_H
#define SHADERLOADER_H

class ShaderLoader
{
public:
	ShaderLoader();
	~ShaderLoader();


	void CreateShaders(unsigned int shader, const char* VertShaderLoc, const char* FragShaderLoc);
	int loadShader(unsigned int type, const char* path);


private:
	
	unsigned int m_defaultShader;
	unsigned int m_LightingShader;
	unsigned int m_defferedRenderingShader;
	unsigned int m_SkyBoxShader;
	


};

#endif