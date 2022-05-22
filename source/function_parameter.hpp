#pragma once

/** @file */

#include "type.hpp"

#include <variant>

namespace glsl
{
	class GLSLFunctionParameter
	{
	public:

		enum class Convertability
		{
			none,
			with_cast,
			implicit,
			deduced,
			strict_deduced,
			same,
		};

		bool is_generic() const;

		GLSLType get_type() const;
		GLSLGenType get_generic() const;

		/**
		 * @brief Checks if a type can be used directly (without casting) for this parameter.
		 * @param _type GLSL type name.
		 * @return True if allowed, false otherwose.
		*/
		bool check_type(GLSLType _type) const;

		Convertability convertability_from(GLSLType _fromType) const;


		/**
		 * @brief Sets the parameter to only accept a specific type.
		 * @param _type The type for the parameter.
		*/
		GLSLFunctionParameter(GLSLType _type) :
			type_(_type)
		{};

		/**
		 * @brief Sets the parameter accept a type within a generic type category.
		 * @param _type Generic type category.
		*/
		GLSLFunctionParameter(GLSLGenType _type) :
			type_(_type)
		{};

	private:

		/**
		 * @brief The type or type category for the parameter.
		*/
		std::variant<GLSLType, GLSLGenType> type_;

	};


}