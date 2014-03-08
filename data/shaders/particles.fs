#version 130

in vec2 uv;
in float Lifetime;
in float Alpha;

out vec4 out_Color;

uniform sampler2D tex;
uniform float time;

void main()
{
	if (Lifetime < time)
		out_Color = vec4(0.0, 0.0, 0.0, 0.0);
	else
		out_Color = texture(tex, uv) * vec4(1.0, 1.0, 1.0, Alpha);
}
