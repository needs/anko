#version 130

in vec3 position;
in vec2 UV;

out vec2 uv;

uniform mat4 projection;
uniform	mat4 view;

void main()
{
	uv = UV;
	gl_Position = projection * view * vec4(position, 1.0);
}
