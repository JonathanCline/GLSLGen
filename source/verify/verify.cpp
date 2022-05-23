#include "verify.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cppopengl/cppopengl.hpp>

namespace glsl
{
	template <typename OpT>
	inline auto validate_opengl(OpT _op)
	{
		if (!glfwInit())
		{
			abort();
		};

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		auto _window = glfwCreateWindow(40, 40, "foo", nullptr, nullptr);

		glfwMakeContextCurrent(_window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			abort();
		};

		const auto ctx = gl::context();

		auto o = _op(ctx);

		glfwDestroyWindow(_window);
		glfwTerminate();

		return o;
	};
	



	inline bool validate_shader_glsl(std::string_view _source, gl::shader_type _type, std::string* _errorOut)
	{
		return validate_opengl([&](const gl::context& ctx)
			{
				auto _shader = gl::new_shader(ctx, _type);
				if (!gl::compile_shader(ctx, _shader, _source))
				{
					if (_errorOut)
					{
						*_errorOut = gl::get_info_log(ctx, _shader);
					};
					return false;
				};
				return true;
			});
	};

	bool opengl_validate_vertex_shader_glsl(std::string_view _source, std::string* _errorOut)
	{
		return validate_shader_glsl(_source, gl::shader_type::vertex, _errorOut);
	};
	bool opengl_validate_fragment_shader_glsl(std::string_view _source, std::string* _errorOut)
	{
		return validate_shader_glsl(_source, gl::shader_type::fragment, _errorOut);
	};
	bool opengl_validate_compute_shader_glsl(std::string_view _source, std::string* _errorOut)
	{
		return validate_shader_glsl(_source, gl::shader_type::compute, _errorOut);
	};



	bool opengl_validate_vertex_fragment_glsl_program(std::string_view _vertSource, std::string_view _fragSource, std::string* _errorOut)
	{
		return validate_opengl([&](const gl::context& ctx)
			{
				auto _vert = gl::new_shader(ctx, gl::shader_type::vertex);
				if (!gl::compile_shader(ctx, _vert, _vertSource))
				{
					if (_errorOut)
					{
						*_errorOut = gl::get_info_log(ctx, _vert);
					};
					return false;
				};

				auto _frag = gl::new_shader(ctx, gl::shader_type::fragment);
				if (!gl::compile_shader(ctx, _frag, _fragSource))
				{
					if (_errorOut)
					{
						*_errorOut = gl::get_info_log(ctx, _frag);
					};
					return false;
				};

				auto _prog = gl::new_program(ctx);
				gl::attach(ctx, _prog, _vert);
				gl::attach(ctx, _prog, _frag);
				
				if (!gl::link(ctx, _prog))
				{
					if (_errorOut)
					{
						*_errorOut = gl::get_info_log(ctx, _prog);
					};
					return false;
				};

				return true;
			});
	};

}