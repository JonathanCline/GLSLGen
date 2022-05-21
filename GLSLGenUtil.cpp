#include "GLSLGenUtil.hpp"

#include <ostream>
#include <format>

#include <jclib/algorithm.h>

namespace glsl
{
	namespace
	{
		constexpr auto TYPE_CATEGORY_FLOAT = std::array
		{
			GLSLType::glsl_float,
			GLSLType::glsl_vec2,
			GLSLType::glsl_vec3,
			GLSLType::glsl_vec4,
		};
		
		constexpr auto TYPE_CATEGORY_DOUBLE = std::array
		{
			GLSLType::glsl_double,
			GLSLType::glsl_dvec2,
			GLSLType::glsl_dvec3,
			GLSLType::glsl_dvec4,
		};

		constexpr auto SCALAR_TYPES = std::array
		{
			GLSLType::glsl_double,
			GLSLType::glsl_float,
			GLSLType::glsl_int,
		};
		constexpr auto VECTOR_TYPES = std::array
		{
			GLSLType::glsl_vec2,
			GLSLType::glsl_vec3,
			GLSLType::glsl_vec4,
			GLSLType::glsl_dvec2,
			GLSLType::glsl_dvec3,
			GLSLType::glsl_dvec4,
		};
		constexpr auto MATRIX_TYPES = std::array
		{
			GLSLType::glsl_mat4
		};
		constexpr auto SAMPLER_TYPES = std::array
		{
			GLSLType::glsl_sampler_2D,
			GLSLType::glsl_sampler_2D_array,
		};
	};


	bool is_type_in_category(GLSLType _type, GLSLGenType _genType)
	{
		HUBRIS_ASSERT(_type != GLSLType::glsl_error);

		switch (_genType)
		{
		case GLSLGenType::gen_double:
			return jc::contains(TYPE_CATEGORY_DOUBLE, _type);
		case GLSLGenType::gen_float:
			return jc::contains(TYPE_CATEGORY_FLOAT, _type);
		default:
			return false;
		};
	};

	bool is_scalar(GLSLType _type)
	{
		HUBRIS_ASSERT(_type != GLSLType::glsl_error);
		return jc::contains(SCALAR_TYPES, _type);
	};
	bool is_vector(GLSLType _type)
	{
		HUBRIS_ASSERT(_type != GLSLType::glsl_error);
		return jc::contains(VECTOR_TYPES, _type);
	};
	bool is_matrix(GLSLType _type)
	{
		HUBRIS_ASSERT(_type != GLSLType::glsl_error);
		return jc::contains(MATRIX_TYPES, _type);
	};
	bool is_sampler(GLSLType _type)
	{
		HUBRIS_ASSERT(_type != GLSLType::glsl_error);
		return jc::contains(SAMPLER_TYPES, _type);
	};



	GLSLType element_type(GLSLType _type)
	{
		if (is_scalar(_type)) { return GLSLType::glsl_error; };

		if (is_matrix(_type))
		{
			// Matrix
			HUBRIS_ASSERT(_type == GLSLType::glsl_mat4);
			return GLSLType::glsl_vec4;
		}
		else if (is_vector(_type))
		{
			// Vector
			if (is_type_in_category(_type, GLSLGenType::gen_float))
			{
				return GLSLType::glsl_float;
			}
			else if (is_type_in_category(_type, GLSLGenType::gen_double))
			{
				return GLSLType::glsl_double;
			}
			else
			{
				// ???
				HUBRIS_ASSERT(false);
				return {};
			};
		}
		else
		{
			// ???
			HUBRIS_ASSERT(false);
			return {};
		};
	};


	bool is_implicitly_convertible_to(GLSLType _fromType, GLSLType _toType)
	{
		HUBRIS_ASSERT(_fromType != GLSLType::glsl_error);
		HUBRIS_ASSERT(_toType != GLSLType::glsl_error);

		if (_fromType == _toType) { return true; };

		if (is_vector(_toType) ^ is_vector(_fromType))
		{
			return false;
		}
		else
		{
			if (vec_size(_toType) != vec_size(_fromType))
			{
				return false;
			};
		};

		if (is_type_in_category(_toType, GLSLGenType::gen_double))
		{
			return is_type_in_category(_fromType, GLSLGenType::gen_float) || _fromType == GLSLType::glsl_int;
		}
		else if (is_type_in_category(_toType, GLSLGenType::gen_float))
		{
			return _fromType == GLSLType::glsl_int;
		}
		else
		{
			return false;
		};
	};
	bool is_castable_to(GLSLType _fromType, GLSLType _toType)
	{
		HUBRIS_ASSERT(_fromType != GLSLType::glsl_error);
		HUBRIS_ASSERT(_toType != GLSLType::glsl_error);

		if (_fromType != GLSLType::glsl_void && _toType != GLSLType::glsl_void)
		{
			return true;
		}
		else
		{
			return false;
		};
	};


};

