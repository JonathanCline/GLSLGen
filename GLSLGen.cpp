#include "GLSLGen.hpp"
#include "GLSLGenUtil.hpp"

#include <fstream>
#include <charconv>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <array>
#include <map>
#include <ranges>
#include <algorithm>
#include <variant>
#include <random>
#include <iostream>
#include <span>

#include <jclib/memory.h>

namespace fs = std::filesystem;

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


enum class GLSLStatementType
{
	declaration = 1,
	assignment,
};



struct GLSLStatement
{
	using Type = GLSLStatementType;

	GLSLVariableID dest;
	GLSLExpression expr;

	/**
	 * @brief The type of statement.
	*/
	GLSLStatementType type;

	explicit GLSLStatement(GLSLStatementType _type) :
		type(_type)
	{};

};


struct GLSLFunction
{
public:

	GLSLFunction& set_name(const std::string& _name)
	{
		this->name_ = _name;
		return *this;
	};

	std::string_view name() const
	{
		return this->name_;
	}
	GLSLType return_type() const
	{
		return GLSLType::glsl_void;
	};

	auto body()
	{
		return std::span(this->body_);
	};
	auto body() const
	{
		return std::span(this->body_);
	};

	void append(GLSLStatement _statement)
	{
		this->body_.push_back(std::move(_statement));
	};

	GLSLFunction(const std::string& _name) :
		name_(_name)
	{};
	GLSLFunction() = default;

private:
	std::string name_;
	std::vector<GLSLStatement> body_{};

};

struct GLSLParams
{
public:

	auto inputs(bool _builtin = false) const
	{
		return this->context_->inputs(_builtin);
	};
	auto outputs(bool _builtin = false) const
	{
		return this->context_->outputs(_builtin);
	};
	auto uniforms() const
	{
		return this->context_->uniforms();
	};

	auto get_name(GLSLVariableID _varID) const
	{
		return this->context_->name(_varID);
	};
	auto get_type(GLSLVariableID _varID) const
	{
		return this->context_->type(_varID);
	};

	GLSLVariableID id(const std::string& _name) const
	{
		return this->context_->id(_name);
	};

	GLSLFunction main_fn{ "main" };

	int version = 330;

	bool check() const
	{
		for (auto& i : inputs())
		{
			for (auto& o : outputs())
			{
				if (i.name() == o.name())
				{
					return false;
				};
			};
		};

		return true;
	};

	GLSLParams(GLSLContext& _context) :
		context_(&_context)
	{};

private:
	GLSLContext* context_{};
};

void add_builtin_vertex_shader_variables(GLSLContext& _context)
{
	// Inputs
	{
		_context.new_variable("gl_VertexID", GLSLType::glsl_int)
			->set_builtin()
			.set_inout(GLSLInOut::in);
	};
	{
		_context.new_variable("gl_InstanceID", GLSLType::glsl_int)
			->set_builtin()
			.set_inout(GLSLInOut::in);
	};

	// Outputs
	{
		_context.new_variable("gl_Position", GLSLType::glsl_vec4)
			->set_builtin()
			.set_inout(GLSLInOut::out);
	};
};
void add_builtin_fragment_shader_variables(GLSLContext& _context)
{
	// Inputs
	(*_context.new_variable("gl_FragCoord", GLSLType::glsl_vec4))
		.set_builtin()
		.set_inout(GLSLInOut::in);
	(*_context.new_variable("gl_FrontFacing", GLSLType::glsl_bool))
		.set_builtin()
		.set_inout(GLSLInOut::in);
	(*_context.new_variable("gl_PointCoord", GLSLType::glsl_vec2))
		.set_builtin()
		.set_inout(GLSLInOut::in);

	// Outputs
	(*_context.new_variable("gl_FragDepth", GLSLType::glsl_float))
		.set_builtin()
		.set_inout(GLSLInOut::out);
};

void add_builtin_functions(GLSLContext& _context)
{
	(*_context.new_function("sin", GLSLType::glsl_float))
		.set_builtin()
		.add_overload(GLSLType::glsl_float, GLSLType::glsl_float);
	(*_context.new_function("cos", GLSLType::glsl_float))
		.set_builtin()
		.add_overload(GLSLType::glsl_float, GLSLType::glsl_float);
	(*_context.new_function("tan", GLSLType::glsl_float))
		.set_builtin()
		.add_overload(GLSLType::glsl_float, GLSLType::glsl_float);

	(*_context.new_function("abs", GLSLType::glsl_float))
		.set_builtin()
		.add_overload(GLSLType::glsl_float, GLSLType::glsl_float);

	(*_context.new_function("dot"))
		.set_builtin()
		.add_overload(GLSLType::glsl_float, { GLSLGenType::gen_float, GLSLGenType::gen_float })
		.add_overload(GLSLType::glsl_double, { GLSLGenType::gen_double, GLSLGenType::gen_double });

	(*_context.new_function("texture"))
		.set_builtin()
		// texture 2D sampler
		.add_overload(GLSLType::glsl_vec4, { GLSLType::glsl_sampler_2D, GLSLType::glsl_vec2 })
		// texture 2D array Sampler
		.add_overload(GLSLType::glsl_vec4, { GLSLType::glsl_sampler_2D_array, GLSLType::glsl_vec3 });

};


bool deduce_auto(GLSLContext& _context, GLSLParams& _params)
{
	for (auto& _statement : _params.main_fn.body())
	{
		if (_context.type(_statement.dest) == GLSLType::glsl_auto)
		{
			const auto _resultType = _statement.expr.result_type(_context);
			_context.set_deduced_type(_statement.dest, _resultType);
		};
	};
	return true;
};

