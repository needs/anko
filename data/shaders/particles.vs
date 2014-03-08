#version 130

in vec2 position;
in vec2 UV;
in vec2 lifetime;
in vec2 alpha;
in vec4 box;

out vec2 uv;
out vec2 Lifetime;
out vec2 Alpha;

uniform float time;
uniform mat4  projection;
uniform	mat4  view;

void main()
{
	float curtime = (time - Lifetime.x) / Lifetime.y;

	uv = UV;
	Lifetime = lifetime;
	Alpha = alpha;
	gl_Position = projection * view * vec4(position.x, position.y, 0.0, 1.0);
}
