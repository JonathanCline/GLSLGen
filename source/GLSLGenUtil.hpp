#pragma once

/** @file */

#include "variable.hpp"
#include "function_decl.hpp"
#include "literal.hpp"
#include "context.hpp"

#include <jclib/concepts.h>
#include <jclib/functional.h>
#include <jclib/ranges.h>

#include <map>
#include <list>
#include <array>
#include <ranges>
#include <memory>
#include <charconv>
#include <algorithm>

namespace glsl
{
	class GLSLExpression;
	struct GLSLExpressionDeleter
	{
		void operator()(GLSLExpression* p) const;
	};

	enum class GLSLBinaryOperator
	{
		// a + b
		addition,
		add = addition,

		// a - b
		subtract,
		sub = subtract,

		// a * b
		multiply,
		mult = multiply,

		// a / b
		divide,
		div = divide,

		// a == b
		equal,
		eq = equal,

		// a != b
		not_equal,
		neq = not_equal,

	};
	
	/**
	 * @brief Checks if a binary operator is invocable given the types of the args.
	 * @param _operator Binary operator name.
	 * @param lhs Left hand arg type.
	 * @param rhs Right hand arg type.
	 * @return True if invocable, false otherwise.
	*/
	bool invocable(GLSLBinaryOperator _operator, GLSLType lhs, GLSLType rhs);


	enum class GLSLUnaryOperator
	{
		positive = 1,	// +x
		pos = positive,

		negative,		// -x
		neg = negative,
	};



	enum class GLSLExpressionType
	{
		identity = 0,
		cast = 1,
		function_call,
		binary_op,
		swizzle,
	};

	using UniqueExpression = std::unique_ptr<GLSLExpression, GLSLExpressionDeleter>;

	class GLSLArgument
	{
	private:
		using variant_type = std::variant<GLSLVariableID, UniqueExpression, GLSLLiteral>;

	public:

		bool is_variable() const { return this->vt_.index() == 0; };
		bool is_expression() const { return this->vt_.index() == 1; };
		bool is_literal() const { return this->vt_.index() == 2; };

		GLSLVariableID id() const noexcept
		{
			if (this->is_expression())
			{
				return GLSLVariableID();
			}
			else
			{
				return std::get<GLSLVariableID>(this->vt_);
			};
		};

		GLSLType type(const GLSLContext& _context) const;


		GLSLExpression& expr()
		{
			return *std::get<1>(this->vt_);
		};
		const GLSLExpression& expr() const
		{
			return *std::get<1>(this->vt_);
		};

		bool generate(std::ostream& _ostr, const GLSLContext& _context) const;

		GLSLArgument() :
			vt_(GLSLVariableID(0))
		{};
		GLSLArgument(GLSLVariableID _id) :
			vt_(_id)
		{};
		GLSLArgument(UniqueExpression _expr) :
			vt_(std::move(_expr))
		{};
		GLSLArgument(GLSLLiteral _literal) :
			vt_(std::move(_literal))
		{};

		GLSLArgument(GLSLArgument&& other) noexcept = default;
		GLSLArgument& operator=(GLSLArgument&& other) noexcept = default;

	private:
		variant_type vt_;
	};



	struct GLSLExpressionBase
	{
	public:
		bool check_validity(const GLSLContext& _context) const { return true; };
		GLSLExpressionBase() = default;
	};

	class GLSLExpr_Identity : public GLSLExpressionBase
	{
	public:

		GLSLArgument param;

		GLSLType result_type(const GLSLContext& _context) const
		{
			const auto _type = this->param.type(_context);

			if (_type == GLSLType::glsl_auto)
			{
				HUBRIS_BREAK();
			};

			return _type;
		};

		GLSLExpr_Identity() = default;
		GLSLExpr_Identity(GLSLArgument _param) :
			param(std::move(_param))
		{};
	};

	class GLSLExpr_Cast : public GLSLExpressionBase
	{
	public:

		GLSLArgument param;

	private:
		GLSLType to_;
	public:

