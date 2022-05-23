#pragma once

/** @file */

#include <string>
#include <string_view>

namespace glsl
{
	bool opengl_validate_vertex_shader_glsl(std::string_view _source, std::string* _errorOut = nullptr);
	bool opengl_validate_fragment_shader_glsl(std::string_view _source, std::string* _errorOut = nullptr);
	bool opengl_validate_vertex_fragment_glsl_program(std::string_view _vertSource, std::string_view _fragSource, std::string* _errorOut = nullptr);
};
