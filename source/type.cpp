#include "type.hpp"

#include <jclib/algorithm.h>

#include <array>
#include <ostream>

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


	std::ostream& operator<<(std::ostream& _ostr, const GLSLType& v)
	{
		return _ostr << glsl_typename(v);
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
