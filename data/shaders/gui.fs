#version 130

in vec2 uv;
in vec4 color;
out vec4 out_Color;

uniform sampler2D tex;

uniform bool has_texture = true;

void main()
{
	if(has_texture)
		out_Color = texture(tex,uv) * color;
	else
		out_Color = color;
}
