#version 130

in vec2 position;
in vec2 UV;
in float lifetime;
in float alpha;

out vec2  uv;
out float Lifetime;
out float Alpha;

uniform float time;
uniform mat4  projection;
uniform	mat4  view;

void main()
{
	uv = UV;
	Lifetime = lifetime;
	Alpha = alpha;
	gl_Position = projection * view * vec4(position, 0.0, 1.0);
}
