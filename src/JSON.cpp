#include "rlJSON/JSON.hpp"

#include <fstream>

namespace rlJSON
{

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