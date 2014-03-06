#version 130

in vec2 position;
in vec2 UV;
out vec2 uv;

void main()
{
	uv = UV;
	gl_Position = vec4(position,0.0,1.0);
}
