#include "function_parameter.hpp"



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
