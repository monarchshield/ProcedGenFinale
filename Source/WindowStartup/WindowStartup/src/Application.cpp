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
m_shaderloader = new ShaderLoader();


#pragma region Particle Emitter
	//m_GPUParticles = new GPUParticleEmitter();
	//m_GPUParticles->initialise(100000, 0.1f, 5.0f, 5, 20, 1, 0.1f, vec4(1, 0, 0, 1),vec4(1, 1, 0, 1));
#pragma endregion

#pragma region FBXFILES + Terrain
	//m_EnemyElite = new FbxLoader("./Resources/EnemyElite/EnemyElite.fbx",vec4(-25, 0, 0,0));
	//m_Pyro = new FbxLoader("./Resources/Pyro/pyro.fbx", vec4(25,0,0,0));
	//m_Tree = new FbxLoader("./Resources/Tree/PineTree.fbx", vec4(0,20,0,0));
	//m_terrain = new ProceduralGen();
#pragma endregion

#pragma region Skybox
	//m_skybox = new Skybox();
#pragma endregion

#pragma region ObjModels

m_objModel = new ObjLoader("./Resources/beretta.obj", "./Resources/REOberetta.png", 2);

#pragma endregion


	m_clearcolour = vec4(1,1,1,1);

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



	//m_GPUParticles->GUIEndcolourchanger(m_endcolour);
	//m_GPUParticles->GUIStartcolourchanger(m_startcolour);


	camera->update(DeltaTime);
	camera->getWorldTransform();
}

void Application::Draw()
{

#pragma region OBJ FILES
	m_objModel->Draw(m_shaderloader->UseFlatLitShader(), camera->getProjectionTransform());
#pragma endregion


#pragma region SkyBox
	//m_skybox->Draw(camera);
#pragma endregion

#pragma region FBXFILES
	//m_Pyro->Draw(camera, m_LightDirection,m_AmbientColour);
	//m_EnemyElite->Draw(camera, m_LightDirection,m_AmbientColour);
	//m_Tree->Draw(camera, m_LightDirection, m_AmbientColour);
	#pragma endregion

#pragma region GPUParticles + Terrain
	//m_GPUParticles->Draw(currentTime, camera->getWorldTransform(), camera->getProjectionTransform());
	//m_terrain->Draw(camera, m_LightDirection,m_AmbientColour);
#pragma endregion
	
}