namespace glsl
{
	bool GLSLFunctionParameter::is_generic() const
	{
		return this->type_.index() == 1;
	};

	GLSLType GLSLFunctionParameter::get_type() const
	{
		return std::get<0>(this->type_);
	};
	GLSLGenType GLSLFunctionParameter::get_generic() const
	{
		return std::get<1>(this->type_);
	};




	bool GLSLFunctionParameter::check_type(GLSLType _type) const
	{
		HUBRIS_ASSERT(_type != GLSLType::glsl_error);

		if (this->is_generic())
		{
			// Accepts a type within a type category
			const auto _genType = this->get_generic();
			return is_type_in_category(_type, _genType);
		}
		else
		{
			// Accepts only a specific type, if auto then accepts any
			const auto _requiredType = this->get_type();
			if (_requiredType == GLSLType::glsl_auto && _type != GLSLType::glsl_void)
			{
				// Auto is used and given type is not void, we good.
				return true;
			}
			else
			{
				// Direct compare.
				return _requiredType == _type;
			};
		};
	};

	GLSLFunctionParameter::Convertability GLSLFunctionParameter::convertability_from(GLSLType _fromType) const
	{
		HUBRIS_ASSERT(_fromType != GLSLType::glsl_error);

		if (_fromType == GLSLType::glsl_auto)
		{
			return Convertability::deduced;
		};

		if (this->is_generic())
		{
			const auto _genType = this->get_generic();
			if (is_type_in_category(_fromType, _genType))
			{
				return Convertability::strict_deduced;
			}
			else
			{
				// TODO : Check if we can convert into a type in the category
				return Convertability::none;
			};
		}
		else
		{
			const auto _toType = this->get_type();

			// Check for match
			if (_toType == _fromType)
			{
				return Convertability::same;
			};

			// Check for auto deduction
			if (_toType == GLSLType::glsl_auto && _fromType != GLSLType::glsl_void)
			{
				// Auto is used and given type is not void, we good.
				return Convertability::deduced;
			};

			// Non-auto type, check for implicit cast
			if (is_implicitly_convertible_to(_fromType, _toType))
			{
				return Convertability::implicit;
			};

			// Check for explicit cast
			if (is_castable_to(_fromType, _toType))
			{
				return Convertability::with_cast;
			};

			// No conversion
			return Convertability::none;
		};
	};

};

namespace glsl
{
	inline std::ostream& write(std::ostream& _ostr, std::string_view _fmt)
	{
		_ostr.write(_fmt.data(), _fmt.size());
		return _ostr;
	};
	inline std::ostream& write(std::ostream& _ostr, std::string_view _fmt, const auto&... _args)
	{
		const auto s = std::format(_fmt, _args...);
		return write(_ostr, s);
	};





	std::ostream& operator<<(std::ostream& _ostr, const GLSLType& v)
	{
		return _ostr << glsl_typename(v);
	};


	void GLSLExpressionDeleter::operator()(GLSLExpression* p) const
	{
		delete p;
	};


	GLSLType GLSLExpression::Parameter::type(const GLSLContext& _context) const
	{
		if (this->is_expression())
		{
			return this->expr().result_type(_context);
		}
		else if (this->is_literal())
		{
			return std::get<GLSLLiteral>(this->vt_).type();
		}
		else
		{
			return _context.type(this->id());
		};
	};


