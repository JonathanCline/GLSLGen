#version 330 core

in vec3 in_pos; // id = 10
in vec2 in_uvs; // id = 11
in vec4 in_col; // id = 12

out vec2 frag_uvs; // id = 13
out vec4 frag_col; // id = 14

void main()
{
	frag_uvs = in_uvs;
	frag_col = in_col;
	gl_Position = vec4(in_pos.xyz, 1.0);
};
