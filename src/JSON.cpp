#include "rlJSON/JSON.hpp"
#include "rlJSON/Array.hpp"
#include "rlJSON/Object.hpp"

#include <fstream>

namespace rlJSON
{

	JSON JSON::CreateAsArray()
	{
		JSON oResult;
		oResult.m_upRootValue = std::make_unique<Array>();
		return oResult;
	}

	JSON JSON::CreateAsObject()
	{
		JSON oResult;
		oResult.m_upRootValue = std::make_unique<Object>();
		return oResult;
	}

	JSON::JSON() : m_upRootValue(std::make_unique<Object>()) {}

	JSON::JSON(const JSON &other)
	{
		switch (other.root()->type())
		{
		case Value::Type::Array:
			m_upRootValue = std::make_unique<Array>(*dynamic_cast<const Array*>(other.root()));
			break;

		case Value::Type::Object:
			m_upRootValue = std::make_unique<Object>(*dynamic_cast<const Object*>(other.root()));
			break;
		}
	}

	bool JSON::saveToFile(const wchar_t *szFilename)
	{
		if (!m_upRootValue)
			return false;

		std::ofstream file(szFilename);
		if (!file)
			return false;


		std::string sValue = m_upRootValue->encode();

		file.exceptions(std::ios::badbit | std::ios::failbit);
		file.write(sValue.c_str(), sValue.length());

		file.close();
		return true;
	}

}