	bool GLSLExpression::Parameter::generate(std::ostream& _ostr, const GLSLContext& _context) const
	{
		if (this->is_expression())
		{
			return generate_expression_string(_ostr, _context, this->expr());
		}
		else if (this->is_literal())
		{
			auto& _literal = std::get<GLSLLiteral>(this->vt_);
			switch (_literal.type())
			{


			case GLSLType::glsl_bool:
			{
				write(_ostr, "{}", _literal.vec1<bool>());
			};
			return true;

			case GLSLType::glsl_int:
			{
				write(_ostr, "{:f}", _literal.vec1<int>());
			};
			return true;


			case GLSLType::glsl_float:
			{
				write(_ostr, "{:f}", _literal.vec1());
			};
			return true; 
			case GLSLType::glsl_vec2:
			{
				const auto [x, y] = _literal.vec2();
				write(_ostr, "vec2({:f}, {:f})", x, y);
			};
			return true;
			case GLSLType::glsl_vec3:
			{
				const auto [x, y, z] = _literal.vec3();
				write(_ostr, "vec3({:f}, {:f}, {:f})", x, y, z);
			};
			return true;
			case GLSLType::glsl_vec4:
			{
				const auto [x, y, z, w] = _literal.vec4();
				write(_ostr, "vec4({:f}, {:f}, {:f}, {:f})", x, y, z, w);
			};
			return true;

			case GLSLType::glsl_double:
			{
				write(_ostr, "{:f}", _literal.vec1<double>());
			};
			return true;
			case GLSLType::glsl_dvec2:
			{
				const auto [x, y] = _literal.vec2<double>();
				write(_ostr, "dvec2({:f}, {:f})", x, y);
			};
			return true;
			case GLSLType::glsl_dvec3:
			{
				const auto [x, y, z] = _literal.vec3<double>();
				write(_ostr, "dvec3({:f}, {:f}, {:f})", x, y, z);
			};
			return true;
			case GLSLType::glsl_dvec4:
			{
				const auto [x, y, z, w] = _literal.vec4<double>();
				write(_ostr, "dvec4({:f}, {:f}, {:f}, {:f})", x, y, z, w);
			};
			return true;


			case GLSLType::glsl_mat4:
			{
				const auto _iVal = _literal.vec1<float>();
				write(_ostr, "mat4({:f})", _iVal);
			};
			return true;

			default:
				abort();
				return false;
			};
		}
		else
		{
			_ostr << _context.name(this->id());
			return true;
		};
	};
	
	GLSLExpression::FunctionCall& GLSLExpression::FunctionCall::resolve_params(GLSLContext& _context) &
	{
		// Determine best overload
		auto& _function = *_context.find(this->function);
		auto _paramTypes = this->resolve_parameters(_context);
		auto _bestOverload = _function.find_best_overload(_paramTypes);

		size_t n = 0;
		for (auto& _param : this->params)
		{
			const auto _type = _param.type(_context);
			if (_type == GLSLType::glsl_error)
			{
				abort();
			}
			else if (!_bestOverload->params[n].check_type(_type))
			{
				// Non-implicit match, insert casting step
				_param = GLSLExpression::make_unique(
					GLSLExpression::Cast(_bestOverload->params[n].get_type(), std::move(_param))
				);
			}
			++n;
		};
		return *this;
	};


	inline GLSLType binary_operator_result_type(GLSLBinaryOperator _op, GLSLType lhs, GLSLType rhs)
	{
		switch (_op)
		{
		case GLSLBinaryOperator::sub:
			[[fallthrough]];
		case GLSLBinaryOperator::mult:
			[[fallthrough]];
		case GLSLBinaryOperator::div:
			[[fallthrough]];
		case GLSLBinaryOperator::add:
			if (is_scalar(lhs) && (is_vector(rhs) || is_matrix(rhs)))
			{
				return rhs;
			}
			else if (is_scalar(rhs) && (is_vector(lhs) || is_matrix(lhs)))
			{
				return lhs;
			}
			else
			{
				return lhs;
			};
			break;

		case GLSLBinaryOperator::eq:
			[[fallthrough]];
		case GLSLBinaryOperator::neq:
			return GLSLType::glsl_bool;

		default:
			abort();
			return {};
		};
	};

	bool invocable(GLSLBinaryOperator _operator, GLSLType lhs, GLSLType rhs)
	{
		switch (_operator)
		{
		case GLSLBinaryOperator::sub:
			[[fallthrough]];
		case GLSLBinaryOperator::mult:
			[[fallthrough]];
		case GLSLBinaryOperator::div:
			[[fallthrough]];
		case GLSLBinaryOperator::add:

			if (is_scalar(lhs) && (is_vector(rhs) || is_matrix(rhs)))
			{
				return true;
			}
			else if (is_scalar(rhs) && (is_vector(lhs) || is_matrix(lhs)))
			{
				return true;
			}
			else
			{
				return lhs == rhs;
			};
			break;

		case GLSLBinaryOperator::eq:
			[[fallthrough]];
		case GLSLBinaryOperator::neq:
			return lhs == rhs;

		default:
			abort();
			return false;
		};
	};