		GLSLExpr_Cast& set_to_type(GLSLType _type)&
		{
			HUBRIS_ASSERT(_type != GLSLType::glsl_error);
			HUBRIS_ASSERT(_type != GLSLType::glsl_auto);
			this->to_ = _type;
			return *this;
		};
		GLSLExpr_Cast&& set_to_type(GLSLType _type)&&
		{
			this->set_to_type(_type);
			return static_cast<GLSLExpr_Cast&&>(*this);
		};

		GLSLType result_type(const GLSLContext& _context) const
		{
			return this->to_;
		};

		GLSLExpr_Cast() :
			param{},
			to_{ GLSLType::glsl_error }
		{};
		GLSLExpr_Cast(GLSLType _toType, GLSLArgument _param) :
			to_(_toType), param(std::move(_param))
		{
			HUBRIS_ASSERT(_toType != GLSLType::glsl_error);
			HUBRIS_ASSERT(_toType != GLSLType::glsl_auto);
		};
	};

	class GLSLExpr_FunctionCall : public GLSLExpressionBase
	{
	public:

		GLSLFunctionID function{};
		std::vector<GLSLArgument> params{};

		GLSLExpr_FunctionCall& add_param(GLSLArgument _param)&
		{
			this->params.push_back(std::move(_param));
			return *this;
		};
		GLSLExpr_FunctionCall&& add_param(GLSLArgument _param)&&
		{
			this->params.push_back(std::move(_param));
			return static_cast<GLSLExpr_FunctionCall&&>(*this);
		};

	private:

		inline auto resolve_parameters(const GLSLContext& _context) const
		{
			std::vector<GLSLType> _paramTypes(this->params.size(), GLSLType());
			std::ranges::copy(this->params | std::views::transform([&_context](auto& v)
				{
					return v.type(_context);
				}), _paramTypes.begin());
			return _paramTypes;
		};

	public:

		GLSLType result_type(const GLSLContext& _context) const;


		GLSLExpr_FunctionCall& resolve_params(GLSLContext& _context)&;

		GLSLExpr_FunctionCall&& resolve_params(GLSLContext& _context)&&
		{
			this->resolve_params(_context);
			return static_cast<GLSLExpr_FunctionCall&&>(*this);
		};

		GLSLExpr_FunctionCall() = default;
		GLSLExpr_FunctionCall(GLSLFunctionID _function) :
			function(std::move(_function))
		{};

	};

	class GLSLExpr_BinaryOp : public GLSLExpressionBase
	{
	public:
		
		GLSLArgument lhs;
		GLSLArgument rhs;
		GLSLBinaryOperator op;

		GLSLType result_type(const GLSLContext& _context) const;

		GLSLExpr_BinaryOp() = default;
		GLSLExpr_BinaryOp(GLSLBinaryOperator _operator) :
			op(_operator)
		{};
		GLSLExpr_BinaryOp(GLSLBinaryOperator _operator, GLSLArgument _lhs, GLSLArgument _rhs) :
			op(_operator),
			lhs(std::move(_lhs)),
			rhs(std::move(_rhs))
		{};
	};

	class GLSLExpr_Swizzle : public GLSLExpressionBase
	{
	public:

		GLSLArgument what;
		std::array<uint8_t, 4> swizzle_{};

		GLSLExpr_Swizzle& swizzle(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)&
		{
			auto& [s0, s1, s2, s3] = this->swizzle_;
			s0 = n0;
			s1 = n1;
			s2 = n2;
			s3 = n3;
			return *this;
		};
		GLSLExpr_Swizzle& swizzle(uint8_t n0, uint8_t n1, uint8_t n2)&
		{
			return this->swizzle(n0, n1, n2, 255);
		};
		GLSLExpr_Swizzle& swizzle(uint8_t n0, uint8_t n1)&
		{
			return this->swizzle(n0, n1, 255, 255);
		};
		GLSLExpr_Swizzle& swizzle(uint8_t n0)&
		{
			return this->swizzle(n0, 255, 255, 255);
		};

		GLSLExpr_Swizzle&& swizzle(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)&&
		{
			return static_cast<GLSLExpr_Swizzle&&>(this->swizzle(n0, n1, n2, n3));
		};
		GLSLExpr_Swizzle&& swizzle(uint8_t n0, uint8_t n1, uint8_t n2)&&
		{
			return static_cast<GLSLExpr_Swizzle&&>(this->swizzle(n0, n1, n2));
		};
		GLSLExpr_Swizzle&& swizzle(uint8_t n0, uint8_t n1)&&
		{
			return static_cast<GLSLExpr_Swizzle&&>(this->swizzle(n0, n1));
		};
		GLSLExpr_Swizzle&& swizzle(uint8_t n0)&&
		{
			return static_cast<GLSLExpr_Swizzle&&>(this->swizzle(n0));
		};

