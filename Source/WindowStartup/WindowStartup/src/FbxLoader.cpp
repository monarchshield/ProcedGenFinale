#include "FBXLoader.h"

FbxLoader::FbxLoader(const char *path, vec4 Position)
{
	//TODO: Different Files use different Matierlas
	//Create something that specifies the textures they will use!
	//glBindTexture(GL_TEXTURE_2D,mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

	m_fbx = new FBXFile();
	m_fbx->load(path,FBXFile::UNITS_METER,true,true,true);
	m_fbx->initialiseOpenGLTextures();

	
	m_position = Position;

	LightColour = vec3(1,1,1);

	UniqueTexture = false;
	CreateBuffers();
}

FbxLoader::FbxLoader(const char *path, const char *textureloc, vec4 Position)
{
	m_fbx = new FBXFile();
	m_fbx->load(path,FBXFile::UNITS_METER,true,true,true);
	LightColour = vec3(1,1,1);

	UniqueTexture = true;

	m_position = Position;

	if(textureloc != NULL)
	{
		unsigned char *data = stbi_load(textureloc,&imageWidth,&imageHeight,&imageFormat, STBI_default);
	
		

		glGenTextures(1, &m_texture);
		glBindSampler(GL_TEXTURE_2D, m_texture);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		stbi_image_free(data);
	}


	CreateBuffers();

}



FbxLoader::~FbxLoader()
{

}

void FbxLoader::CreateBuffers()
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{

		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		if (mesh->m_material != NULL)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		}
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
		glEnableVertexAttribArray(1); // Color
		glEnableVertexAttribArray(2); // UV
		glEnableVertexAttribArray(3); // Normal
		
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),((char*)0) + FBXVertex::PositionOffset);	
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) +	FBXVertex::ColourOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
		}

		


}

void FbxLoader::Draw(unsigned int Shader, FlyCamera *camera, vec3 LightDir, vec3 AmbientColor)
{

	glUseProgram(Shader);

	int loc = glGetUniformLocation(Shader, "WorldPosition");
	glUniform4f(loc, m_position.x,m_position.y,m_position.z,0);


	loc = glGetUniformLocation(Shader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(camera->getProjectionTransform()[0][0]));

	loc = glGetUniformLocation(Shader, "CameraPos");
	glUniform3f(loc, camera->xPosition,camera->yPosition,camera->zPosition);

	loc = glGetUniformLocation(Shader, "LightDir");
	glUniform3f(loc, LightDir.x,LightDir.y,LightDir.z);

	loc = glGetUniformLocation(Shader, "LightColour");
	glUniform3f(loc, LightColour.x,LightColour.y,LightColour.z);

	loc = glGetUniformLocation(Shader, "SpecPow");
	glUniform1f(loc, 1);

	
	
	//loc = glGetUniformLocation(m_drawShader, "WorldPosition");
	//glUniform4f(loc, 20,0,0,1);
	 
	loc = glGetUniformLocation(Shader, "AmbientColour");
	glUniform3f(loc, AmbientColor.x,AmbientColor.y,AmbientColor.z);

	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) 
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		
		if(UniqueTexture != true)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		}
	
		else
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,m_texture);
			loc = glGetUniformLocation(Shader, "diffuse");
			glUniform1f(loc,0);

		}

		unsigned int* glData = (unsigned int*)mesh->m_userData;
	
		loc = glGetUniformLocation(Shader, "global");
		glUniformMatrix4fv(loc,1,GL_FALSE,&(mesh->m_globalTransform)[0][0]);
	
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	
		mesh->m_userData = glData;
		
	}
}

