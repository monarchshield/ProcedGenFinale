#include "GPUParticleEmitter.h"
#include <stdio.h>



GPUParticleEmitter::GPUParticleEmitter()
{
	m_particles = nullptr;
	m_maxParticles = 0;
	m_drawShader = 0;
	m_updateShader = 0;
	m_lastDrawTime = 0;
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}


GPUParticleEmitter::GPUParticleEmitter(const char *textureloc) 
{
	m_particles = nullptr;
	m_maxParticles = 0;
	m_drawShader = 0;
	m_updateShader = 0;
	m_lastDrawTime = 0;
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;


	CreateTexture(textureloc);
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] m_particles;
	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	//delete the shaders
	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}


void GPUParticleEmitter:: initialise(unsigned int a_maxParticles,
									float a_lifespanMin, float a_lifespanMax,
									float a_velocityMin, float a_velocityMax,
									float a_startSize, float a_endSize,
									const glm::vec4& a_startColour,
									const glm::vec4& a_endColour)
{

	//stores all varables passed in
	m_startColour = a_startColour;
	m_endColour = a_endColour;
	m_startSize = a_startSize;
	m_endSize = a_endSize;

	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;

	m_lifeSpanMin = a_lifespanMin;
	m_lifeSpanMax = a_lifespanMax;
	
	m_maxParticles = a_maxParticles;

	m_particles = new GPUParticle[a_maxParticles];

	//set our starting ping-pong buffer HAH!
	m_activeBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();

	
}



void GPUParticleEmitter::Draw(float time, const glm::mat4& a_cameraTransform, const glm::mat4&a_projectionView)
{

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	

	//update the particles using transform feedback
	glUseProgram(m_updateShader);

	//bind time information
	int location = glGetUniformLocation(m_updateShader,"time");
	glUniform1f(location, time);

	float deltaTime = time - m_lastDrawTime;
	m_lastDrawTime = time;

	location = glGetUniformLocation(m_updateShader,"deltaTime");
	glUniform1f(location, deltaTime);

	//bind Emitter Position
	location = glGetUniformLocation(m_updateShader,"emitterPosition");
	glUniform3fv(location,1, &m_position[0]);

	//disable rasterisation
	glEnable(GL_RASTERIZER_DISCARD);

	//bind the bugger we will update
	glBindVertexArray(m_vao[m_activeBuffer]);

	//work out the "other" buffer
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	//bind the buffer we will update into as points
	//and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuffer]);

	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	//disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	//Draw the particles using the geometry shader to billboard them

	glUseProgram(m_drawShader);

	location = glGetUniformLocation(m_drawShader,"projectionView");
	glUniformMatrix4fv(location, 1, false, &a_projectionView[0][0]);

	location = glGetUniformLocation(m_drawShader,"cameraTransform");
	glUniformMatrix4fv(location, 1, false, &a_cameraTransform[0][0]);

	//Draw particles in the "other buff"
	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	//swap for the next frame
	m_activeBuffer = otherBuffer;

	int location2 = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location2,1, &m_endColour[0]);

	location2 = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location2,1, &m_startColour[0]);

}


void GPUParticleEmitter::CreateTexture(const char *textureloc)
{
	if(textureloc != NULL)
	{
		data = stbi_load(textureloc,&imageWidth,&imageHeight,&imageFormat, STBI_default);
	
		glGenTextures(1, &m_texture);
		glBindSampler(GL_TEXTURE_2D, m_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
		stbi_image_free(data);
	
	}
}


void GPUParticleEmitter::CreateBuffers()
{



	//create opengl buffer 

	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	//Setup the first buffer
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2,1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle),((char*)0) + 24);
	glVertexAttribPointer(3,1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	//setup the second buffer
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2,1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle),((char*)0) + 24);
	glVertexAttribPointer(3,1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

	//tell the shader where texture is


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticleEmitter::CreateUpdateShader()
{
	//create a shader
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/gpuParticleUpdate2.vert");



	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);

	//specify the data that we will stream back
	const char* varyings[] = {"position", "velocity", "lifetime", "lifespan"};
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_updateShader);

	//removeUnneded handles
	glDeleteShader(vs);

	//Bind the shader so that we can set some uniforms that dont change per frame
	glUseProgram(m_updateShader);


	// bind lifetime minimum and maximum
	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifeSpanMin);
	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifeSpanMax);


}

void GPUParticleEmitter::CreateDrawShader()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Shaders/gpuParticle.vert");
	unsigned int gs = loadShader(GL_GEOMETRY_SHADER, "./Shaders/gpuParticle.geom");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Shaders/gpuParticle.frag");

	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader,vs);
	glAttachShader(m_drawShader,fs);
	glAttachShader(m_drawShader,gs);
	glLinkProgram(m_drawShader);
	
	//REMOVE UNNEDED HANDLES GUH!
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);


	//bind the shader so that we can set 
	//some uniforms that dont change per-frame
	glUseProgram(m_drawShader);

	//bind size information for interpolation that wont change
	int location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);

	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	//Bind colour information for interpolation that wont change ever
	location = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location, 1, &m_startColour[0]);

	location = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location,1, &m_endColour[0]);

	loc = glGetUniformLocation(m_drawShader, "diffuse");
	glUniform1i(loc,m_texture);
}

int GPUParticleEmitter::loadShader(unsigned int type, const char* path)
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

void GPUParticleEmitter::GUIEndcolourchanger(glm::vec4 &Col)
{
	 m_endColour = Col;
}

void GPUParticleEmitter::GUIStartcolourchanger(glm::vec4 &Col)
{
	 m_startColour = Col;
}