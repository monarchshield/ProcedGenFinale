#include "Camera.h"


Camera::Camera()
{
	xPosition = 10;
	yPosition = 10;
	zPosition = 10;

	cameraPosition = vec3(xPosition,yPosition,zPosition);
	cameraDirection = vec3(0,0,0);
	


}

Camera::~Camera()
{

}

void Camera::update(float deltatime)
{
	if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A) == GLFW_PRESS)
	{
		xPosition += .1f;
	}

	if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D) == GLFW_PRESS)
	{
		xPosition -= .1f;
	}

	if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W) == GLFW_PRESS)
	{
		zPosition -= .1f;
	}

	if(glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S) == GLFW_PRESS)
	{
		zPosition += .1f;
	}

	
	viewmatrix = glm::lookAt(vec3(xPosition,yPosition,zPosition), vec3(0), vec3(0,1,0));
	ProjectionView = glm::perspective(glm::pi<float>() * 0.25f, 16/9.f, 0.1f, 1000.f);
}

void Camera::setPerspective(float fieldofView, float aspectRatio, float near, float far)
{
	
}

void Camera::setLookAt(vec3 from, vec3 to, vec3 up)
{
	viewmatrix = glm::lookAt(from, to, up);
}

mat4 Camera::getWorldTransform()
{
		
	worldTransform = glm::inverse(viewmatrix);
	return worldTransform;
}


mat4 Camera::getView()
{
	return viewmatrix;
}

mat4 Camera::getProjectionTransform()
{
	ProjectionViewTransform = ProjectionView * viewmatrix;
	return ProjectionViewTransform;
}

mat4 Camera::getProjectionView()
{
	return ProjectionView;
}

//void Camera::updateProjectionMatrix()
//{
//	
//}