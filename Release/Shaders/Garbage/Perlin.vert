#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 colour;
layout(location=2) in vec3 normal;
layout(location=3) in vec2 texcoord;


uniform mat4 ProjectionView;
out vec2 frag_texcoord;
out vec4 vNormals;
out vec4 vPosition;
out float height;

uniform sampler2D diffuse;

void main()
{
	vPosition = position;
	vNormals = normal;
	vec4 pos = position;
	pos.y += texture(diffuse, texcoord).r * 5;

	height = texture(diffuse, texcoord).r ;
	
	frag_texcoord = texcoord;
	gl_Position = ProjectionView * pos;
}
