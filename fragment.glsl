#version 330 core

in vec4 frag_col; // id = 11
in vec3 frag_uvs; // id = 12

out vec4 color; // id = 13

uniform sampler2DArray test_texture;
void main()
{
	vec4 texel = texture(test_texture, frag_uvs);
	color = texel;
};
