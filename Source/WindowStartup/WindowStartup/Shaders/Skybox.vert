#version 410
layout(location=0) in vec3 position;
uniform mat4 ProjectionView;
uniform mat4 Projection;
uniform mat4 View;

out vec3 TexCoords;



void main()
{
	gl_Position = Projection * View * vec4(position, 1.0);
    TexCoords = position;
}  