#pragma once

/** @file */

#include "id.hpp"
#include "type.hpp"
#include "variable.hpp"
#include "function_decl.hpp"

#include <jclib/functional.h>

#include <map>
#include <array>
#include <string>
#include <ranges>
#include <charconv>
#include <algorithm>

namespace glsl
{

	class GLSLContext
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
			return (it != _vs.end()) ? &it->second : nullptr;
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


}