void generate_glsl(const GLSLContext& _context, const GLSLParams& _params, std::ostream& _ostr)
{
	_ostr << "#version " << _params.version << " core\n\n";

	{
		size_t n = 0;
		for (auto& v : _params.inputs())
		{
			_ostr << "in " << v.type() << ' ' << v.name() << "; // id = " << v.id().get() << '\n';
			++n;
		};
		if (n != 0)
		{
			_ostr << '\n';
		};
	};

	{
		size_t n = 0;
		for (auto& v : _params.outputs())
		{
			_ostr << "out " << v.type() << ' ' << v.name() << "; // id = " << v.id().get() << '\n';
			++n;
		};
		if (n != 0)
		{
			_ostr << '\n';
		};
	};

	// Uniforms
	{
		for (auto& v : _params.uniforms())
		{
			_ostr << "uniform " << v.type() << ' ' << v.name() << ";\n";
		};
	};

	_ostr << _params.main_fn.return_type() << ' '
		<< _params.main_fn.name() << "()\n{\n";

	for (auto& v : _params.main_fn.body())
	{
		if (!v.expr.check_validity(_context))
		{
			HUBRIS_ASSERT(false);
		};

		const auto _destID = v.dest;
		const auto _dest = _params.get_name(_destID);

		const auto& _expr = v.expr;

		switch (v.type)
		{
		case GLSLStatementType::assignment:
			_ostr << '\t' << _context.name(v.dest) << " = ";
			break;
		case GLSLStatementType::declaration:
			_ostr << '\t' << _context.type(v.dest) << ' ' << _context.name(v.dest) << " = ";
			break;
		default:
			abort();
			break;
		};

		if (!generate_expression_string(_ostr, _context, v.expr))
		{
			abort();
		};

		_ostr << ";\n";
	};

	_ostr << "};\n";
};


struct GLSLFunctionBuilder
{
public:

	GLSLFunctionBuilder& append_statement(GLSLStatement _statement)
	{
		this->function_->append(std::move(_statement));
		return *this;
	};

	GLSLFunctionBuilder& assign(GLSLContext& _context, GLSLVariableID _dest, GLSLExpression::Parameter _param)
	{
		auto _statement = GLSLStatement(GLSLStatementType::assignment);
		_statement.dest = _dest;
		
		const auto _paramType = _param.type(_context);
		const auto _destType = _context.type(_dest);

		HUBRIS_ASSERT(_paramType != GLSLType::glsl_error);
		HUBRIS_ASSERT(_paramType != GLSLType::glsl_auto);
		HUBRIS_ASSERT(_destType != GLSLType::glsl_error);

		if (_paramType == _destType || _destType == GLSLType::glsl_auto)
		{
			if (_destType == GLSLType::glsl_auto)
			{
				_context.set_deduced_type(_dest, _paramType);
			};

			auto _expr = GLSLExpression::Identity();
			_expr.param = std::move(_param);
			_statement.expr = std::move(_expr);
		}
		else
		{
			_statement.expr = GLSLExpression::Cast(_destType, std::move(_param));
		};

		return this->append_statement(std::move(_statement));
	};
	GLSLFunctionBuilder& declare(GLSLContext& _context, GLSLVariableID _dest, GLSLExpression::Parameter _param)
	{
		auto _statement = GLSLStatement(GLSLStatementType::declaration);
		_statement.dest = _dest;

		const auto _destType = _context.type(_dest);
		const auto _paramType = _param.type(_context);

		HUBRIS_ASSERT(_destType != GLSLType::glsl_error);
		HUBRIS_ASSERT(_paramType != GLSLType::glsl_error);

		if (_paramType == _destType || _destType == GLSLType::glsl_auto)
		{
			if (_destType == GLSLType::glsl_auto)
			{
				_context.set_deduced_type(_dest, _paramType);
			};

			auto _expr = GLSLExpression::Identity();
			_expr.param = std::move(_param);
			_statement.expr = std::move(_expr);
		}
		else
		{
			_statement.expr = GLSLExpression::Cast(_destType, std::move(_param));
		};

		return this->append_statement(std::move(_statement));
	};

	GLSLExpression::UniqueExpression binary_op(GLSLContext& _context, GLSLBinaryOperator _op,
		GLSLExpression::Parameter lhs, GLSLExpression::Parameter rhs)
	{
		// Check types.
		const auto _lhsType = lhs.type(_context);
		const auto _rhsType = rhs.type(_context);
		
		if (_lhsType != GLSLType::glsl_auto && _rhsType != GLSLType::glsl_auto)
		{
			// Both parameters are not set to auto, make sure the expression is invocable
			if (!invocable(_op, _lhsType, _rhsType))
			{
				// NOT invocable, try for a cast?
				HUBRIS_ASSERT(false);
			};
		};

		// Construct the expression
		return GLSLExpression::make_unique(GLSLExpression::BinaryOp(_op, std::move(lhs), std::move(rhs)));
	};

	GLSLFunctionBuilder(GLSLFunction& _function) :
		function_(&_function)
	{};

private:
	GLSLFunction* function_;
};


struct GLSLGen
{
	GLSLContext context;
	GLSLParams params;

	GLSLGen() :
		context(),
		params(this->context)
	{};
};

void gen_vertex_shader(GLSLGen& _gen)
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
void gen_fragment_shader(GLSLGen& _gen)
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
