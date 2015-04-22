#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 colour;
layout(location=2) in vec2 texcoord;
layout(location=3) in vec4 Normal;

out vec4 vNormal;
out vec4 vPosition;

uniform mat4 ProjectionView;
out vec2 frag_texcoord;
out float height;

uniform sampler2D diffuse;

void main()
{
	vNormal = Normal;
	vPosition = position;


	vec4 pos = position;
	pos.y += texture(diffuse, texcoord).r * 5;
	pos.x -= 32;
	pos.z -= 32;
	pos.y -= 5;
	height = texture(diffuse, texcoord).r ;
	
	frag_texcoord = texcoord;
	gl_Position = ProjectionView * pos;
}
