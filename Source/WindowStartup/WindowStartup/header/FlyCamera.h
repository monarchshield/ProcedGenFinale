//Author: Anthony Bogli
//Description: Creating a camera class for a look at Matrix

#ifndef FLYCAMERA_H
#define FLYCAMERA_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Gizmos.h>
#include <GLFW\glfw3.h>

using glm::vec2;
using glm::mat4;
using glm::vec3;
using glm::vec4;

class FlyCamera
{
public:
	FlyCamera();
	~FlyCamera();

	void update(float deltatime);
	void setPerspective(float fieldofView, float aspectRatio, float near, float far);
	void setLookAt(vec3 from, vec3 to, vec3 up);
	mat4 getWorldTransform();
	mat4 getView();
	mat4 getProjectionTransform(); // Get the Projection view transfor (Projection view * view matrix)
	mat4 getProjectionView();
	vec3 getPosition();

	void updateProjectionMatrix();

	float xPosition;	//Floats holding axis positions
	float yPosition;	//Floats holding axis positions
	float zPosition;	//Floats holding axis positions
	float Speed;	//Used for the camera speed


private:
	


	vec3 right;	//Move right
	vec3 up;	//move up
	vec3 forward; //move forward

	vec3 cameraPosition;	//a Vec3 Version of my CameraPosition
	vec3 cameraDirection;	//A Vector3 Direction for moving my Camera directly in front

	mat4 viewmatrix;	// The view Matrix of the camera which is the inverse of the world transform
	
	mat4 worldTransform; //The world transform of the camera

	mat4 ProjectionView; // The base Project View
	mat4 ProjectionViewTransform; //The Project * by the view matrix

	//mat4 worldTransform;


};
#endif
