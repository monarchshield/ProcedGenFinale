#include "ObjLoader.h"


ObjLoader::ObjLoader() {}


ObjLoader::ObjLoader(const char *path, const char *textureloc, float Faces)
{
	Facenum = Faces;

	m_vertexCount = 0;
	m_ModelIndexCount = 0;



	Initialise(path, textureloc);
	CreateBuffers();

	
}

ObjLoader::~ObjLoader()
{

}


void ObjLoader::Draw(unsigned int shader, mat4 &ProjectionView)
{
	glUseProgram(shader);




	int loc = glGetUniformLocation(shader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(ProjectionView[0][0]));
	
	
	loc = glGetUniformLocation(shader, "diffuse");
	glUniform1i(loc, 0);


	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_ModelIndexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}




void ObjLoader::Initialise(const char *path, const char *textureloc)
{
	if(textureloc != NULL)
	{
		data = stbi_load(textureloc,&imageWidth,&imageHeight,&imageFormat, STBI_default);
	
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		stbi_image_free(data);

	}

		std::ifstream in(path, std::ios::in);

	if(!in)
	{
		std::cerr << "Cannot open" << "Path" << std::endl;
		exit(1); 
	}

	std::string line;

	while(std::getline(in, line))
	{
		if(line.substr(0,2) == "v ")
		{
			std::istringstream s(line.substr(2));
			Vertex v;

			s >> v.position.x;		 
			s >> v.position.y;		 
			s >> v.position.z;		 
			v.position.w = 1.0f;	 

			v.colour = vec4(1,1,1,1);
			Vertices.push_back(v);
			m_vertexCount++;

		}

		else if(line.substr(0,2) == "vt")
		{
			std::istringstream s(line.substr(2));
			vec2 uv;

			s >> uv.x;
			s >> uv.y;
			uv.y = uv.y * -1;

			ModelUV.push_back(uv);
		}

		else if(line.substr(0,2) == "f " && Facenum == 2.1)
		{
			char dummychar;
			int dummyint;

			std::istringstream s(line.substr(2));
			unsigned int a,b,c;

			s >> a; s >> dummychar;  s >> dummychar; s >> dummyint;
			s >> b; s >> dummychar;  s >> dummychar; s >> dummyint;
			s >> c; s >> dummychar;  s >> dummychar; s >> dummyint;

			a--; b--; c--;
			Elements.push_back(a);
			Elements.push_back(b);
			Elements.push_back(c);

		}

		else if(line.substr(0,2) == "f " && Facenum == 2)
		{
			char dummychar;
			

			std::istringstream s(line.substr(2));
			unsigned int a,b,c,aTex,bTex,cTex;

			s >> a; s >> dummychar; s >> aTex;
			s >> b; s >> dummychar; s >> bTex;
			s >> c; s >> dummychar; s >> cTex;


			a--; b--; c--;
			aTex--; bTex--; cTex--;

			Elements.push_back(a);
			Elements.push_back(b);
			Elements.push_back(c);



			Vertices[a].UV = ModelUV[aTex];
			Vertices[b].UV = ModelUV[bTex];
			Vertices[c].UV = ModelUV[cTex];

			//uv .Y *-1

		}

#pragma region FaceCondition3
		else if(line.substr(0,2) == "f " && Facenum == 3)
		{
			char dummychar;
			int dummyint;

			std::istringstream s(line.substr(2));
			unsigned int a,b,c;

			s >> a; s >> dummychar;  s >> dummyint;  s >> dummychar;  s >> dummyint;
			s >> b; s >> dummychar;  s >> dummyint;  s >> dummychar;  s >> dummyint;
			s >> c; s >> dummychar;  s >> dummyint;  s >> dummychar;  s >> dummyint;

			a--; b--; c--;
			Elements.push_back(a);
			Elements.push_back(b);
			Elements.push_back(c);

		}
#pragma endregion

		else if (line.substr(0, 2) == "f " && Facenum == 3.1f)
		{
			char dummychar;

			std::istringstream s(line.substr(2));
			unsigned int a, b, c, aTex, bTex, cTex, aNormal, bNormal, cNormal;

			s >> a; 
			s >> dummychar; 
			s >> aNormal;
			s >> dummychar;
			s >> aTex;
			
			s >> b;
			s >> dummychar;
			s >> bNormal;
			s >> dummychar;
			s >> bTex;
			
			s >> c;
			s >> dummychar;
			s >> cNormal;
			s >> dummychar;
			s >> cTex;


			a--; b--; c--;
			aTex--; bTex--; cTex--;

			Elements.push_back(a);
			Elements.push_back(b);
			Elements.push_back(c);



			Vertices[a].UV = ModelUV[aTex];
			Vertices[b].UV = ModelUV[bTex];
			Vertices[c].UV = ModelUV[cTex];


		}


		else if( line[0] == '#')
		{
			// Do nothing
		}

		else {/* Also ignore */}
	}


	m_ModelIndexCount = Elements.size();
	normals.resize(Vertices.size(), vec3(0,0,0));

	for (unsigned int i = 0; i < Elements.size(); i+=3)
	{
		GLushort ia =  Elements[i];
		GLushort ib =  Elements[i+1];
		GLushort ic =  Elements[i+2];


		vec3 normal = glm::normalize(glm::cross(vec3(Vertices[ib].position) - vec3(Vertices[ia].position),vec3(Vertices[ic].position) - vec3(Vertices[ia].position)));
	

		normals[ia] = normals[ib] = normals[ic] = normal;
		Vertices[ia].colour = Vertices[ib].colour = Vertices[ic].colour = vec4(normal, 1.0f); 
	}
}

int ObjLoader::loadShader(unsigned int type, const char* path)
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

void ObjLoader::CreateBuffers()
{
	
	// Create VAO, VBO and IBO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	
	// Bind buffers
	glBindVertexArray( m_VAO );
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	
	// send data to the buffers	
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size() * sizeof(unsigned int), &Elements[0], GL_STATIC_DRAW);
	
	
	// describe how the vertices are setup so they can be sent to the shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)*2));
	


	//Setting textur slots 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);


	//tell the shader where it is
	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);
	
	
	
	// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
	// so just rebind it later before using glDrawElements
	glBindVertexArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}