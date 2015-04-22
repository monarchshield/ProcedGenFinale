#version 410
layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Colour; 
layout(location=2) in vec2 UV; 
layout(location=3) in vec4 Normal;




out vec4 vPosition;
out vec4 vNormal;
out vec4 vColour; 
out vec2 vTexCoord; 

uniform vec3 WorldPosition;
uniform mat4 ProjectionView; 
uniform mat4 global; 

uniform float time; 

void main() 
{ 
	vPosition = Position;
	vNormal = Normal;
	vTexCoord = UV; vColour = Colour; 
	gl_Position = ProjectionView * Position;
}