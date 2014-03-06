#version 130

in vec2 Tex_coord;
out vec4 color;

uniform sampler2D tex;

void main()
{
	color = texture(tex, Tex_coord);
}