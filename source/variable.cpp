#include "variable.hpp"

#include <ostream>

namespace glsl
{
	inline const char* inout_string(GLSLInOut _inOut)
	{
		switch (_inOut)
		{
		case GLSLInOut::in:
			return "in";
		case GLSLInOut::out:
			return "out";
		case GLSLInOut::local:
			return "";
		};
	};


	void GLSLVariable::generate(std::ostream& _ostr) const
	{ 
		if (this->uniform())
		{
			_ostr << "uniform ";
		}
		else if (const auto _inOut = this->inout(); _inOut != GLSLInOut::local)
		{
			_ostr << inout_string(_inOut) << ' ';
		};

		_ostr << this->type() << ' ' << this->name() << "; // id = " << this->id().get() << '\n';
	};


};