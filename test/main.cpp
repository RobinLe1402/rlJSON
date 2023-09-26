#include "rlJSON/Value.hpp"
#include <rlTextDLL/UTF8StringHelper.hpp>
#include <rlTextDLL/Windows.hpp>
#include <rlTextDLL/FileIO.hpp>
#include <iostream>

void NumberTest();

int main(int argc, char* argv[])
{
	rlJSON::Value oRoot{ rlJSON::Object() };
	auto &oRootData = *oRoot.object();

	oRootData[u8"testnull"]   = {};
	oRootData[u8"testarray"]  = rlJSON::Array({ 1, L"2", 3.1 });
	oRootData[u8"testbool"]   = false;
	oRootData[u8"testnumber"] = 12;
	oRootData[u8"teststring"] = L"ASDF";
	oRootData[u8"testobject"] = rlJSON::Object(
		{
			{ u8"named-item", 0 },
			{ u8"teststring", u8"Hello World!" }
		}
	);

	const char8_t szTestFilePath[] = u8R"(C:\[TempDel]\test.json)";

	if (oRoot.saveToFile(szTestFilePath))
#ifndef _WIN32
	std::cout << "Test JSON file \"" << (const char*)szTestFilePath << "\" successfully created.\n";
#else
	std::wcout << L"Test JSON file \"" << rlText::UTF8toUTF16(szTestFilePath) <<
		L"\" successfully created.\n";
#endif
	else
#ifndef _WIN32
		std::cout << "Error creating test JSON file \"" << (const char*)szTestFilePath << "\".\n";
#else
		std::wcout << L"Error creating test JSON file \"" << rlText::UTF8toUTF16(szTestFilePath) <<
			L"\".\n";
#endif

	return 0;
}

void NumberTest()
{
	rlJSON::Number no;
	std::cout << "Default: " << rlText::ToString(no.toString()) << "\n";

	no = 82953454.0;
	std::cout << "82953452.0 = " << rlText::ToString(no.toString()) << "\n";

	no = -no;
	std::cout << "* -1 = " << rlText::ToString(no.toString()) << "\n";

	no /= 2.0;
	std::cout << "/= 2.0 = " << rlText::ToString(no.toString()) << "\n";

	no /= 2.0;
	std::cout << "/= 2.0 = " << rlText::ToString(no.toString()) << "\n";

	no *= 200000000000000000000.0;
	std::cout << "*= 200000000000000000000.0 = " << rlText::ToString(no.toString()) << "\n";
}
