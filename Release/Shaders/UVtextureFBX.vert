#version 410
layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Colour; 
layout(location=2) in vec2 UV; 
layout(location=3) in vec4 Normal; 

out vec4 vNormal;
out vec4 vColour; 
out vec2 vTexCoord; 
out vec4 vPosition;


uniform mat4 ProjectionView; 
uniform mat4 global; 
uniform vec4 WorldPosition;
uniform float time; 

void main() 
{ 

	vPosition = Position;
	vNormal = Normal;
	vTexCoord = UV; 
	vColour = Colour; 
	gl_Position = ProjectionView * (global * Position + WorldPosition);  
	
}