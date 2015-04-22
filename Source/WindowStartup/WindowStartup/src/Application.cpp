#include "Application.h"
//#define STB_IMAGE_IMPLEMENTATION
#include "stb-master\stb_image.h"

#include "ProceduralGen.h"



void OnMouseButton(GLFWwindow*, int b, int a, int m) 
{
	TwEventMouseButtonGLFW(b, a);
}
void OnMousePosition(GLFWwindow*, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}
void OnMouseScroll(GLFWwindow*, double x, double y) 
{
	TwEventMouseWheelGLFW((int)y);
}
void OnKey(GLFWwindow*, int k, int s, int a, int m) 
{
	TwEventKeyGLFW(k, a);
}
void OnChar(GLFWwindow*, unsigned int c)
{
	TwEventCharGLFW(c, GLFW_PRESS);
}
void OnWindowResize(GLFWwindow*, int w, int h)
{
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}

Application::Application()
{
	if(glfwInit() == false)
		return;

	window = glfwCreateWindow(1280,720,"Computer Graphics" , nullptr,nullptr);
	camera = new FlyCamera();
	previousTime = 0;
	currentTime = 0;
	DeltaTime = 0;
	m_rows = 20;
	m_collumns = 20;
	m_ModelIndexCount = 0;
	
	m_startcolour = vec4(1,1,1,1);
	m_endcolour = vec4(1,0,0,1);
	

	m_vertexCount = 0;


	m_timer = 0;
	if(window == nullptr)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	if(ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwMakeContextCurrent(window);
		glfwDestroyWindow(window);
		glfwTerminate();
		return;
	}
	m_GPUParticles = new GPUParticleEmitter();
	m_GPUParticles->initialise(100000, 0.1f, 5.0f, 5, 20, 1, 0.1f, vec4(1, 0, 0, 1),vec4(1, 1, 0, 1));
	m_clearcolour = vec4(1,1,1,1);
	m_skybox = new Skybox();


	m_EnemyElite = new FbxLoader("./Resources/EnemyElite/EnemyElite.fbx",vec4(-25, 0, 0,0));
	m_Pyro = new FbxLoader("./Resources/Pyro/pyro.fbx", vec4(25,0,0,0));
	m_Tree = new FbxLoader("./Resources/Tree/PineTree.fbx", vec4(0,20,0,0));
	m_terrain = new ProceduralGen();


	Gizmos::create();
	
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);



	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280,720);

	m_LightDirection = vec3(0.72,0.03,-0);
	m_AmbientColour =  vec3(0,0,0);
	m_seed = 0;
	m_amp = 1.0f;
	m_persis = 0.3f;

	m_bar = TwNewBar("Debug Console");
	TwAddVarRW(m_bar, "Start colour", TW_TYPE_COLOR4F, &m_startcolour, "");
	TwAddVarRW(m_bar, "End colour", TW_TYPE_COLOR4F, &m_endcolour, "");

	TwAddSeparator(m_bar, "Procedural Gen:", "");
	TwAddVarRW(m_bar, "Seed", TW_TYPE_FLOAT, &m_seed, "");
	TwAddVarRW(m_bar, "Scale", TW_TYPE_UINT16, &m_terrain->scalesize, "");
	TwAddVarRW(m_bar, "Amplitude", TW_TYPE_FLOAT, &m_amp, "");
	TwAddVarRW(m_bar, "Persistence", TW_TYPE_FLOAT, &m_persis, "");

	TwAddSeparator(m_bar, "Camera position:", "");
	TwAddVarRW(m_bar, "X position: ", TW_TYPE_FLOAT, &camera->xPosition, "");
	TwAddVarRW(m_bar, "y position: ", TW_TYPE_FLOAT, &camera->yPosition, "");
	TwAddVarRW(m_bar, "z position: ", TW_TYPE_FLOAT, &camera->zPosition, "");
	TwAddVarRW(m_bar, "Camera Speed: ", TW_TYPE_FLOAT, &camera->Speed, "");

	TwAddSeparator(m_bar, "Light Settings:", "");
	TwAddVarRW(m_bar, "Light Direction: ", TW_TYPE_DIR3F, &m_LightDirection, "");
	TwAddVarRW(m_bar, "Ambient Colour: ", TW_TYPE_COLOR3F, &m_AmbientColour, "");
}

Application::~Application()
{
	glfwDestroyWindow(window);
	Gizmos::destroy();
	glfwTerminate();
	TwDeleteAllBars();
	TwTerminate();


}

