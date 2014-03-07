#version 130

in vec2 uv;
out vec4 out_Color;

uniform sampler2D tex;

uniform bool has_texture = true;
uniform vec4 color = vec4(1,1,1,1);

void main()
{
	if(has_texture)
		out_Color = texture(tex,uv) * color;
	else
		out_Color = color;
}
