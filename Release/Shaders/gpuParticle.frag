#version 410
in vec4 Colour;
in vec2 texCoord;

in vec4 out_color;

out vec4 fragColour;
uniform sampler2D diffuse;
void main() 
{
	//fragColour = out_color;
	fragColour = Colour;
	//fragColour = texture(diffuse,texCoord);
}