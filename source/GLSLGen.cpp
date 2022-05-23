#include "GLSLGen.hpp"



namespace glsl
{
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
	void add_builtin_compute_shader_variables(GLSLContext& _context)
	{
		// Inputs
		(*_context.new_variable("gl_NumWorkGroups", GLSLType::glsl_uvec3))
			.set_builtin()
			.set_inout(GLSLInOut::in);
		(*_context.new_variable("gl_WorkGroupID", GLSLType::glsl_uvec3))
			.set_builtin()
			.set_inout(GLSLInOut::in);
		(*_context.new_variable("gl_LocalInvocationID", GLSLType::glsl_uvec3))
			.set_builtin()
			.set_inout(GLSLInOut::in);
		(*_context.new_variable("gl_GlobalInvocationID", GLSLType::glsl_uvec3))
			.set_builtin()
			.set_inout(GLSLInOut::in);
		(*_context.new_variable("gl_LocalInvocationIndex", GLSLType::glsl_uint))
			.set_builtin()
			.set_inout(GLSLInOut::in);
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
			for (auto& v : _context.inputs())
			{
				v.generate(_ostr);
				++n;
			};
			if (n != 0)
			{
				_ostr << '\n';
			};
		};

		{
			size_t n = 0;
			for (auto& v : _context.outputs())
			{
				v.generate(_ostr);
				++n;
			};
			if (n != 0)
			{
				_ostr << '\n';
			};
		};

		// Uniforms
		{
			size_t n = 0;
			for (auto& v : _context.uniforms())
			{
				v.generate(_ostr);
				++n;
			};
			if (n != 0)
			{
				_ostr << '\n';
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
			const auto _dest = _context.name(_destID);

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
};
