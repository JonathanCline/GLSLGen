#include "GLSLGen.hpp"
#include "GLSLGenUtil.hpp"

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
		_context.new_variable("in_uvs", GLSLType::glsl_vec2)
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

		_main.declare(_context, _context.new_variable()->id(),
			GLSLExpression::make_unique(
				GLSLExpression::FunctionCall(_context.function_id("cos"))
				.add_param(
					GLSLExpression::make_unique(
						GLSLExpression::Swizzle(_context.id("in_pos"), 0, 1)
					)
				)
			)
		);

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
		auto _texel = _context.new_variable("texel",
			GLSLType::glsl_vec4);

		_main.declare(_context, _texel->id(),
			GLSLExpression::make_unique
			(
				GLSLExpression::FunctionCall(_context.function_id("texture"))
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

int main()
{
	{
		const auto _outPath = fs::path(PROJECT_SOURCE_ROOT "/vertex.glsl");
		auto g = GLSLGen();
		gen_vertex_shader(g);
		auto f = std::ofstream(_outPath);
		generate_glsl(g.context, g.params, f);
	}

	{
		const auto _outPath = fs::path(PROJECT_SOURCE_ROOT "/fragment.glsl");
		auto g = GLSLGen();
		gen_fragment_shader(g);
		auto f = std::ofstream(_outPath);
		generate_glsl(g.context, g.params, f);
	};

	return 0;
};
