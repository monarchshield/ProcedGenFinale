#version 410
layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord; 

out vec2 vTexCoord; 
out vec4 vNormal;


uniform mat4 ProjectionView; 
uniform float time;
void main() 
{ 
	
	vNormal = Normal;
	vTexCoord = TexCoord;  
	gl_Position= ProjectionView * Position;
}