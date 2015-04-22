#include "Skybox.h"
#include "stb-master\stb_image.h"


Skybox::Skybox()
{
	images.push_back("./Resources/Skybox/right.jpg");
	images.push_back("./Resources/Skybox/left.jpg");
	images.push_back("./Resources/Skybox/top.jpg");
	images.push_back("./Resources/Skybox/bottom.jpg");
	images.push_back("./Resources/Skybox/back.jpg");
	images.push_back("./Resources/Skybox/front.jpg");
	
	
	cubeMapTexture = loadCubeMap(images);
	CreateDrawShaders();
	
	CreateBuffers();
	
}

Skybox::~Skybox()
{


}



unsigned int Skybox::loadCubeMap(std::vector<const char*> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	
	int imagewidth, imageheight, imageformat;
	unsigned char* image;

	glBindTexture(GL_SAMPLER_CUBE, textureID);
	
	for(unsigned int i = 0; i <faces.size(); i++)
	{
		image = stbi_load(faces[i], &imagewidth,&imageheight,&imageformat, STBI_default);
		//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imagewidth, imageheight, 0, GL_RGB,GL_UNSIGNED_BYTE,image);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imagewidth, imageheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
		
	}
	//GL_SAMPLER_CUBE;
	

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP,0);

	return textureID;
}

int Skybox::loadShader(unsigned int type, const char* path)
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


void Skybox::CreateDrawShaders()
{
	int success = GL_FALSE;
	unsigned int vs = loadShader(GL_VERTEX_SHADER,"./Shaders/Skybox.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER,"./Shaders/Skybox.frag");

	//Creating the Shader
	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glLinkProgram(m_drawShader);

	glGetProgramiv(m_drawShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_drawShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_drawShader, infoLogLength, 0, infoLog);
		
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		
		delete[] infoLog;
	}




	//Removing the Shader
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Skybox::CreateBuffers()
{
	GLfloat cubeVertices[] = {
		  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
	};


	//Creating Buffers
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	

	//Binding buffers
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	//Sending data through
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	//Enabling location 1 of the shader
	glEnableVertexAttribArray(0);

	//Setting three parameters for it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),(GLvoid*)0);
	

	glBindVertexArray(0);
	

}

void Skybox::Draw(FlyCamera *camera)
{

	glDepthMask(GL_FALSE);
	glUseProgram(m_drawShader);
	//Set view and projection matrix
	int loc = glGetUniformLocation(m_drawShader, "ProjectionView");

	
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(camera->getProjectionTransform()[0][0]));


	loc = glGetUniformLocation(m_drawShader, "Projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(camera->getProjectionView()[0][0]));

	view = (mat3)camera->getView();
	viewer = (mat4)view;

	loc = glGetUniformLocation(m_drawShader, "View");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(viewer[0][0]));

	
	

	glActiveTexture(GL_TEXTURE0);
	loc = glGetUniformLocation(m_drawShader, "skybox");
	glUniform1i(loc, cubeMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
	
	glBindVertexArray(m_vao);
	
	glDrawArrays(GL_TRIANGLES,0,36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);

}

