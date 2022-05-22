#pragma once

/** @file */

#include <compare>
#include <cstdint>

namespace glsl
{
	
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
}