	GLSLType GLSLExpression::BinaryOp::result_type(const GLSLContext& _context) const
	{
		const auto _lhsType = this->lhs.type(_context);
		const auto _rhsType = this->rhs.type(_context);

		if (invocable(this->op, _lhsType, _rhsType))
		{
			return binary_operator_result_type(this->op, _lhsType, _rhsType);
		}
		else
		{
			return GLSLType::glsl_error;
		};
	};


	inline GLSLType make_vector_type(GLSLType _type, uint8_t _count)
	{
		HUBRIS_ASSERT(_count > 0 && _count <= 4);

		switch (_type)
		{
		case GLSLType::glsl_float:
			return (GLSLType)(jc::to_underlying(_type) + _count - 1);
		case GLSLType::glsl_double:
			return (GLSLType)(jc::to_underlying(_type) + _count - 1);
		default:
			HUBRIS_ASSERT(false);
			return GLSLType::glsl_error;
		};
	};
	inline GLSLType make_matrix_type(GLSLType _type, uint8_t _rows, uint8_t _columns)
	{
		HUBRIS_ASSERT(_rows == 4 && _columns == 4);
		switch (_type)
		{
		case GLSLType::glsl_float:
			return GLSLType::glsl_mat4;
		default:
			HUBRIS_ASSERT(false);
			return GLSLType::glsl_error;
		};
	};


	GLSLType GLSLExpression::Swizzle::result_type(const GLSLContext& _context) const
	{
		const auto _paramType = this->what.type(_context);
		const auto _swizzleCount = std::ranges::distance(this->swizzle_.begin(), std::ranges::find(this->swizzle_, 255));
		const auto _elementType = element_type(_paramType);

		if (is_matrix(_paramType))
		{
			if (_swizzleCount == 1)
			{
				return _elementType;
			}
			else
			{
				// Unfinished
				HUBRIS_ASSERT(false);
				return GLSLType::glsl_error;
			};
		}
		else if (is_vector(_paramType))
		{
			if (_swizzleCount == 1)
			{
				return _elementType;
			}
			else
			{
				return make_vector_type(_elementType, (uint8_t)_swizzleCount);
			};
		}
		else
		{
			// Scalar, therefore swizzle is not valid
			HUBRIS_ASSERT(false);
			return GLSLType::glsl_error;
		};
	};
	bool GLSLExpression::Swizzle::check_validity(const GLSLContext& _context) const
	{
		if (this->swizzle_.front() == 255)
		{
			// No swizzle params
			HUBRIS_ASSERT(false);
			return false;
		};

		return true;
	};



	inline char swizzle_char(uint8_t n)
	{
		switch (n)
		{
		case 0:
			return 'x';
		case 1:
			return 'y';
		case 2:
			return 'z';
		case 3:
			return 'w';
		default:
			abort();
			return ' ';
		};
	};



	inline std::string swizzle_str(std::span<const uint8_t> ns)
	{
		auto s = std::string();
		for (auto& n : ns) { s += swizzle_char(n); };
		return s;
	};

	inline std::string swizzle_str(uint8_t n0)
	{
		const auto ns = std::array{ n0 };
		return swizzle_str(ns);
	};
	inline std::string swizzle_str(uint8_t n0, uint8_t n1)
	{
		const auto ns = std::array{ n0, n1 };
		return swizzle_str(ns);
	};
	inline std::string swizzle_str(uint8_t n0, uint8_t n1, uint8_t n2)
	{
		const auto ns = std::array{ n0, n1, n2 };
		return swizzle_str(ns);
	};
	inline std::string swizzle_str(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)
	{
		const auto ns = std::array{ n0, n1, n2, n3 };
		return swizzle_str(ns);
	};

	inline std::string sequential_swizzle_str(size_t _count)
	{
		std::string s = {};
		for (size_t n = 0; n != _count; ++n)
		{
			s += swizzle_char((uint8_t)n);
		};
		return s;
	};


