#pragma once

/** @file */

#include "id.hpp"
#include "type.hpp"

#include <string>
#include <string_view>

namespace glsl
{
	using GLSLVariableName = std::string;

	enum class GLSLInOut
	{
		local = 0,
		in = 1,
		out = 2,
	};

	class GLSLVariable
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
		std::string_view name() const &
		{
			return this->name_;
		};
		std::string name() &&
		{
			return std::move(this->name_);
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

};