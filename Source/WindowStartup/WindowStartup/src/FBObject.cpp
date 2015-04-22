#include "FBObject.h"


FBObject::FBObject()
{
	CreateBuffers();
	CreateDrawShader();
}

FBObject::~FBObject()
{
	glDeleteProgram(m_drawShader);
}


int FBObject::loadShader(unsigned int type, const char* path)
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

void FBObject::CreateBuffers()
{

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboTexture);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture, 0);
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	float vertexData[] = 
	{
		-5, 0, -5, 1, 0, 0,
		5, 0, -5, 1, 1, 0,
		5, 10, -5, 1, 1, 1,
		-5, 10, -5, 1, 0, 1,
	};
	unsigned int indexData[] = 
	{
		0, 1, 2,
		0, 2, 3,
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
}




void FBObject::CreateDrawShader()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/FBO.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/FBO.frag");

	//Creating the Shader
	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glLinkProgram(m_drawShader);

	//Removing the Shader
	glDeleteShader(vs);
	glDeleteShader(fs);

}




void FBObject::Draw(glm::mat4 &ProjectView)
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 512, 512);
	glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw stuff inbetween here

	Gizmos::draw(ProjectView);


	//blah

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_drawShader);
	
	//Gizmos::draw(ProjectView);

	int loc = glGetUniformLocation(m_drawShader,"ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(ProjectView[0][0]));





	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture);
	glUniform1i(glGetUniformLocation(m_drawShader, "diffuse"), 0);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}