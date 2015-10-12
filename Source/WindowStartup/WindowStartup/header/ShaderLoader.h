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

	unsigned int UseDefaultShader() { return m_defaultShader; }
	unsigned int UseLightingShader() { return m_LightingShader; }
	unsigned int UseDeferredRenderShader() { return m_defferedRenderingShader; }
	unsigned int UseSkyBoxShader() { return m_SkyBoxShader; }
	unsigned int UseFlatLitShader() { return m_flatLitShader;  }


protected:
	void CreateDefaultShader();
	void CreateLightingShader();
	void CreateDefferedRenderShader();
	void CreateSkyboxShader();
	void CreateFlatLitShader();



private:
	
	unsigned int m_defaultShader;
	unsigned int m_LightingShader;
	unsigned int m_defferedRenderingShader;
	unsigned int m_SkyBoxShader;
	unsigned int m_flatLitShader;


};

#endif