#include "rlJSON/Null.hpp"

namespace rlJSON
{

	Null::Null() : Value(Value::Type::Null) {}

	std::string Null::encode() const { return "null"; }

}