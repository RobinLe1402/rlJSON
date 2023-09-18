#include "rlJSON/Object.hpp"
#include "../private-includes/Tools.hpp"

using namespace std::string_literals;

namespace rlJSON
{

	Object::Object() : Value(Value::Type::Object) {}

	Object::Object(std::map<String, Value *> oValues) :
		Value(Value::Type::Object),
		m_oValues(oValues)
	{}

	std::string Object::encode() const
	{
		std::string sResult = "{\n";

		for (const auto &it : m_oValues)
		{
			sResult += AddIndent(it.first.encode(), true) + ": " +
				AddIndent(it.second->encode(), false) + ",\n";
		}
		if (m_oValues.size() > 0)
			sResult.erase(sResult.length() - 2, 1);

		sResult += "}\n";

		return sResult;
	}

}