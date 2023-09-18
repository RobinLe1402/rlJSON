#include <string>

#include "../private-includes/Tools.hpp"

using namespace std::string_literals;

std::string AddIndent(std::string s, bool bFirstRow)
{
	if (s.empty())
		return "";

	std::string sResult;

	size_t iLineCount = 1 + std::count(s.begin(), s.end(), '\n');
	sResult.reserve(iLineCount);
	if (bFirstRow)
		sResult = '\t';
	sResult += s;
	for (size_t i = sResult.length(); i > 1; --i)
	{
		if (sResult[i - 1] == '\n')
			sResult.insert(sResult.begin() + i, '\t');
	}

	return sResult;
}