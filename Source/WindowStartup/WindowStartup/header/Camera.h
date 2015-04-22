//Author: Anthony Bogli
//Description: Creating a camera class for a look at Matrix

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Gizmos.h>
#include <GLFW\glfw3.h>

using glm::mat4;
using glm::vec3;
using glm::vec4;

class Camera
{
public:
	Camera();
	~Camera();

	void update(float deltatime);
	void setPerspective(float fieldofView, float aspectRatio, float near, float far);
	void setLookAt(vec3 from, vec3 to, vec3 up);
	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjectionTransform(); // Get the Projection view transfor (Projection view * view matrix)
	mat4 getProjectionView();
	void updateProjectionMatrix();



private:
	float xPosition;
	float yPosition;
	float zPosition;

	vec3 cameraPosition;
	vec3 cameraDirection;

	mat4 viewmatrix;	// The view Matrix of the camera which is the inverse of the world transform
	
	mat4 worldTransform; //The world transform of the camera

	mat4 ProjectionView; // The base Project View
	mat4 ProjectionViewTransform; //The Project * by the view matrix

	//mat4 worldTransform;


};
#endif