void Application::RunGame()
{

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) 
	{
		
		
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glClearColor(0.25f,0.25f,0.25f,1);

		Update();
		Draw();

		////Update Loop Put all Crap here
		//currentTime = (float)glfwGetTime();
		//DeltaTime = currentTime - previousTime;
		//previousTime = currentTime;
		TwDraw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}	
}

void Application::Update()
{
	currentTime = (float)glfwGetTime();
	DeltaTime = currentTime - previousTime;
	previousTime = currentTime;
	m_timer += DeltaTime;

	if(glfwGetKey(window, GLFW_KEY_R)) {
		m_terrain->GenerateTexture(m_seed,m_amp,m_persis);
	}

	if (glfwGetKey(window, GLFW_KEY_T)) {
		m_GPUParticles->CreateUpdateShader();
	}



	m_GPUParticles->GUIEndcolourchanger(m_endcolour);
	m_GPUParticles->GUIStartcolourchanger(m_startcolour);


	camera->update(DeltaTime);
	camera->getWorldTransform();
}

void Application::Draw()
{
	
	m_skybox->Draw(camera);
	m_GPUParticles->Draw(currentTime, camera->getWorldTransform(), camera->getProjectionTransform());
	m_EnemyElite->Draw(camera, m_LightDirection,m_AmbientColour);
	m_Pyro->Draw(camera, m_LightDirection,m_AmbientColour);
	m_terrain->Draw(camera, m_LightDirection,m_AmbientColour);
	m_Tree->Draw(camera, m_LightDirection, m_AmbientColour);
	
}

void Application::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* aoVertices = new Vertex[rows * cols];

	for(unsigned int i = 0; i < rows; i++)
	{
		for(unsigned int j = 0; j < cols; j++)
		{
			aoVertices[ i * cols + j ].position = vec4((float)j, 0, (float)i, 1);

			//Creating a colour scheme based of something random/ CRAZY!!!
			vec3 Colour = vec3(sinf( (j/ (float)(cols - 1)) * ( i/ (float)(rows - 1))));


			aoVertices[ i * cols + j].colour = vec4( 0,1,1, 1);

		}
	}

	//Defining index count based of quad counter (2 triangles per quad or 3 Vertexs for a triangle) :P
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
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

	// Create VAO, VBO and IBO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	// Bind buffers
	glBindVertexArray( m_VAO );
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	// send data to the buffers	
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	// describe how the vertices are setup so they can be sent to the shader
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)*2));

	// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
	// so just rebind it later before using glDrawElements
	glBindVertexArray(0);


	delete[] auiIndices;
	delete[] aoVertices;
}

