#version 130

in vec2 uv;
out vec4 color;

uniform sampler2D tex;
uniform float time;
uniform int effect;

const float PI = 3.1415926535;

float rand(vec2 v)
{
    return fract(sin(dot(v.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 noise()
{
	return texture(tex,uv)/3+ texture(tex,uv)*vec4(vec3(rand(uv*mod(time,10))),1.0);
}

vec4 invert()
{
	return vec4(1-texture(tex,uv).rgb,1);
}

vec4 greyscale()
{
	vec3 texcol = texture(tex,uv).rgb;
	float gray = (texcol.r + texcol.g + texcol.b)/3;
	return vec4(vec3(gray), 1);
}

vec4 desaturate()
{
	return mix(texture(tex,uv), greyscale(), 0.7);
}

vec4 fisheye()
{
	float aperture = 200.0;
	float apertureHalf = 0.5 * aperture * (PI / 180.0);
	float maxFactor = sin(apertureHalf);
	vec2 new_uv;
	vec2 xy = 2.0 * uv.xy - 1.0;
	float d = length(xy);
	if (d < (2.0-maxFactor))
	{
		d = length(xy * maxFactor);
		float z = sqrt(1.0 - d * d);
		float r = atan(d, z) / PI;
		float phi = atan(xy.y, xy.x);
    
		new_uv.x = r * cos(phi) + 0.5;
		new_uv.y = r * sin(phi) + 0.5;
	}
	else
	{
		new_uv = uv.xy;
	}
	return texture(tex,new_uv);
}


void main()
{
	switch(effect)
	{
	case 0:
		color = texture(tex,uv);
		break;
	case 1:
		color = greyscale();
		break;
	case 2:
		color = desaturate();
		break;
	case 3:
		color = invert();
		break;
	case 4:
		color = noise();
		break;
	case 5:
		color = fisheye();
	}
}
