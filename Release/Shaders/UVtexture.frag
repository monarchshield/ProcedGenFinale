#version 410
in vec4 vColour; 
in vec2 vTexCoord; 
out vec4 FragColor; 
in vec4 vNormal;
in vec4 vPosition;

//uniform vec3 LightDir;
//uniform vec3 LightColour;
//uniform vec3 CameraPos;
//uniform float SpecPow;


uniform sampler2D diffuse;
void main() 
{ 

	//float d = max(0, \ dot( normalize(vNormal.xyz), LightDir ) ); 
	//vec3 E =  normalize( CameraPos - vPosition.xyz);
	//
	//vec3 R = reflect( -LightDir, vNormal.xyz);
	//
	//float s = max(0, dot(E,R) );
	//s = pow(s,SpecPow);
	//
	//* vec4(LightColour * d + LightColour * s, 1)
	FragColor = texture(diffuse, vTexCoord);
	//FragColor = vec4(1,1,1,1);
	
	//FragColor = vColour;
}