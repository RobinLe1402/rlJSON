#include "rlJSON/Array.hpp"
#include "../private-includes/Tools.hpp"

using namespace std::string_literals;

namespace rlJSON
{

	Array::Array() : Value(Value::Type::Array) {}

	Array::Array(std::initializer_list<Value*> oValues) :
		Value(Value::Type::Array),
		m_oValues(oValues)
	{}

	std::string Array::encode() const
	{
		std::string sResult = "[\n";

		for (Value *p : m_oValues)
		{
			sResult += AddIndent(p->encode(), true) + ",\n";
		}
		if (m_oValues.size() > 0)
			sResult.erase(sResult.length() - 2, 1);

		sResult += "]";

		return sResult;
	}

}