void Application::CreateShaders()
{
	const char* vsSource = "#version 410\n \
			layout(location=0) in vec4 Position; \
			layout(location=1) in vec4 Colour; \
			layout(location=2) in vec2 UV; \
			out vec4 vColour; \
			out vec2 vTexCoord; \
			uniform mat4 ProjectionView; \
			uniform float time; \
			void main() { vTexCoord = UV; vColour = Colour; gl_Position = ProjectionView * Position;}";
	
	const char* fsSource = "#version 410\n \
				in vec4 vColour; \
				in vec2 vTexCoord; \
				out vec4 FragColor; \
				uniform sampler2D diffuse;\
				void main() {  FragColor = texture(diffuse, vTexCoord); }";
	




	//vsSource = "#version 410\n \
	//		layout(location=0) in vec4 Position; \
	//		layout(location=1) in vec4 Colour; \
	//		layout(location=2) in vec2 UV; \
	//		out vec4 vColour; \
	//		out vec2 vTexCoord; \
	//		uniform mat4 ProjectionView; \
	//		uniform float time; \
	//		void main() { vTexCoord = UV; vColour = Colour; gl_Position = ProjectionView * Position;}";
	//
	//
	//fsSource = "#version 410\n \
	//			in vec4 vColour; \
	//			in vec2 vTexCoord; \
	//			out vec4 FragColor; \
	//			uniform sampler2D diffuse;\
	//			void main() {  FragColor = texture(diffuse, vTexCoord); }";
	//texture(diffuse,vTexCoord);
	//FragColor = vColour;
	//vsSource = "#version 410\n \
	//			layout(location=0) in vec4 Position; \
	//			layout(location=1) in vec2 TexCoord; \
	//			out vec2 vTexCoord; \
	//			uniform mat4 ProjectionView; \
	//			uniform float time;\
	//			void main() { \
	//			vTexCoord = TexCoord * sin(time);  \
	//			gl_Position= ProjectionView * Position;\
	//			}";
	//
	//
	//fsSource = "#version 410\n \
	//			in vec2 vTexCoord; \
	//			out vec4 FragColor; \
	//			uniform sampler2D diffuse; \
	//			void main() { \
	//			FragColor = texture(diffuse,vTexCoord);\
	//			}";



	success = GL_FALSE;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader,1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	
	int loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc,0);

	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	if(success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(m_programID,infoLogLength,0,infoLog);
		printf("Error: failed to link shader program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

//bool Application::loadObj(const char *path, const char *textureloc, vec4 SetPosition,  std::vector< Vertex > &Vertices, std::vector< vec2 > & uvs, std::vector< vec3 > & normals , std::vector< unsigned int> Elements, float Facenum)
//{	
//	int imageWidth, imageHeight, imageFormat;
//
//	if(textureloc != NULL)
//	{
//		data = stbi_load(textureloc,&imageWidth,&imageHeight,&imageFormat, STBI_default);
//
//		glGenTextures(1, &m_texture);
//		glBindSampler(GL_TEXTURE_2D, m_texture);
//
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//
//	}
//
//
//	std::ifstream in(path, std::ios::in);
//
//	if(!in)
//	{
//		std::cerr << "Cannot open" << "Path" << std::endl;
//		exit(1); 
//	}
//
//	std::string line;
//
//	while(std::getline(in, line))
//	{
//		if(line.substr(0,2) == "v ")
//		{
//			std::istringstream s(line.substr(2));
//			Vertex v;
//
//			v.position  += SetPosition;
//
//			s >> v.position.x;		 
//			s >> v.position.y;		 
//			s >> v.position.z;		 
//			v.position.w = 1.0f;	 
//
//			v.colour = vec4(1,1,1,1);
//			Vertices.push_back(v);
//			m_vertexCount++;
//
//		}
//
//		else if(line.substr(0,2) == "vt")
//		{
//			std::istringstream s(line.substr(2));
//			vec2 uv;
//
//			s >> uv.x;
//			s >> uv.y;
//
//			uv.y = uv.y * -1;
//
//			ModelUV.push_back(uv);
//
//		}
//
//		else if(line.substr(0,2) == "f " && Facenum == 2.1)
//		{
//			char dummychar;
//			int dummyint;
//
//			std::istringstream s(line.substr(2));
//			unsigned int a,b,c;
//
//			s >> a; s >> dummychar;  s >> dummychar; s >> dummyint;
//			s >> b; s >> dummychar;  s >> dummychar; s >> dummyint;
//			s >> c; s >> dummychar;  s >> dummychar; s >> dummyint;
//
//			a--; b--; c--;
//			Elements.push_back(a);
//			Elements.push_back(b);
//			Elements.push_back(c);
//
//			
//			
//
//		}
//
//		else if(line.substr(0,2) == "f " && Facenum == 2)
//		{
//			char dummychar;
//			
//
//			std::istringstream s(line.substr(2));
//			unsigned int a,b,c,aTex,bTex,cTex;
//
//			s >> a; s >> dummychar; s >> aTex;
//			s >> b; s >> dummychar; s >> bTex;
//			s >> c; s >> dummychar; s >> cTex;
//
//
//			a--; b--; c--;
//			aTex--; bTex--; cTex--;
//
//			Elements.push_back(a);
//			Elements.push_back(b);
//			Elements.push_back(c);
//
//
//
//			Vertices[a].UV = ModelUV[aTex];
//			Vertices[b].UV = ModelUV[bTex];
//			Vertices[c].UV = ModelUV[cTex];
//
//			//uv .Y *-1
//
//		}
//
//
//		else if(line.substr(0,2) == "f " && Facenum == 3)
//		{
//			char dummychar;
//			int dummyint;
//
//			std::istringstream s(line.substr(2));
//			unsigned int a,b,c;
//
//			s >> a; s >> dummychar;  s >> dummyint;  s >> dummychar;  s >> dummyint;
//			s >> b; s >> dummychar;  s >> dummyint;  s >> dummychar;  s >> dummyint;
//			s >> c; s >> dummychar;  s >> dummyint;  s >> dummychar;  s >> dummyint;
//
//			a--; b--; c--;
//			Elements.push_back(a);
//			Elements.push_back(b);
//			Elements.push_back(c);
//
//		}
//
//
//
//		else if( line[0] == '#')
//		{
//			// Do nothing
//		}
//
//		else {/* Also ignore */}
//	}
//
//
//	m_ModelIndexCount = Elements.size();
//
//	normals.resize(Vertices.size(), vec3(0,0,0));
//
//	for (unsigned int i = 0; i < Elements.size(); i+=3)
//	{
//		GLushort ia =  Elements[i];
//		GLushort ib =  Elements[i+1];
//		GLushort ic =  Elements[i+2];
//
//		vec3 normal = glm::normalize(glm::cross(vec3(Vertices[ib].position) - vec3(Vertices[ia].position),
//			vec3(Vertices[ic].position) - vec3(Vertices[ia].position)));
//
//
//		normals[ia] = normals[ib] = normals[ic] = normal;
//		Vertices[ia].colour = Vertices[ib].colour = Vertices[ic].colour = vec4(normal, 1.0f); 
//	}
//
//
//
//
//	// Create VAO, VBO and IBO
//	glGenVertexArrays(1, &m_VAO);
//	glGenBuffers(1, &m_VBO);
//	glGenBuffers(1, &m_IBO);
//
//	// Bind buffers
//	glBindVertexArray( m_VAO );
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
//
//	// send data to the buffers	
//	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Elements.size() * sizeof(unsigned int), &Elements[0], GL_STATIC_DRAW);
//	
//
//	// describe how the vertices are setup so they can be sent to the shader
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glEnableVertexAttribArray(2);
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)*2));
//
//	// tell the shader where it is
//	loc = glGetUniformLocation(m_programID, "diffuse");
//	glUniform1i(loc, 0);
//	
//
//
//	// m_VAO hold all our ARRAY_BUFFER and ARRAY_ELEMENT_BUFFER settings
//	// so just rebind it later before using glDrawElements
//	glBindVertexArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//
//
//
//	return true;
//};

