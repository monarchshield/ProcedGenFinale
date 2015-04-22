#include "DeferredRender.h"

DeferredRender::DeferredRender()
{
	m_fbx = new FBXFile();
	m_fbx->load("./Resources/Bunny.fbx", FBXFile::UNITS_METER,true,true,true);

	CreateBuffers();
	CreateShaders();
}

DeferredRender::~DeferredRender()
{

}

void DeferredRender::CreateBuffers()
{
	//setup gpass framebuffer
	glGenFramebuffers(1, &m_gpassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);

	glGenTextures(1, &m_albedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glGenTextures(1, &m_positionTexture);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB32F,1280,720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glGenTextures(1, &m_normalTexture);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glTexStorage2D(GL_TEXTURE_2D,1,GL_RGB32F,1280,720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glGenRenderbuffers(1, &m_gpassDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gpassDepth);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,1280,720);

	
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, m_albedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1, m_positionTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2, m_normalTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gpassDepth);

	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, gpassTargets);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//setup light framebuffer

	glGenFramebuffers(1, &m_lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);

	glGenTextures(1, &m_lightTexture);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lightTexture, 0);

	GLenum lightTargets[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, lightTargets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	
	if(status != GL_FRAMEBUFFER_COMPLETE)
		printf("FRAMEBUFFER ERROR!\n");

	glBindFramebuffer(GL_FRAMEBUFFER,0);

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

int DeferredRender::loadShader(unsigned int type, const char* path)
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


void DeferredRender::Draw(FlyCamera* camera)
{
	//G-Pass: render out the albedo, position and normal
	//More like, render out the libido, amirite

	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_gpassShader);

	//bind camera transforms
	int loc = glGetUniformLocation(m_gpassShader, "ProjectionView");
	glUniformMatrix4fv(loc,1,GL_FALSE,&(camera->getProjectionTransform()[0][0]));

	loc = glGetUniformLocation(m_gpassShader, "View");
		  glUniformMatrix4fv(loc,1,GL_FALSE,&(camera->getView()[0][0]));

	//draw our scene or e.g our bunny
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;	
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

	}

	//	//Light pass: render lights as geometry sampling position
	//// and normals disable depth testing and enable additive blending 
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	//glClear(GL_COLOR_BUFFER_BIT);
	//
	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE,GL_ONE);
	//
	//glUseProgram(m_dirlightShader);



}

void DeferredRender::drawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse, FlyCamera* camera)
{
	vec4 viewspacelight = camera->getView() * vec4(glm::normalize(direction), 0);

	int loc =  glGetUniformLocation(m_gpassShader, "lightDirection");
	
	glUniform3fv(loc,1, &viewspacelight[0]);
	loc = glGetUniformLocation(m_dirlightShader, "lightDiffuse");
	glUniform3fv(loc,1,&diffuse[0]);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES,0,6);

}

void DeferredRender::CreateShaders()
{
		unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/Shaders/Gbuffer.frag");
 		unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/Shaders/Gbuffer.vert");

		m_gpassShader = glCreateProgram();
		glAttachShader(m_gpassShader, vs);
		glAttachShader(m_gpassShader, fs);
		glLinkProgram(m_gpassShader);

		fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/Shaders/Composite.frag");
		vs = loadShader(GL_VERTEX_SHADER, "./Shaders/Shaders/Composite.vert");

		m_compositeShader = glCreateProgram();
		glAttachShader(m_compositeShader, vs);
		glAttachShader(m_compositeShader, fs);
		glLinkProgram(m_compositeShader);
}