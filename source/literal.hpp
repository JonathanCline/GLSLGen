#pragma once

/** @file */

#include "type.hpp"

#include <array>
#include <variant>
#include <optional>

namespace glsl
{

	/**
	 * @brief Represents a literal value.
	*/
	class GLSLLiteral
	{
	private:

		using variant_type = std::variant
		<
			std::nullopt_t,
			std::array<bool, 4>,
			std::array<int, 4>,
			std::array<float, 4>,
			std::array<double, 4>
		>;

	public:

		GLSLType type() const noexcept { return this->type_; };

		bool has_value() const noexcept { return this->vt_.index() != 0; };
		explicit operator bool() const noexcept { return this->has_value(); };

		template <typename T>
		const std::array<T, 4>& arr() const
		{
			return std::get<std::array<T, 4>>(this->vt_);
		};

		template <typename T = float>
		T vec1() const { return this->arr<T>()[0]; };

		template <typename T = float>
		std::array<T, 2> vec2() const { return { this->arr<T>()[0], this->arr<T>()[1] }; }

		template <typename T = float>
		std::array<T, 3> vec3() const { return { this->arr<T>()[0], this->arr<T>()[1], this->arr<T>()[2] }; }

		template <typename T = float>
		std::array<T, 4> vec4() const { return this->arr<T>(); }



		explicit GLSLLiteral(GLSLType _type, std::array<bool, 4> _parts) :
			type_(_type), vt_(_parts)
		{};
		explicit GLSLLiteral(GLSLType _type, std::array<double, 4> _parts) :
			type_(_type), vt_(_parts)
		{};
		explicit GLSLLiteral(GLSLType _type, std::array<int, 4> _parts) :
			type_(_type), vt_(_parts)
		{};
		explicit GLSLLiteral(GLSLType _type, std::array<float, 4> _parts) :
			type_(_type), vt_(_parts)
		{};

		explicit GLSLLiteral(GLSLType _type, float _value) :
			GLSLLiteral(_type, std::array{ _value, 0.0f, 0.0f, 0.0f })
		{};
		explicit GLSLLiteral(GLSLType _type, double _value) :
			GLSLLiteral(_type, std::array{ _value, 0.0, 0.0, 0.0 })
		{};

		GLSLLiteral(float _value) :
			GLSLLiteral(GLSLType::glsl_float, std::array{ _value, 0.0f, 0.0f, 0.0f })
		{};
		GLSLLiteral(float x, float y) :
			GLSLLiteral(GLSLType::glsl_vec2, std::array{ x, y, 0.0f, 0.0f })
		{};
		GLSLLiteral(float x, float y, float z) :
			GLSLLiteral(GLSLType::glsl_vec3, std::array{ x, y, z, 0.0f })
		{};
		GLSLLiteral(float x, float y, float z, float w) :
			GLSLLiteral(GLSLType::glsl_vec4, std::array{ x, y, z, w })
		{};

		GLSLLiteral(double _value) :
			GLSLLiteral(GLSLType::glsl_double, std::array{ _value, 0.0, 0.0, 0.0 })
		{};
		GLSLLiteral(double x, double y) :
			GLSLLiteral(GLSLType::glsl_dvec2, std::array{ x, y, 0.0, 0.0 })
		{};
		GLSLLiteral(double x, double y, double z) :
			GLSLLiteral(GLSLType::glsl_dvec3, std::array{ x, y, z, 0.0 })
		{};
		GLSLLiteral(double x, double y, double z, double w) :
			GLSLLiteral(GLSLType::glsl_dvec4, std::array{ x, y, z, w })
		{};

		GLSLLiteral(int _value) :
			GLSLLiteral(GLSLType::glsl_int, std::array{ _value, 0, 0, 0 })
		{};

		GLSLLiteral(bool _value) :
			GLSLLiteral(GLSLType::glsl_bool, std::array{ _value, false, false, false })
		{};

		GLSLLiteral() :
			vt_(std::nullopt),
			type_(GLSLType::glsl_void)
		{};

	private:
		variant_type vt_;
		GLSLType type_;
	};

}