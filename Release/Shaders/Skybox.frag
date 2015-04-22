#version 410
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{    	
    color = texture(skybox, TexCoords);
	 //color = vec4(1,1,1,1);
}