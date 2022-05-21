#pragma once

/** @file */

#include "utility.hpp"

#include <jclib/concepts.h>
#include <jclib/functional.h>
#include <jclib/ranges.h>

#include <map>
#include <array>
#include <iosfwd>
#include <ranges>
#include <string>
#include <cstdint>
#include <variant>
#include <charconv>
#include <algorithm>
#include <memory>
#include <compare>
#include <list>
#include <vector>
#include <span>
#include <optional>

namespace glsl
{
	enum class GLSLInOut
	{
		local = 0,
		in = 1,
		out = 2,
	};

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

	constexpr std::string_view glsl_typename(GLSLType _type)
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

		case GLSLType::glsl_dvec2:
			return 2;
		case GLSLType::glsl_dvec3:
			return 3;
		case GLSLType::glsl_dvec4:
			return 4;

		case GLSLType::glsl_void:
			return 0;

		default:
			return 1;
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
	

	std::ostream& operator<<(std::ostream& _ostr, const GLSLType& v);

	namespace impl
	{
		template <typename TagT>
		struct IDBase
		{
		public:

			using rep = uint32_t;

			constexpr explicit operator bool() const noexcept
			{
				return this->id_ != 0;
			};
			constexpr rep get() const noexcept { return this->id_; };

			constexpr auto operator<=>(const IDBase& rhs) const noexcept = default;

			constexpr IDBase() :
				id_(0)
			{};
			constexpr explicit IDBase(rep _value) :
				id_(_value)
			{};

		private:
			rep id_;
		};
	};

	using GLSLVariableID = impl::IDBase<struct GLSLVariableIDTag>;
	using GLSLFunctionID = impl::IDBase<struct GLSLFunctionIDTag>;



	using GLSLVariableName = std::string;

	struct GLSLVariable
	{
	public:

		using ID = GLSLVariableID;

		ID id() const
		{
			return this->id_;
		};
		GLSLInOut inout() const
		{
			return this->inout_;
		};
		GLSLType type() const
		{
			return this->type_;
		};
		bool builtin() const
		{
			return this->builtin_;
		};
		std::string_view name() const 
		{
			return this->name_;
		};
		bool uniform() const
		{
			return this->uniform_;
		};
		bool is_const() const
		{
			return this->const_;
		};

		/**
		 * @brief Checks if the variable can be written to.
		 * @return True if can be writable, false otherwise.
		*/
		bool can_write() const
		{
			return !this->is_const() &&
					this->inout() != GLSLInOut::in &&
					!this->uniform();
		};

		GLSLVariable& set_inout(GLSLInOut _value)
		{
			this->inout_ = _value;
			return *this;
		};
		GLSLVariable& set_type(GLSLType _value)
		{
			this->type_ = _value;
			// Samplers MUST be uniforms.
			if (is_sampler(_value))
			{
				this->uniform_ = true;
			};
			return *this;
		};
		GLSLVariable& set_builtin(bool _builtin = true)
		{
			this->builtin_ = _builtin;
			return *this;
		};
		GLSLVariable& set_deduced_type(GLSLType _type)
		{
			if (this->type() != GLSLType::glsl_auto) { abort(); };
			this->type_ = _type;
			return *this;
		};
		GLSLVariable& set_uniform(bool _uniform = true)
		{
			this->uniform_ = _uniform;
			return *this;
		};
		GLSLVariable& set_const(bool _const = true)
		{
			this->const_ = _const;
			return *this;
		};

		GLSLVariable() = default;

		explicit GLSLVariable(ID _id) :
			id_(_id)
		{};
		explicit GLSLVariable(ID _id, const GLSLVariableName& _name, GLSLType _type) :
			id_(_id), name_(_name), type_(_type)
		{
			if (is_sampler(this->type()))
			{
				this->set_uniform();
			};
		};

	private:
		GLSLVariableName name_{};
		GLSLType type_{};
		ID id_{};
		GLSLInOut inout_ = GLSLInOut::local;
		bool builtin_ = false;
		bool uniform_ = false;
		bool const_ = false;
	};


	/**
	 * @brief Generic type categories
	*/
	enum class GLSLGenType
	{
		gen_float = 1,
		gen_double = 2,
	};

	/**
	 * @brief Checks if a type is in a generic type category.
	 * @param _type GLSL type name.
	 * @param _genType Generic type category.
	 * @return True if in category, false otherwise.
	*/
	bool is_type_in_category(GLSLType _type, GLSLGenType _genType);




