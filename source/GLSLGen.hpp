#pragma once

/** @file */


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

namespace glsl
{
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



	void add_builtin_vertex_shader_variables(GLSLContext& _context);
	void add_builtin_fragment_shader_variables(GLSLContext& _context);

	void add_builtin_functions(GLSLContext& _context);

	bool deduce_auto(GLSLContext& _context, GLSLParams& _params);

	void generate_glsl(const GLSLContext& _context, const GLSLParams& _params, std::ostream& _ostr);






	struct GLSLFunctionBuilder
	{
	public:

		GLSLFunctionBuilder& append_statement(GLSLStatement _statement)
		{
			this->function_->append(std::move(_statement));
			return *this;
		};

		GLSLFunctionBuilder& assign(GLSLContext& _context, GLSLVariableID _dest, GLSLArgument _param)
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

				auto _expr = GLSLExpr_Identity();
				_expr.param = std::move(_param);
				_statement.expr = std::move(_expr);
			}
			else
			{
				_statement.expr = GLSLExpr_Cast(_destType, std::move(_param));
			};

			return this->append_statement(std::move(_statement));
		};
		GLSLFunctionBuilder& declare(GLSLContext& _context, GLSLVariableID _dest, GLSLArgument _param)
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

				auto _expr = GLSLExpr_Identity();
				_expr.param = std::move(_param);
				_statement.expr = std::move(_expr);
			}
			else
			{
				_statement.expr = GLSLExpr_Cast(_destType, std::move(_param));
			};

			return this->append_statement(std::move(_statement));
		};

		UniqueExpression binary_op(GLSLContext& _context, GLSLBinaryOperator _op,
			GLSLArgument lhs, GLSLArgument rhs)
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
			return GLSLExpression::make_unique(GLSLExpr_BinaryOp(_op, std::move(lhs), std::move(rhs)));
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

};