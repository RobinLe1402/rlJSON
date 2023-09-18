#include "rlJSON/Number.hpp"

#include <iomanip>
#include <sstream>

namespace rlJSON
{

	Number::Number(uint64_t i) :
		Value(rlJSON::Value::Type::Number),
		m_eType(Type::UInt),
		m_iUIntValue(i),
		m_iIntValue(i),
		m_dFloatValue((double)i)
	{}

	Number::Number(int64_t i) :
		Value(rlJSON::Value::Type::Number),
		m_eType(Type::UInt),
		m_iUIntValue(i),
		m_iIntValue(i),
		m_dFloatValue((double)i)
	{}

	Number::Number(double f) :
		Value(rlJSON::Value::Type::Number),
		m_eType(Type::Float),
		m_iUIntValue ((uint64_t)f),
		m_iIntValue  ((int64_t)f),
		m_dFloatValue(f)
	{}

	std::string Number::encode() const
	{
		switch (m_eType)
		{
		case Type::UInt:
			return std::to_string(m_iUIntValue);
			
		case Type::Int:
			return std::to_string(m_iIntValue);

		case Type::Float:
			return std::to_string(m_dFloatValue);

		default:
			throw std::exception();
		}
	}

}