	struct GLSLFunctionParameter
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


	using GLSLFunctionOverloadRating = int;
	constexpr static GLSLFunctionOverloadRating rating_match_v = std::numeric_limits<GLSLFunctionOverloadRating>::max();
	constexpr static GLSLFunctionOverloadRating rating_no_match_v = std::numeric_limits<GLSLFunctionOverloadRating>::min();

	struct GLSLFunctionDecl
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



			OverloadRating rate_parameter_match(std::span<const GLSLType> _params) const
			{
				if (this->invocable(_params))
				{
					return rating_match_v;
				}
				else
				{
					auto& _overloadParams = this->params;

					// Check parameter count
					const auto _invokeParamsCount = _params.size();
					const auto _myParamsCount = this->params.size();
					if (_invokeParamsCount != _myParamsCount)
					{
						return rating_no_match_v;
					};

					// Rate parameter castability
					OverloadRating _convertabilityScore = 0;
					for (size_t n = 0; n != _invokeParamsCount; ++n)
					{
						const auto& _fromType = _params[n];
						const auto& _toType = _overloadParams[n];

						using Convertability = GLSLFunctionParameter::Convertability;
						const auto _convertability = _toType.convertability_from(_fromType);

						if (_convertability == Convertability::none)
						{
							return rating_no_match_v;
						}
						else
						{
							_convertabilityScore += static_cast<OverloadRating>(_convertability);
						};
					};

					return _convertabilityScore;
				};
			};



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
		GLSLType return_type() const
		{
			if (this->overloads_.size() == 0)
			{
				// No overloads defined, assume void.
				return GLSLType::glsl_void;
			}
			else if (this->overloads_.size() == 1)
			{
				// Only one overload, return its type.
				return this->overloads_.front().return_type;
			}
			else
			{
				// If all overloads return the same type, we can just return
				// that type as the return type.

				GLSLType _ret = this->overloads_.front().return_type;
				for (auto& _overload : this->overloads_ | std::views::drop(1))
				{
					if (_ret != _overload.return_type)
					{
						return GLSLType::glsl_auto;
					}
				};
				return _ret;
			};
		};

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

		const Overload* find_best_overload(std::span<const GLSLType> _params) const
		{
			struct Data
			{
				const Overload* overload{};
				GLSLFunctionDecl::OverloadRating rating = rating_no_match_v;

				void rate(std::span<const GLSLType> _params)
				{
					this->rating = this->overload->rate_parameter_match(_params);
				};

				Data() = default;
				Data(const Overload& _overload) :
					overload(&_overload),
					rating(rating_no_match_v)
				{};
			};

			auto _overloads = std::vector<Data>(this->overloads_.size());
			std::ranges::copy(this->overloads_, _overloads.begin());

			for (auto& v : _overloads) { v.rate(_params); };

			// Remove none-matches
			std::erase_if(_overloads, (jc::member & &Data::rating) | jc::equals & 0);
			
			if (_overloads.empty())
			{
				return nullptr;
			};

			// Sort by rating
			const auto _comparisonFn = [&_params](const Data& lhs, const Data& rhs) -> int
			{
				const auto l = lhs.rating;
				const auto r = rhs.rating;
				return r - l;
			};
			std::ranges::sort(_overloads, _comparisonFn);

			// Return best (front)
			return _overloads.front().overload;
		};

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


	/**
	 * @brief Represents a literal value.
	*/
	struct GLSLLiteral
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
	struct GLSLContext
	{
	private:

		GLSLVariableID new_variable_id()
		{
			return GLSLVariableID(++this->id_counter_);
		};
		GLSLFunctionID new_function_id()
		{
			return GLSLFunctionID(++this->id_counter_);
		};

		std::string new_variable_name(GLSLVariableID _id) const
		{
			auto _buf = std::array<char, 16>{};
			auto r = std::to_chars(_buf.data(), _buf.data() + _buf.size(), _id.get());
			auto s = std::string("_var");
			s.append(_buf.data(), r.ptr);
			return s;
		};
		GLSLVariable* new_variable(GLSLVariableID _id, const std::string& _name, GLSLType _type)
		{
			auto _var = GLSLVariable(_id, _name, _type);
			auto [it, _good] = this->variables_.insert_or_assign(_id, _var);
			return &it->second;
		};

	public:

