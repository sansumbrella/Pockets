#version 150 core


uniform sampler2D uTex0;

in vec4 Color;
in vec2 TexCoord;

out vec4 oColor;

void main()
{
	vec4 color = texture( uTex0, TexCoord );
	oColor = color * Color;
}