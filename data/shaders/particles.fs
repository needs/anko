#version 130

in vec2 uv;
in vec2 Lifetime;
in vec2 Alpha;

out vec4 out_Color;

uniform sampler2D tex;
uniform float time;

void main()
{
	float curtime = (time - Lifetime.x) / Lifetime.y;

	if (Lifetime.x + Lifetime.y < time)
		out_Color = vec4(0.0, 0.0, 0.0, 0.0);
	else
		out_Color = texture(tex, uv) * vec4(1.0, 1.0, 1.0, Alpha.x + (Alpha.y - Alpha.x) * curtime);
}
