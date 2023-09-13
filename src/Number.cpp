#include "rlJSON/Number.hpp"

#include <iomanip>
#include <sstream>

namespace rlJSON
{

	Number::Number(double d) : m_dValue(d) {}

	std::string Number::encode() const
	{
		const int iPrec = (m_dValue == std::fmod(m_dValue, 1.0)) ? 0 : 17;

		std::stringstream os;
		os << std::defaultfloat << std::setprecision(iPrec) << m_dValue;
		return os.str();
	}

}