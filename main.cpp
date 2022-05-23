#include "GLSLGen.hpp"
#include "GLSLGenUtil.hpp"

#include "verify/verify.hpp"

#include <sstream>

using namespace glsl;

inline void write_text_file(const fs::path& _path, const std::string_view _data)
{
	auto f = std::ofstream(_path);
	f.write(_data.data(), _data.size());
};
inline std::string read_text_file(const fs::path& _path)
{
	auto buf = std::array<char, 512>{};
	auto s = std::string();

	auto f = std::ifstream(_path);
	while (f)
	{
		f.read(buf.data(), buf.size());
		s.append(buf.data(), f.gcount());
	};

	return s;
};


inline void gen_vertex_shader(GLSLGen& _gen)
{
	auto& _context = _gen.context;
	auto& _params = _gen.params;
	_params.version = 330;

	add_builtin_vertex_shader_variables(_context);
	add_builtin_functions(_context);

	{
		_context.new_variable("in_pos", GLSLType::glsl_vec3)
			->set_inout(GLSLInOut::in);
		_context.new_variable("in_uvs", GLSLType::glsl_vec3)
			->set_inout(GLSLInOut::in);
		_context.new_variable("in_col", GLSLType::glsl_vec4)
			->set_inout(GLSLInOut::in);

		_context.new_variable("frag_uvs", GLSLType::glsl_auto)
			->set_inout(GLSLInOut::out);
		_context.new_variable("frag_col", GLSLType::glsl_vec4)
			->set_inout(GLSLInOut::out);
	};

	{
		auto _main = GLSLFunctionBuilder(_params.main_fn);
		_main.assign(_context, _context.id("frag_uvs"), _context.id("in_uvs"));
		_main.assign(_context, _context.id("frag_col"), _context.id("in_col"));
		_main.assign(_context, _context.id("gl_Position"), _context.id("in_pos"));

		_main.assign(_context, _context.id("in_pos"), GLSLLiteral(12.0f, 0.0f, 0.0f));
	};

	if (!_params.check())
	{
		abort();
	};

	deduce_auto(_context, _params);
};
inline void gen_fragment_shader(GLSLGen& _gen)
{
	auto& _context = _gen.context;
	auto& _params = _gen.params;
	_params.version = 330;

	// Add builtins
	add_builtin_fragment_shader_variables(_gen.context);
	add_builtin_functions(_gen.context);

	// Define shader

	// Inputs
	_context.new_variable("frag_col", GLSLType::glsl_vec4)->set_inout(GLSLInOut::in);
	_context.new_variable("frag_uvs", GLSLType::glsl_vec3)->set_inout(GLSLInOut::in);

	// Output(s)
	_context.new_variable("color", GLSLType::glsl_vec4)->set_inout(GLSLInOut::out);

	// Fingers crossed.
	_context.new_variable("test_texture", GLSLType::glsl_sampler_2D_array);

	{
		auto _main = GLSLFunctionBuilder(_params.main_fn);

		// Get texel color.
		auto _texel = _context.new_variable("texel");

		_main.declare(_context, _texel->id(),
			GLSLExpression::make_unique
			(
				GLSLExpr_FunctionCall(_context.function_id("texture"))
				.add_param(_context.id("test_texture"))
				.add_param(_context.id("frag_uvs"))
				.resolve_params(_context)
			)
		);

		// Assign to color output.
		_main.assign(_context, _context.id("color"), _texel->id());
	};


	if (!_params.check())
	{
		abort();
	};

	deduce_auto(_context, _params);
};

inline void gen_compute_shader(GLSLGen& _gen)
{
	auto& _context = _gen.context;
	auto& _params = _gen.params;
	_params.version = 330;

	add_builtin_compute_shader_variables(_context);
	add_builtin_functions(_context);

	{
		auto _main = GLSLFunctionBuilder(_params.main_fn);



	};

	if (!_params.check())
	{
		abort();
	};

	deduce_auto(_context, _params);
};

inline int test_compute_shader()
{
	const auto _outPath = fs::path(PROJECT_SOURCE_ROOT "/_out/compute.glsl");
	if (!fs::exists(_outPath.parent_path()))
	{
		fs::create_directories(_outPath.parent_path());
	};
	auto g = GLSLGen();
	gen_compute_shader(g);
	auto f = std::ofstream(_outPath);
	generate_glsl(g.context, g.params, f);

	auto ss = std::stringstream();
	generate_glsl(g.context, g.params, ss);
	auto s = ss.str();

	std::string _error{};
	if (!glsl::opengl_validate_compute_shader_glsl(s, &_error))
	{
		std::cout << _error << '\n';
		return 1;
	};

	return 0;
};

inline int test_vertfrag_shader()
{
	auto _vertCode = std::string();
	auto _fragCode = std::string();

	{
		const auto _outPath = fs::path(PROJECT_SOURCE_ROOT "/_out/vertex.glsl");
		if (!fs::exists(_outPath.parent_path()))
		{
			fs::create_directories(_outPath.parent_path());
		};
		auto g = GLSLGen();
		gen_vertex_shader(g);
		auto f = std::ofstream(_outPath);
		generate_glsl(g.context, g.params, f);

		auto s = std::stringstream();
		generate_glsl(g.context, g.params, s);

		_vertCode = s.str();
	};

	{
		const auto _outPath = fs::path(PROJECT_SOURCE_ROOT "/_out/fragment.glsl");
		if (!fs::exists(_outPath.parent_path()))
		{
			fs::create_directories(_outPath.parent_path());
		};
		auto g = GLSLGen();
		gen_fragment_shader(g);
		auto f = std::ofstream(_outPath);
		generate_glsl(g.context, g.params, f);

		auto s = std::stringstream();
		generate_glsl(g.context, g.params, s);

		_fragCode = s.str();
	};

	std::string _error{};
	if (!glsl::opengl_validate_vertex_fragment_glsl_program(_vertCode, _fragCode, &_error))
	{
		std::cout << _error << '\n';
		return 1;
	};

	return 0;
};





int main()
{
	return test_vertfrag_shader();
};
