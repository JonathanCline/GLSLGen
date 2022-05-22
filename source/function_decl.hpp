#pragma once

/** @file */

#include "id.hpp"
#include "type.hpp"
#include "function_parameter.hpp"

#include <span>
#include <vector>
#include <numeric>
#include <optional>

namespace glsl
{

	using GLSLFunctionOverloadRating = int;
	constexpr static GLSLFunctionOverloadRating rating_match_v = std::numeric_limits<GLSLFunctionOverloadRating>::max();
	constexpr static GLSLFunctionOverloadRating rating_no_match_v = std::numeric_limits<GLSLFunctionOverloadRating>::min();

	class GLSLFunctionDecl
	{
	private:

		using OverloadRating = int;

		/**
		 * @brief Describes an overload of the function.
		*/
		struct Overload
		{
			std::vector<GLSLFunctionParameter> params{};
			GLSLType return_type;

			Overload& add_param(GLSLType _type)
			{
				this->params.push_back(GLSLFunctionParameter(_type));
				return *this;
			};
			Overload& add_param(GLSLGenType _genType)
			{
				this->params.push_back(GLSLFunctionParameter(_genType));
				return *this;
			};

			bool invocable(std::span<const GLSLType> _params) const
			{
				if (this->params.size() != _params.size()) {
					return false;
				};

				for (size_t n = 0; n != _params.size(); ++n)
				{
					if (!this->params.at(n).check_type(_params[n]))
					{
						return false;
					};
				};

				return true;
			};



			OverloadRating rate_parameter_match(std::span<const GLSLType> _params) const;



			explicit Overload(GLSLType _returnType, std::span<const GLSLFunctionParameter> _params) :
				return_type(_returnType),
				params(_params.begin(), _params.end())
			{};
			explicit Overload(GLSLType _returnType, std::initializer_list<GLSLFunctionParameter> _params) :
				return_type(_returnType),
				params(_params)
			{};
			explicit Overload(GLSLType _returnType, GLSLFunctionParameter _param) :
				return_type(_returnType),
				params{}
			{
				this->params.push_back(_param);
			};

			explicit Overload(GLSLType _returnType) :
				return_type(_returnType)
			{};

			Overload() :
				Overload(GLSLType::glsl_void)
			{};
		};

	public:

		using ID = GLSLFunctionID;

		ID id() const
		{
			return this->id_;
		};
		bool builtin() const
		{
			return this->builtin_;
		};

		/**
		 * @brief Gets the return type for the function.
		 *
		 * Generic functions may only have the return type defined when the parameter
		 * types are specified.
		 *
		 * If this is called for a generic function, it will return "glsl_auto" for the type.
		 *
		 * If ALL of the function's overloads always return the same type, that type
		 * is returned (even if the overload(s) is/are generic!)
		 *
		 * @return GLSL type.
		*/
		GLSLType return_type() const;

		/**
		 * @brief Gets the return type for the function.
		 *
		 * Generic functions may only have the return type defined when the parameter
		 * types are specified.
		 *
		 * Result is unspecified if the parameters given do not match the parameter
		 * count for the function.
		 *
		 * @param _params Parameter types used to "invoke" the function.
		 * @return GLSL type.
		*/
		std::optional<GLSLType> return_type(const std::span<GLSLType>& _params) const
		{
			// Return the result type of the first matching overload.
			for (auto& _overload : this->overloads_)
			{
				if (_overload.invocable(_params))
				{
					return _overload.return_type;
				};
			};

			// No matching overload found.
			return std::nullopt;
		};

		const Overload* find_best_overload(std::span<const GLSLType> _params) const;

		/**
		 * @brief Checks if the function can be invoked with a particular set of parameters.
		 * @param _params Paramaters.
		 * @return True if allowed, false otherwise.
		*/
		bool invocable(std::span<const GLSLType> _params)
		{
			for (auto& _overload : this->overloads_)
			{
				if (_overload.invocable(_params))
				{
					return true;
				};
			};
			return false;
		};



		std::string_view name() const
		{
			return this->name_;
		}

		GLSLFunctionDecl& set_name(const std::string& _name)
		{
			this->name_ = _name;
			return *this;
		};
		GLSLFunctionDecl& set_builtin(bool _builtin = true)
		{
			this->builtin_ = _builtin;
			return *this;
		};

		GLSLFunctionDecl& add_overload(GLSLType _returnType, std::span<const GLSLFunctionParameter> _params)
		{
			this->overloads_.push_back(Overload(_returnType, _params));
			return *this;
		};
		GLSLFunctionDecl& add_overload(GLSLType _returnType, GLSLFunctionParameter _param)
		{
			this->overloads_.push_back(Overload(_returnType, _param));
			return *this;
		};
		GLSLFunctionDecl& add_overload(GLSLType _returnType, std::initializer_list<GLSLFunctionParameter> _params)
		{
			this->overloads_.push_back(Overload(_returnType, _params));
			return *this;
		};


		GLSLFunctionDecl(ID _id, const std::string& _name, GLSLType _returnType) :
			id_(_id), name_(_name), overloads_{}
		{};
		GLSLFunctionDecl(ID _id, const std::string& _name) :
			GLSLFunctionDecl(_id, _name, GLSLType::glsl_void)
		{};
		GLSLFunctionDecl(ID _id) :
			id_(_id), overloads_{ Overload() }
		{};
		GLSLFunctionDecl() = default;

	private:
		std::string name_;
		std::vector<Overload> overloads_;

		ID id_;
		bool builtin_ = false;
	};

}
