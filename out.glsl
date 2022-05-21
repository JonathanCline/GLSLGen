#version 69 core

in vec3 in_pos; // id = 9
in vec2 in_uvs; // id = 10
in vec4 in_col; // id = 11

out vec3 out_uvs; // id = 12
out vec4 out_col; // id = 13

void ()
{
	out_uvs = vec3(in_uvs.xy, 0.0);
	out_col = in_col;
	gl_Position = vec4(in_pos.xyz, 1.0);
};