		GLSLVariable* new_variable(const std::string& _name, GLSLType _type)
		{
			const auto _id = this->new_variable_id();
			return this->new_variable(_id, _name, _type);
		};
		GLSLVariable* new_variable(const std::string& _name)
		{
			return this->new_variable(_name, GLSLType::glsl_auto);
		};
		GLSLVariable* new_variable(GLSLType _type)
		{
			auto _id = this->new_variable_id();
			auto _name = this->new_variable_name(_id);
			return this->new_variable(_id, _name, _type);
		};
		GLSLVariable* new_variable()
		{
			return this->new_variable(GLSLType::glsl_auto);
		};

		GLSLFunctionDecl* new_function(const std::string& _name, GLSLType _returnType)
		{
			const auto _id = this->new_function_id();
			auto _decl = GLSLFunctionDecl(_id, _name, _returnType);
			auto [it, _good] = this->functions_.insert_or_assign(_id, std::move(_decl));
			return &it->second;
		};
		GLSLFunctionDecl* new_function(const std::string& _name)
		{
			return this->new_function(_name, GLSLType::glsl_void);
		};

		GLSLVariable* find(GLSLVariableID _id)
		{
			auto& _vs = this->variables_;
			const auto it = _vs.find(_id);
			return (it != _vs.end())? &it->second : nullptr;
		};
		const GLSLVariable* find(GLSLVariableID _id) const
		{
			auto& _vs = this->variables_;
			const auto it = _vs.find(_id);
			return (it != _vs.end()) ? &it->second : nullptr;
		};

		GLSLFunctionDecl* find(GLSLFunctionID _id)
		{
			auto& _vs = this->functions_;
			const auto it = _vs.find(_id);
			return (it != _vs.end()) ? &it->second : nullptr;
		};
		const GLSLFunctionDecl* find(GLSLFunctionID _id) const
		{
			auto& _vs = this->functions_;
			const auto it = _vs.find(_id);
			return (it != _vs.end()) ? &it->second : nullptr;
		};

		GLSLVariable* find(std::string_view _name)
		{
			auto& _vs = this->variables_;
			const auto it = std::ranges::find_if(
				_vs,
				[&_name](auto& v) -> bool
				{
					return v.second.name() == _name;
				});
			return (it != _vs.end()) ? &it->second : nullptr;
		};
		const GLSLVariable* find(std::string_view _name) const
		{
			auto& _vs = this->variables_;
			const auto it = std::ranges::find_if(
				_vs,
				[&_name](auto& v) -> bool
				{
					return v.second.name() == _name;
				});
			return (it != _vs.end()) ? &it->second : nullptr;
		};

		GLSLFunctionDecl* find_function(std::string_view _name)
		{
			auto& _vs = this->functions_;
			const auto it = std::ranges::find_if(
				_vs,
				[&_name](auto& v) -> bool
				{
					return v.second.name() == _name;
				});
			return (it != _vs.end()) ? &it->second : nullptr;
		};
		const GLSLFunctionDecl* find_function(std::string_view _name) const
		{
			auto& _vs = this->functions_;
			const auto it = std::ranges::find_if(
				_vs,
				[&_name](auto& v) -> bool
				{
					return v.second.name() == _name;
				});
			return (it != _vs.end()) ? &it->second : nullptr;
		};

		bool contains(GLSLVariableID _id) const
		{
			return this->find(_id) != nullptr;
		};
		bool contains(GLSLFunctionID _id) const
		{
			return this->find(_id) != nullptr;
		};
		
		bool contains(std::string_view _name) const
		{
			return this->find(_name) != nullptr;
		};
		bool contains_function(std::string_view _name) const
		{
			return this->find_function(_name) != nullptr;
		};

		std::string_view name(GLSLVariableID _id) const
		{
			auto _var = this->find(_id);
			return (_var) ? _var->name() : std::string_view();
		};
		std::string_view name(GLSLFunctionID _id) const
		{
			auto _var = this->find(_id);
			return (_var) ? _var->name() : std::string_view();
		};
		
		GLSLType type(GLSLVariableID _id) const
		{
			auto _var = this->find(_id);
			return (_var) ? _var->type() : GLSLType::glsl_auto;
		};
		GLSLType type(std::string_view _name) const
		{
			auto _var = this->find(_name);
			return (_var) ? _var->type() : GLSLType::glsl_auto;
		};

	private:
		auto variables()
		{
			return this->variables_ | std::views::values;
		};
		auto variables() const
		{
			return this->variables_ | std::views::values;
		};