void Application::createOpenGLBuffers(FBXFile* fbx)
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{

		FBXMeshNode* mesh = fbx->getMeshByIndex(i);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);



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
		glEnableVertexAttribArray(2); // Tangents
		glEnableVertexAttribArray(3); // UV
		glEnableVertexAttribArray(4); // Weight
		glEnableVertexAttribArray(5); // Indices
		
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex),((char*)0) + FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::TangentOffset);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex),((char*)0) + FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		mesh->m_userData = glData;
		}
}

void Application::CreateShaderFBX()
{
	
	const char* vsSource = "#version 410\n																														\
							layout(location=0) in vec4 Position;																								\
							layout(location=1) in vec4 Normal;																									\
							layout(location=2) in vec4 Tangent;																									\
							layout(location=3) in vec2 TexCoord;																								\
							layout(location=4) in vec4 Weights;																									\
							layout(location=5) in vec4 Indices;																									\
																																								\
							out vec3 vPosition;																													\
							out vec2 vTexCoord;																													\
							out vec3 vNormal;																													\
							out vec3 vTangent;																													\
							out vec3 vBiTangent;																												\
																																								\
							uniform mat4 ProjectionView;																										\
							uniform mat4 global;																												\
																																								\
							const int MAX_BONES = 128;																											\
							uniform mat4 bones[MAX_BONES];																										\
																																								\
							void main()																															\
							{                                                                                                                                   \
								vPosition = Position.xyz;																										\
								vTexCoord = TexCoord;																											\
								vNormal = Normal.xyz;																											\
								vTangent = Tangent.xyz;																											\
								vBiTangent = cross(vNormal, vTangent);																							\
								gl_Position = ProjectionView * Position;																						\
																																								\
								ivec4 index = ivec4(Indices);																									\
																																								\
								vec4 P = bones[index.x]*Position*Weights.x;																						\
								P += bones[index.y]*Position*Weights.y;																							\
								P += bones[index.z]*Position*Weights.z;																							\
								P += bones[index.w]*Position*Weights.w;																							\
																																								\
								gl_Position = ProjectionView * global * P;																						\
								}";

	const char* fsSource = "#version 410\n																														\
							in vec2 vTexCoord;																													\
							in vec3 vNormal;																													\
							in vec3 vTangent;																													\
							in vec3 vBiTangent;																													\
							out vec4 FragColor;																													\
							uniform vec3 LightDir;																												\
							uniform sampler2D diffuse;																											\
							uniform sampler2D normal;																											\
							void main()																															\
							{																																	\
								mat3 TBN = mat3(normalize( vTangent ), normalize( vBiTangent ), normalize( vNormal ));											\
								vec3 N = texture(normal, vTexCoord).xyz * 2 - 1;																				\
								float d = max( 0, dot( normalize( TBN * N ), normalize( -LightDir )));															\
								FragColor = texture(diffuse, vTexCoord);																						\
							}";




   

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Application::loadFbx(const char *path)
{
	m_fbx = new FBXFile();
	m_fbx->load(path,FBXFile::UNITS_METER,true,true,true);

	m_fbx->initialiseOpenGLTextures();
	
	createOpenGLBuffers(m_fbx);

	//Sending Info VAO, VBO, IBO Shaders
	

}

