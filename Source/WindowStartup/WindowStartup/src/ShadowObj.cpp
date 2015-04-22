#include "ShadowObj.h"

ShadowObj::ShadowObj()
{
	m_fbx = new FBXFile();
	m_fbx->load("./Resources/Bunny.fbx", FBXFile::UNITS_METER,true,true,true);

	CreateBuffers();
	CreateDrawShader();

	m_lightDirection = glm::normalize(vec3(1,2.5f,1));
	mat4 lightprojection = glm::ortho<float>(-10,10,-10,10,-10,10);
	mat4 lightView = glm::lookAt(m_lightDirection, vec3(0), vec3(0,1,0));
	m_lightMatrix = lightprojection * lightView;

}
ShadowObj::~ShadowObj()
{
	glDeleteProgram(m_shadowGenProgram);
	glDeleteProgram(m_useShadowProgram);
}

void ShadowObj::CreateBuffers()
{

	//setup shadow map buffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	
	glGenTextures(1, &m_fboDepth);
	glBindTexture(GL_TEXTURE_2D, m_fboDepth);
	
	//Texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT16,1024,1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	
	//attatches as a depth attatchment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_fboDepth, 0);
	
	//no color targets are used
	glDrawBuffer(GL_NONE);
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error!\n");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float vertexData[] = 
	{
		-15, 0, -15, 1, 0, 0,
		15, 0, -15, 1, 1, 0,
		-15, 0, 15, 1, 1, 1,
		15, 0, 15, 1, 0, 1,
	};
	unsigned int indexData[] = 
	{
		0, 1, 2,
		1, 2, 3,
	};
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertexData, GL_STATIC_DRAW);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{

			FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

			glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D,mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);


			// storage for the opengl data in 3 unsigned int
			unsigned int* glData = new unsigned int[3];
			// continued next page…

			glGenVertexArrays(1, &glData[0]);
			glBindVertexArray(glData[0]);
			glGenBuffers(1, &glData[1]);
			glGenBuffers(1, &glData[2]);
			
			glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]); 
			
			glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);
			
			glEnableVertexAttribArray(0); // position
			glEnableVertexAttribArray(1); // normal

			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),((char*)0) + FBXVertex::PositionOffset);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::NormalOffset);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			mesh->m_userData = glData;
		}

	

}

int ShadowObj::loadShader(unsigned int type, const char* path)
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

void ShadowObj::CreateDrawShader()
{
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/SS/ShadowMap.frag");
 	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/SS/ShadowMap.vert");

	//Creating the Shader
	m_shadowGenProgram = glCreateProgram();
	glAttachShader(m_shadowGenProgram, vs);
	glAttachShader(m_shadowGenProgram, fs);
	glLinkProgram(m_shadowGenProgram);

	unsigned int fs1  = loadShader(GL_FRAGMENT_SHADER, "./Shaders/SS/ShadowGen.frag");
	unsigned int vs1  = loadShader(GL_VERTEX_SHADER, "./Shaders/SS/ShadowGen.vert");


	m_useShadowProgram = glCreateProgram();
	glAttachShader( m_useShadowProgram, vs1);
	glAttachShader( m_useShadowProgram, fs1);
	glLinkProgram(m_useShadowProgram);


	//Removing the Shader
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(vs1);
	glDeleteShader(fs1);
}

void ShadowObj::Draw(glm::mat4 &ProjectView)
{


	//Shadow pass: bind our shadow map target and clear the depth
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0,0,1024,1024);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shadowGenProgram);

	//Bind the light matrix
	int loc = glGetUniformLocation(m_shadowGenProgram, "LightMatrix");
	glUniformMatrix4fv(loc,1,GL_FALSE, &(m_lightMatrix[0][0]));

	//draw wall shadow-casting geometry
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;	
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

	}

	//Final pass: bind back-buffer
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,1280,720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_useShadowProgram);

	//bind the camera
	loc = glGetUniformLocation(m_useShadowProgram, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(ProjectView[0][0]));

	//bind the light matrix
	mat4 textureSpaceOffset(0.5f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.5f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.5f, 0.0f,
							0.5f, 0.5f, 0.5f, 1.0f);
	
	mat4 lightMatrix = textureSpaceOffset * m_lightMatrix;

	loc = glGetUniformLocation(m_useShadowProgram, "LightMatrix");
	glUniformMatrix4fv(loc,1,GL_FALSE, &lightMatrix[0][0]);

	loc = glGetUniformLocation(m_useShadowProgram, "lightDir");
	glUniform3fv(loc,1, &m_lightDirection[0]);

	loc = glGetUniformLocation(m_useShadowProgram, "shadowMap");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(m_useShadowProgram, "shadowBias");
	glUniform1f(loc, 0.01f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboDepth);

	//bind our vertex array object and draw the mesh
		for (unsigned int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;	
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

	}

	//Draw  aplane under the bunny
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}