		auto filter_variables_inout(GLSLInOut _inOut, bool _builtin)
		{
			return this->variables() | std::views::filter([_inOut, _builtin](auto& v)
					{
						return v.inout() == _inOut && v.builtin() == _builtin;
					});
		};
		auto filter_variables_inout(GLSLInOut _inOut, bool _builtin) const
		{
			return this->variables() | std::views::filter([_inOut, _builtin](auto& v)
					{
						return v.inout() == _inOut && v.builtin() == _builtin;
					});
		};

	public:

		auto inputs(bool _builtin = false) const
		{
			return this->filter_variables_inout(GLSLInOut::in, _builtin);
		};
		auto outputs(bool _builtin = false) const
		{
			return this->filter_variables_inout(GLSLInOut::out, _builtin);
		};
		auto uniforms() const
		{
			return this->variables() | std::views::filter([](auto& v)
				{
					return v.uniform();
				});
		};

		auto functions(bool _builtin = false) const
		{
			return this->functions_ | std::views::values | std::views::filter([_builtin](auto& v) -> bool
					{
						return v.builtin() == _builtin;
					});
		};

		GLSLVariableID id(const std::string& _name) const
		{
			auto& vs = this->variables_;
			for (auto& [_id, _var] : vs)
			{
				if (_var.name() == _name)
				{
					return _var.id();
				};
			};
			return GLSLVariableID();
		};
		GLSLFunctionID function_id(const std::string& _name) const
		{
			auto& vs = this->functions_;
			for (auto& [_id, _var] : vs)
			{
				if (_var.name() == _name)
				{
					return _var.id();
				};
			};
			return GLSLFunctionID();
		};

		void set_deduced_type(GLSLVariableID _varID, GLSLType _type)
		{
			auto _var = this->find(_varID);
			if (!_var) { abort(); };

			_var->set_deduced_type(_type);
		};

		GLSLContext() = default;
	private:

		std::map<GLSLVariableID, GLSLVariable, jc::transparent<jc::less_t>> variables_;
		std::map<GLSLFunctionID, GLSLFunctionDecl, jc::transparent<jc::less_t>> functions_;

		GLSLVariableID::rep id_counter_ = 0;

	};


	struct GLSLExpression;
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

	struct GLSLExpression
	{
	public:

		using UniqueExpression = std::unique_ptr<GLSLExpression, GLSLExpressionDeleter>;

		struct Parameter
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

			Parameter() :
				vt_(GLSLVariableID(0))
			{};
			Parameter(GLSLVariableID _id) :
				vt_(_id)
			{};
			Parameter(UniqueExpression _expr) :
				vt_(std::move(_expr))
			{};
			Parameter(GLSLLiteral _literal) :
				vt_(std::move(_literal))
			{};

			Parameter(Parameter&& other) noexcept = default;
			Parameter& operator=(Parameter&& other) noexcept = default;

		private:
			variant_type vt_;
		};

		struct ExprBase
		{
		public:
			bool check_validity(const GLSLContext& _context) const { return true; };
			ExprBase() = default;
		};

		struct Identity : public ExprBase
		{
			Parameter param;

			GLSLType result_type(const GLSLContext& _context) const
			{
				return this->param.type(_context);
			};

			Identity() = default;
			Identity(Parameter _param) :
				param(std::move(_param))
			{};
		};
	
		struct Cast : public ExprBase
		{
			Parameter param;

		private:
			GLSLType to_;
		public:

			Cast& set_to_type(GLSLType _type) &
			{
				HUBRIS_ASSERT(_type != GLSLType::glsl_error);
				HUBRIS_ASSERT(_type != GLSLType::glsl_auto);
				this->to_ = _type;
				return *this;
			};
			Cast&& set_to_type(GLSLType _type)&&
			{
				this->set_to_type(_type);
				return static_cast<Cast&&>(*this);
			};

			GLSLType result_type(const GLSLContext& _context) const
			{
				return this->to_;
			};

			Cast() :
				param{},
				to_{ GLSLType::glsl_error }
			{};
			Cast(GLSLType _toType, Parameter _param) :
				to_(_toType), param(std::move(_param))
			{
				HUBRIS_ASSERT(_toType != GLSLType::glsl_error);
				HUBRIS_ASSERT(_toType != GLSLType::glsl_auto);
			};
		};

		struct FunctionCall : public ExprBase
		{
			GLSLFunctionID function{};
			std::vector<Parameter> params{};

