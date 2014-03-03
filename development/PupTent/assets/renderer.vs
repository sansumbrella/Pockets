#version 150 core

uniform mat4 ciModelViewProjection;

in vec2 iPosition;
in vec4 iColor;
in vec2 iTexCoord;

out vec4 Color;
out vec2 TexCoord;

void main()
{
	Color = iColor;
	TexCoord = iTexCoord;
	gl_Position = ciModelViewProjection * vec4( iPosition, 0.0, 1.0 );
}