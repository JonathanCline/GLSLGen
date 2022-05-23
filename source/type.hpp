#pragma once

/** @file */

#include "utility.hpp"

#include <iosfwd>

namespace glsl
{
	/**
	 * @brief GLSL types
	*/
	enum class GLSLType
	{
		/**
		 * @brief Used to explicitly represent errors in the type system.
		*/
		glsl_error = -1,

		// autodeduced type.
		glsl_auto = 0,


		// void
		glsl_void,


		// bool
		glsl_bool,


		// int
		glsl_int,

		// ivec2
		glsl_ivec2,

		// ivec3
		glsl_ivec3,

		// ivec4
		glsl_ivec4,



		// uint
		glsl_uint,

		// uvec2
		glsl_uvec2,

		// uvec3
		glsl_uvec3,
		
		// uvec4
		glsl_uvec4,



		// float
		glsl_float,

		// vec2
		glsl_vec2,
		// vec3
		glsl_vec3,
		// vec4
		glsl_vec4,



		// double
		glsl_double,

		// dvec2
		glsl_dvec2,
		// dvec3
		glsl_dvec3,
		// dvec4
		glsl_dvec4,



		// mat4
		glsl_mat4,



		/**
		 * @brief sampler2D
		*/
		glsl_sampler_2D,

		/**
		 * @brief sampler2DArray
		*/
		glsl_sampler_2D_array,


	};


	std::ostream& operator<<(std::ostream& _ostr, const GLSLType& v);

	/**
	 * @brief Generic type categories
	*/
	enum class GLSLGenType
	{
		// float, fvec2, fvec3, fvec4
		gen_float = 1,

		// double, dvec2, dvec3, dvec4
		gen_double,

		// int, ivec2, ivec3, ivec4
		gen_int,

		// uint, uvec2, uvec3, uvec4
		gen_uint,
	};


	/**
	 * @brief Checks if a type is a scalar tyoe.
	 * @param _type GLSL type name.
	 * @return True if scaler, false otherwise.
	*/
	bool is_scalar(GLSLType _type);

	/**
	 * @brief Checks if a type is a vector tyoe.
	 * @param _type GLSL type name.
	 * @return True if vector, false otherwise.
	*/
	bool is_vector(GLSLType _type);

	/**
	 * @brief Checks if a type is a matrix type.
	 * @param _type GLSL type name.
	 * @return True if matrix, false otherwise.
	*/
	bool is_matrix(GLSLType _type);

	/**
	 * @brief Checks if a type is a sampler.
	 * @param _type GLSL type name.
	 * @return True if sampler, false otherwise.
	*/
	bool is_sampler(GLSLType _type);

	/**
	 * @brief Gets the type held within a container-like type.
	 *
	 * Only valid for vector and matrix types.
	 *
	 * @param _type GLSL type name.
	 * @return GLSL element type name.
	*/
	GLSLType element_type(GLSLType _type);




	/**
	 * @brief Gets a string with the name of a glsl type.
	 * @param _type GLSL type.
	 * @return Null-terminated string pointer.
	*/
	constexpr const char* glsl_typename(GLSLType _type)
	{
		switch (_type)
		{
		case GLSLType::glsl_error:
			HUBRIS_ASSERT(false);
			return "ERROR";

		case GLSLType::glsl_void:
			return "void";

		case GLSLType::glsl_bool:
			return "bool";

		case GLSLType::glsl_int:
			return "int";
		case GLSLType::glsl_ivec2:
			return "ivec2";
		case GLSLType::glsl_ivec3:
			return "ivec3";
		case GLSLType::glsl_ivec4:
			return "ivec4";

		case GLSLType::glsl_uint:
			return "int";
		case GLSLType::glsl_uvec2:
			return "uvec2";
		case GLSLType::glsl_uvec3:
			return "uvec3";
		case GLSLType::glsl_uvec4:
			return "uvec4";

		case GLSLType::glsl_float:
			return "float";
		case GLSLType::glsl_vec2:
			return "vec2";
		case GLSLType::glsl_vec3:
			return "vec3";
		case GLSLType::glsl_vec4:
			return "vec4";

		case GLSLType::glsl_double:
			return "double";
		case GLSLType::glsl_dvec2:
			return "dvec2";
		case GLSLType::glsl_dvec3:
			return "dvec3";
		case GLSLType::glsl_dvec4:
			return "dvec4";

		case GLSLType::glsl_mat4:
			return "mat4";

		case GLSLType::glsl_sampler_2D:
			return "sampler2D";
		case GLSLType::glsl_sampler_2D_array:
			return "sampler2DArray";

		default:
			HUBRIS_ASSERT(false);
			return "";
		};
	}

	/**
	 * @brief Gets the size of a vector type.
	 * @param _type GLSL vector type.
	 * @return Number of elements in vector.
	*/
	constexpr size_t vec_size(GLSLType _type)
	{
		switch (_type)
		{
		case GLSLType::glsl_error:
			HUBRIS_ASSERT(false);
			return 0;

		case GLSLType::glsl_vec2:
			return 2;
		case GLSLType::glsl_vec3:
			return 3;
		case GLSLType::glsl_vec4:
			return 4;

		case GLSLType::glsl_ivec2:
			return 2;
		case GLSLType::glsl_ivec3:
			return 3;
		case GLSLType::glsl_ivec4:
			return 4;

		case GLSLType::glsl_uvec2:
			return 2;
		case GLSLType::glsl_uvec3:
			return 3;
		case GLSLType::glsl_uvec4:
			return 4;

		case GLSLType::glsl_dvec2:
			return 2;
		case GLSLType::glsl_dvec3:
			return 3;
		case GLSLType::glsl_dvec4:
			return 4;

		default:
			HUBRIS_ABORT_M("Invalid argument");
			return 0;
		};
	};

	/**
	 * @brief Checks if a type can be implicitly converted into another type.
	 * @param _fromType Type converted from.
	 * @param _toType Type being converted to.
	 * @return True if implicitly convertible, false otherwise.
	*/
	bool is_implicitly_convertible_to(GLSLType _fromType, GLSLType _toType);

	/**
	 * @brief Checks if a type can be casted into another type.
	 * @param _fromType Type casting from.
	 * @param _toType Type being casted to.
	 * @return True if castable, false otherwise.
	*/
	bool is_castable_to(GLSLType _fromType, GLSLType _toType);

	/**
	 * @brief Checks if a type is in a generic type category.
	 * @param _type GLSL type name.
	 * @param _genType Generic type category.
	 * @return True if in category, false otherwise.
	*/
	bool is_type_in_category(GLSLType _type, GLSLGenType _genType);

};
