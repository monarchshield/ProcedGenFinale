#version 410
in vec2 frag_texcoord;
in float height;
in vec4 vNormal;
in vec4 vPosition;

out vec4 out_color;
uniform sampler2D diffuse;

uniform sampler2D grass;
uniform sampler2D water;
uniform sampler2D sand;

uniform vec3 LightDir;
uniform vec3 LightColour;
uniform vec3 CameraPos;
uniform float SpecPow;

uniform vec3 AmbientColour;

void main()
{
	vec3 Ambient = AmbientColour * vec3(0.25f,0.25f,0.25f);

	float d = max(0,  dot( normalize(vNormal.xyz), LightDir ) ); 
	vec3 E =  normalize( CameraPos - vPosition.xyz);
	
	vec3 R = reflect( -LightDir, vNormal.xyz);
	
	float s = max(0, dot(E,R) );
	s = pow(s,SpecPow);
	
	out_color = vec4(1,1,1,1);
	
	if(height <= 0.5)
	{
		//out_color = vec4(0,0,1,0) * texture(diffuse, frag_texcoord).rrrr;
		out_color = texture(water, frag_texcoord) * texture(diffuse, frag_texcoord).rrrr;
	}
	if(height >= .5 && height <= .53)
	{
		//out_color = vec4(1,1,0,1) * texture(diffuse, frag_texcoord).rrrr;
		out_color = texture(sand, frag_texcoord) * texture(diffuse, frag_texcoord).rrrr;
	}
	if(height >= 0.53 )
	{
		//out_color = vec4(0,1,0,1) * texture(diffuse, frag_texcoord).rrrr;
		out_color = texture(grass, frag_texcoord) * texture(diffuse, frag_texcoord).rrrr;
	}
	
		//out_color = texture(grass, frag_texcoord);
	//out_color = texture(diffuse, frag_texcoord).rrrr;
	
}