		GLSLType result_type(const GLSLContext& _context) const;
		bool check_validity(const GLSLContext& _context) const;

		GLSLExpr_Swizzle() = default;
		GLSLExpr_Swizzle(GLSLArgument _what, uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3) :
			what(std::move(_what))
		{
			this->swizzle(n0, n1, n2, n3);
		};
		GLSLExpr_Swizzle(GLSLArgument _what, uint8_t n0, uint8_t n1, uint8_t n2) :
			GLSLExpr_Swizzle(std::move(_what), n0, n1, n2, 255)
		{};
		GLSLExpr_Swizzle(GLSLArgument _what, uint8_t n0, uint8_t n1) :
			GLSLExpr_Swizzle(std::move(_what), n0, n1, 255, 255)
		{};
		GLSLExpr_Swizzle(GLSLArgument _what, uint8_t n0) :
			GLSLExpr_Swizzle(std::move(_what), n0, 255, 255, 255)
		{};
		GLSLExpr_Swizzle(GLSLArgument _what) :
			GLSLExpr_Swizzle(std::move(_what), 255, 255, 255, 255)
		{};

	};

	class GLSLExpression
	{
	private:

		using variant_type = std::variant<
			GLSLExpr_Identity,
			GLSLExpr_Cast,
			GLSLExpr_FunctionCall,
			GLSLExpr_BinaryOp,
			GLSLExpr_Swizzle
		>;

	public:

		GLSLExpressionType type() const
		{
			return GLSLExpressionType(this->vt_.index());
		};

		template <GLSLExpressionType Type>
		auto& get()
		{
			return std::get<(size_t)Type>(this->vt_);
		};
		
		template <GLSLExpressionType Type>
		const auto& get() const
		{
			return std::get<(size_t)Type>(this->vt_);
		};

		template <typename T>
		auto& get()
		{
			return std::get<T>(this->vt_);
		};

		template <typename T>
		const auto& get() const
		{
			return std::get<T>(this->vt_);
		};

		GLSLType result_type(const GLSLContext& _context) const
		{
			const auto _result = std::visit([&_context](auto& _expr)
				{
					const auto _result = _expr.result_type(_context);
					
					HUBRIS_ASSERT(_result != GLSLType::glsl_error);
					if (_result == GLSLType::glsl_auto)
					{
						HUBRIS_BREAK();
					};

					return _result;
				},
				this->vt_);
	
			HUBRIS_ASSERT(_result != GLSLType::glsl_error);
			if (_result == GLSLType::glsl_auto)
			{
				HUBRIS_BREAK();
			};
			return _result;
		};
		bool check_validity(const GLSLContext& _context) const
		{
			const auto _result = std::visit([&_context](auto& _expr)
				{
					return _expr.check_validity(_context);
				},
				this->vt_);
			return _result;
		};
		


		template <typename T> requires (jc::cx_element_of<jc::remove_cvref_t<T>, variant_type> &&
			std::move_constructible<std::remove_cvref_t<T>>)
		GLSLExpression(T&& _expr) :
			vt_(std::forward<T>(_expr))
		{};

		template <typename T> requires (jc::cx_element_of<jc::remove_cvref_t<T>, variant_type>&&
			std::move_constructible<std::remove_cvref_t<T>>)
		static UniqueExpression make_unique(T&& _expr)
		{
			return UniqueExpression(new GLSLExpression(std::forward<T>(_expr)));
		};

		GLSLExpression() :
			vt_(GLSLExpr_Identity{  })
		{};

		GLSLExpression(GLSLExpression&& other) noexcept = default;
		GLSLExpression& operator=(GLSLExpression&& other) noexcept = default;

	private:
		variant_type vt_;
	};



	bool generate_expression_string(std::ostream& _ostr, const GLSLContext& _context, const GLSLExpression& _expr);
};
