#include "function_decl.hpp"


#include <ranges>
#include <algorithm>

#include <jclib/ranges.h>


namespace glsl
{
	GLSLFunctionDecl::OverloadRating GLSLFunctionDecl::Overload::rate_parameter_match(std::span<const GLSLType> _params) const
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

	GLSLType GLSLFunctionDecl::return_type() const
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

	const GLSLFunctionDecl::Overload* GLSLFunctionDecl::find_best_overload(std::span<const GLSLType> _params) const
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

}