	bool generate_expression_string(std::ostream& _ostr, const GLSLContext& _context, const GLSLExpression& _expr)
	{
		// Stringify expression
		switch (_expr.type())
		{
		case GLSLExpressionType::identity:
		{
			auto& _expression = _expr.get<GLSLExpressionType::identity>();
			auto& _param = _expression.param;
			
			if (!_param.generate(_ostr, _context))
			{
				return false;
			};
		};
		break;
		case GLSLExpressionType::cast:
		{
			auto& _expression = _expr.get<GLSLExpression::Cast>();
			
			const auto& _toType = _expression.result_type(_context);
			const auto& _param = _expression.param;
			const auto _fromType = _param.type(_context);

			const auto _toTypeSize = vec_size(_toType);
			const auto _fromTypeSize = vec_size(_fromType);

			// We may need to swizzle if casting from a vector.
			if (is_vector(_fromType))
			{
				const auto _largerSize = std::max(_toTypeSize, _fromTypeSize);
				const auto _smallerSize = std::min(_toTypeSize, _fromTypeSize);

				// Specify vec type if up casting
				if (_toTypeSize > _fromTypeSize)
				{
					_ostr << _toType << '(';
				};

				// Add param name
				_param.generate(_ostr, _context);

				auto _swizzle = sequential_swizzle_str(_smallerSize);
				_ostr << '.' << _swizzle;

				// Define additional fields if we are casting to a larger vec
				if (_toTypeSize > _fromTypeSize)
				{
					for (size_t n = _smallerSize; n != _largerSize; ++n)
					{
						if (n == 3)
						{
							_ostr << ", 1.0";
						}
						else
						{
							_ostr << ", 0.0";
						};
					};
				};
			}
			else
			{
				// <type>(<param>)
				_ostr << _toType << '(';

				// Add param name
				_param.generate(_ostr, _context);
			};

			_ostr << ')';
		};
		break;
		case GLSLExpressionType::function_call:
		{
			const auto& _expression = _expr.get<GLSLExpression::FunctionCall>();
			auto& _function = *_context.find(_expression.function);

			_ostr << _function.name() << '(';

			size_t n = 0;
			for (auto& v : _expression.params)
			{
				if (n != 0)
				{
					_ostr << ", ";
				};

				v.generate(_ostr, _context);
				++n;
			};

			_ostr << ')';
		};
		break;
		case GLSLExpressionType::binary_op:
		{
			const auto& _expression = _expr.get<GLSLExpression::BinaryOp>();
			const auto& _lhsParam = _expression.lhs;
			const auto& _rhsParam = _expression.rhs;

			_ostr << '(';
			_lhsParam.generate(_ostr, _context);
			
			using Op = GLSLBinaryOperator;
			switch (_expression.op)
			{
			case Op::add:
				_ostr << " + ";
				break;
			case Op::sub:
				_ostr << " - ";
				break;
			case Op::mult:
				_ostr << " * ";
				break;
			case Op::div:
				_ostr << " / ";
				break;

			case Op::eq:
				_ostr << " == ";
				break;
			case Op::neq:
				_ostr << " != ";
				break;

			default:
				abort();
				break;
			};
			_rhsParam.generate(_ostr, _context);
			_ostr << ')';
		};
		break;
		case GLSLExpressionType::swizzle:
		{
			const auto& _expression = _expr.get<GLSLExpression::Swizzle>();
			const auto& _param = _expression.what;
			const auto _paramType = _param.type(_context);
			HUBRIS_ASSERT(is_vector(_paramType) || is_matrix(_paramType));
			
			const auto _givenSwizzleIndexesCount =
				std::ranges::distance(_expression.swizzle_.begin(),
					std::ranges::find(_expression.swizzle_, 255));

			// Actual swizzle indexes
			const auto _swizzleIndexes = std::views::take(_expression.swizzle_, _givenSwizzleIndexesCount);

			// Check that we can actually swizzle with the given indexes
			if (is_vector(_paramType))
			{
				for (auto& _index : _swizzleIndexes)
				{
					if (_index >= vec_size(_paramType))
					{
						// Impossible
						HUBRIS_ABORT();
					};
				};
			};

			const auto _swizzleStr = swizzle_str(_swizzleIndexes);
			_param.generate(_ostr, _context);
			_ostr << '.' << _swizzleStr;
		};
		break;
		default:
			abort();
			return false;
		};

		return true;
	};
};
