#version 410
in vec2 vTexCoord; 
in vec4 vNormal;
out vec4 FragColor; 
uniform sampler2D diffuse; 
void main() 
{ 
	//FragColor = vNormal;
	FragColor = texture(diffuse,vTexCoord);
};