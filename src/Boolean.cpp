#include "rlJSON/Boolean.hpp"

namespace rlJSON
{

	Boolean::Boolean() : Value(Value::Type::Boolean) {}

	Boolean::Boolean(bool b) : Value(Value::Type::Boolean), m_bValue(b) {}

	Boolean &Boolean::operator=(bool b)
	{
		m_bValue = b;

		return *this;
	}

	std::string Boolean::encode() const
	{
		return m_bValue ? "true" : "false";
	}

}