#version 130

in vec2 uv;
in vec2 Lifetime;
in vec2 Alpha;

out vec4 out_Color;

uniform sampler2D tex;
uniform float time;

void main()
{
	float Curtime = (time - Lifetime.x) / Lifetime.y;

	if (Curtime > 1.0)
		out_Color = vec4(0.0, 0.0, 0.0, 0.0);
	else
		out_Color = texture(tex, uv) * vec4(1.0, 1.0, 1.0, mix(Alpha.x, Alpha.y, Curtime));
}
