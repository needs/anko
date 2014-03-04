#version 130

in vec2 position;
in vec2 tex_coord;

out vec2 Tex_coord;

uniform mat4 projection;
uniform	mat4 view;

void main()	 
{
	Tex_coord = tex_coord;
	gl_Position = projection * view * vec4(position, 0.0, 1.0);
}
