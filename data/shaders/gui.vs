#version 130

in vec2 position;
in vec2 UV;
in vec4 Color;

out vec2 uv;
out vec4 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	uv = UV;
	color = Color;
	gl_Position = projection * vec4(position, 0.0,1.0);
}
