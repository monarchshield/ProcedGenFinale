
#include "Texture.h"
#include "stb-master\stb_image.h"


Texture::Texture()
{

}

Texture::Texture(const char* filepath, int Width, int height, int format)
{

	filename = filepath;
	imageWidth = Width;
	imageHeight = height;
	imageFormat = format;
	
	data = stbi_load(filename,&imageWidth,&imageHeight,&imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);



}

Texture::Texture(const char* filepath,const char* filepath2, int Width, int height, int format)
{

	filename = filepath;
	imageWidth = Width;
	imageHeight = height;
	imageFormat = format;
	
	data = stbi_load(filename,&imageWidth,&imageHeight,&imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	
	data = stbi_load(filepath2,&imageWidth,&imageHeight,&imageFormat, STBI_default);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

}



Texture::~Texture()
{
	
}

void Texture::Draw(FlyCamera *camera)
{
	Vertex2 vertexData[] = {
	{ -5, 0, 5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
	{ 5, 0, 5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1 },
	{ 5, 0, -5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
	{ -5, 0, -5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },


	};
	
	unsigned int indexData[] = {
	0, 1, 2,
	0, 2, 3,
	};
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex2) * 6 * 4, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);
	
	

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);


	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2), ((char*)0)+48);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,	sizeof(Vertex2), ((char*)0)+16);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2), ((char*)0)+32);





	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// use our texture program
	glUseProgram(m_program);
	// bind the camera
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
	&(camera->getProjectionTransform()[0][0]));
	
	// set texture slots
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	
	// tell the shader where it is
	loc = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(m_program, "normal");
	glUniform1i(loc, 1);
	// bind the light
	vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));
	
	loc = glGetUniformLocation(m_program, "LightDir");
	
	glUniform3f(loc, light.x, light.y, light.z);
	
	// draw
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Texture::CreateShader()
{
	vsSource = "#version 410\n \
				layout(location=0) in vec4 Position; \
				layout(location=1) in vec2 TexCoord; \
				layout(location=2) in vec4 Normal; \
				layout(location=3) in vec4 Tangent; \
				out vec2 vTexCoord; \
				out vec3 vNormal; \
				out vec3 vTangent; \
				out vec3 vBiTangent; \
				uniform mat4 ProjectionView; \
				void main() { vTexCoord = TexCoord; \
				vNormal = Normal.xyz; \
				vTangent = Tangent.xyz; \
				vBiTangent = cross(vNormal, vTangent); \
				gl_Position = ProjectionView * Position; }";
	
	fsSource =  "#version 410\n \
				 in vec2 vTexCoord; \
				 in vec3 vNormal; \
				 in vec3 vTangent; \
				 in vec3 vBiTangent; \
				 out vec4 FragColor; \
				 uniform vec3 LightDir; \
				 uniform sampler2D diffuse; \
				 uniform sampler2D normal; \
				 void main() { \
				 mat3 TBN = mat3( \
				 normalize( vTangent ), \
				 normalize( vBiTangent ), \
				 normalize( vNormal )); \
				 vec3 N = texture(normal, \
				 vTexCoord).xyz * 2 - 1; \
				 float d = max( 0, dot( \
				 normalize( TBN * N ), \
				 normalize( LightDir ))); \
				 FragColor = texture(diffuse, vTexCoord); \
				 FragColor.rgb = FragColor.rgb * d;}";



	m_program = glCreateProgram();


	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	
	
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}