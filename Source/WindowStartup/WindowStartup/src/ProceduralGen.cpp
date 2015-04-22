#include "ProceduralGen.h"

ProceduralGen::ProceduralGen()
{
	
	m_water = loadTexture("./Resources/Terrain/water.png");
	m_grass = loadTexture("./Resources/Terrain/grass.png");
	m_sand = loadTexture("./Resources/Terrain/sand.png");
	
	LightColour = vec3(1,1,1);
	scalesize = 5;

	GenerateGrid(64,64);
	GenerateTexture(0,1.0f,0.3f);
	CreateShaders();
	CreateBuffers();
}

ProceduralGen::~ProceduralGen()
{
	glDeleteProgram(m_drawshader);

}

void ProceduralGen::GenerateNormals(Vertex3 *vert1, Vertex3 *vert2, Vertex3 *vert3)
{
	vec3 d1 = vec3(vert1->position - vert3->position);
	vec3 d2 = vec3(vert2->position - vert3->position);
	
	vec3 crossProduct = glm::cross(d1, d2);
	
	vec3 Normal = glm::normalize(crossProduct);

	vert1->Normal = Normal;
	vert2->Normal = Normal;
	vert3->Normal = Normal;
}

void ProceduralGen::GenerateTexture(float seed, float Amp, float persis)
{
	int dims = 64;
	float *perlin_data = new float[64 * 64];
	float scale = (1.0f / dims) * scalesize;
	int octaves = 15;

	for (int x = 0; x < dims; ++x)
	{
		for (int y = 0; y < dims; ++y)
		{
			float amplitude = Amp;
			float persistence = persis;
			perlin_data[y * dims + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);


				//This is the seed
				
				float perlin_sample = glm::perlin(vec3((float)x, (float)y,(float)seed) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y* dims + x] += perlin_sample * amplitude;

				amplitude *= persistence;
			}

			
			//generate the noise here 
		}
	}


	for(unsigned i = 0; i < 4096; i++)
	{
		aoVertices[i].position.y = perlin_data[i] * 5;
	}

	unsigned int indicesSize = (dims - 1) * (dims - 1) * 6;

	//Generate new normals
	for(unsigned int i = 0; i < indicesSize; i +=3)
	{
		Vertex3* vertex1 = &aoVertices[auiIndices[i +2]];
		Vertex3* vertex2 = &aoVertices[auiIndices[i +1]];
		Vertex3* vertex3 = &aoVertices[auiIndices[i]];

		GenerateNormals(vertex1,vertex2,vertex3);
	}
	
	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}





void ProceduralGen::CreateShaders()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "Shaders/Perlin.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "Shaders/Perlin.frag");

	int success = GL_FALSE;

	//Creating the shader
	m_drawshader = glCreateProgram();
	glAttachShader(m_drawshader, vs);
	glAttachShader(m_drawshader, fs);
	glLinkProgram(m_drawshader);


	glGetProgramiv(m_drawshader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_drawshader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_drawshader, infoLogLength, 0, infoLog);
		
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		
		delete[] infoLog;
	}




	//Removing the Shader
	glDeleteShader(vs);
	glDeleteShader(fs);
}

int ProceduralGen::loadShader(unsigned int type, const char* path)
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

void ProceduralGen::CreateBuffers()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	
	glBufferData(GL_ARRAY_BUFFER, (total) * sizeof(Vertex3), aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, position));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, colour));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, UV));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3), (void*)offsetof(Vertex3, Normal));


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//Setting slots for texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);	

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_water);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_grass);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_sand);


}

void ProceduralGen::Draw(FlyCamera *camera, vec3 LightDir, vec3 AmbientColor)
{

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINES );	
	glUseProgram(m_drawshader);

	int loc = glGetUniformLocation(m_drawshader, "ProjectionView");
	glUniformMatrix4fv(loc,1,GL_FALSE,&(camera->getProjectionTransform()[0][0]));


	loc = glGetUniformLocation(m_drawshader, "diffuse");
	glUniform1i(loc, 0);
	
	loc = glGetUniformLocation(m_drawshader, "water");
	glUniform1i(loc, 1);

	loc = glGetUniformLocation(m_drawshader, "grass");
	glUniform1i(loc, 2);

	loc = glGetUniformLocation(m_drawshader, "sand");
	glUniform1i(loc, 3);


	loc = glGetUniformLocation(m_drawshader, "CameraPos");
	glUniform3fv(loc, 1, &camera->getPosition()[0]);

	loc = glGetUniformLocation(m_drawshader, "LightDir");
	glUniform3f(loc, LightDir.x,LightDir.y,LightDir.z);


	loc = glGetUniformLocation(m_drawshader, "AmbientColour");
	glUniform3f(loc, 1,1,1);


	loc = glGetUniformLocation(m_drawshader, "LightColour");
	glUniform3f(loc, LightColour.x,LightColour.y,LightColour.z);

	loc = glGetUniformLocation(m_drawshader, "SpecPow");
	glUniform1f(loc, 1);


	
	//loc = glGetUniformLocation(m_drawShader, "WorldPosition");
	//glUniform4f(loc, 20,0,0,1);
	 
	loc = glGetUniformLocation(m_drawshader, "AmbientColour"); 
	glUniform3f(loc, AmbientColor.x,AmbientColor.y,AmbientColor.z);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);	

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_water);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_grass);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_sand);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES,indexcount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


}

unsigned int ProceduralGen::loadTexture(const char* textureloc)
{
	unsigned int tex;

	if(textureloc != NULL)
	{
		

		data = stbi_load(textureloc,&imageWidth,&imageHeight,&imageFormat, STBI_default);
	
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);

	
		stbi_image_free(data);
	
	}

	return tex;
}


void ProceduralGen::GenerateGrid(unsigned int rows, unsigned int cols)
{
	aoVertices = new Vertex3[rows * cols];

	for(unsigned int i = 0; i < rows; i++)
	{
		for(unsigned int j = 0; j < cols; j++)
		{
			aoVertices[ i * cols + j ].position = vec4((float)j, 0, (float)i, 1);

			//Creating a colour scheme based of something random/ CRAZY!!!
			vec3 Colour = vec3(sinf( (j/ (float)(cols - 1)) * ( i/ (float)(rows - 1))));


			//aoVertices[ i * cols + j].colour = vec4( 0,1,1, 1);
			aoVertices[ i * cols + j].UV = vec2((float)i / rows, (float)j / rows);
		}
	}

	total = rows * cols;

	//Defining index count based of quad counter (2 triangles per quad or 3 Vertexs for a triangle) :P
	auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
	unsigned int indexCount = 0;

	for (unsigned int i = 0; i < (rows - 1); i++)
	{
		for (unsigned int j = 0; j < (cols - 1); j++)
		{
			//Triangle 1 
			auiIndices[indexCount++] = i * cols + j;
			auiIndices[indexCount++] = (i + 1) * cols + j;
			auiIndices[indexCount++] = (i + 1) * cols + (j + 1);

			//Triangle 2
			auiIndices[indexCount++] = i * cols + j;
			auiIndices[indexCount++] = (i + 1 ) * cols + (j + 1);
			auiIndices[indexCount++] = i * cols + (j + 1);
		}
	}




	m_indexCount = indexCount;

	
	indexcount = (rows - 1) * (cols - 1) * 6;

	total = rows * cols;
}