			FunctionCall& add_param(Parameter _param) &
			{
				this->params.push_back(std::move(_param));
				return *this;
			};
			FunctionCall&& add_param(Parameter _param) &&
			{
				this->params.push_back(std::move(_param));
				return static_cast<FunctionCall&&>(*this);
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

			GLSLType result_type(const GLSLContext& _context) const
			{
				auto& _fn = *_context.find(this->function);
				auto _paramTypes = this->resolve_parameters(_context);
				auto _ret = _fn.return_type(_paramTypes);
				return _ret.value_or(GLSLType::glsl_error);
			};
			

			FunctionCall& resolve_params(GLSLContext& _context)&;

			FunctionCall&& resolve_params(GLSLContext& _context) &&
			{
				this->resolve_params(_context);
				return static_cast<FunctionCall&&>(*this);
			};

			FunctionCall() = default;
			FunctionCall(GLSLFunctionID _function) :
				function(std::move(_function))
			{};
			
		};

		struct BinaryOp : public ExprBase
		{
			Parameter lhs;
			Parameter rhs;
			GLSLBinaryOperator op;

			GLSLType result_type(const GLSLContext& _context) const;

			BinaryOp() = default;
			BinaryOp(GLSLBinaryOperator _operator) :
				op(_operator)
			{};
			BinaryOp(GLSLBinaryOperator _operator, Parameter _lhs, Parameter _rhs) :
				op(_operator),
				lhs(std::move(_lhs)),
				rhs(std::move(_rhs))
			{};
		};

		struct Swizzle
		{
			Parameter what;
			std::array<uint8_t, 4> swizzle_{};

			Swizzle& swizzle(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)&
			{
				auto& [s0, s1, s2, s3] = this->swizzle_;
				s0 = n0;
				s1 = n1;
				s2 = n2;
				s3 = n3;
				return *this;
			};
			Swizzle& swizzle(uint8_t n0, uint8_t n1, uint8_t n2)&
			{
				return this->swizzle(n0, n1, n2, 255);
			};
			Swizzle& swizzle(uint8_t n0, uint8_t n1)&
			{
				return this->swizzle(n0, n1, 255, 255);
			};
			Swizzle& swizzle(uint8_t n0)&
			{
				return this->swizzle(n0, 255, 255, 255);
			};

			Swizzle&& swizzle(uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3)&&
			{
				return static_cast<Swizzle&&>(this->swizzle(n0, n1, n2, n3));
			};
			Swizzle&& swizzle(uint8_t n0, uint8_t n1, uint8_t n2) &&
			{
				return static_cast<Swizzle&&>(this->swizzle(n0, n1, n2));
			};
			Swizzle&& swizzle(uint8_t n0, uint8_t n1) &&
			{
				return static_cast<Swizzle&&>(this->swizzle(n0, n1));
			};
			Swizzle&& swizzle(uint8_t n0) &&
			{
				return static_cast<Swizzle&&>(this->swizzle(n0));
			};

			GLSLType result_type(const GLSLContext& _context) const;
			bool check_validity(const GLSLContext& _context) const;

			Swizzle() = default;
			Swizzle(Parameter _what, uint8_t n0, uint8_t n1, uint8_t n2, uint8_t n3) :
				what(std::move(_what))
			{
				this->swizzle(n0, n1, n2, n3);
			};
			Swizzle(Parameter _what, uint8_t n0, uint8_t n1, uint8_t n2) :
				Swizzle(std::move(_what), n0, n1, n2, 255)
			{};
			Swizzle(Parameter _what, uint8_t n0, uint8_t n1) :
				Swizzle(std::move(_what), n0, n1, 255, 255)
			{};
			Swizzle(Parameter _what, uint8_t n0) :
				Swizzle(std::move(_what), n0, 255, 255, 255)
			{};
			Swizzle(Parameter _what) :
				Swizzle(std::move(_what), 255, 255, 255, 255)
			{};

		};

	private:
		using variant_type = std::variant<Identity, Cast, FunctionCall, BinaryOp, Swizzle>;
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
			vt_(Identity{  })
		{};

		GLSLExpression(GLSLExpression&& other) noexcept = default;
		GLSLExpression& operator=(GLSLExpression&& other) noexcept = default;

	private:
		variant_type vt_;
	};



	bool generate_expression_string(std::ostream& _ostr, const GLSLContext& _context, const GLSLExpression& _expr);
};
