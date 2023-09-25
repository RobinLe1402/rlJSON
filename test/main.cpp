#include "rlJSON/Value.hpp"

void NumberTest();

int main(int argc, char* argv[])
{
	rlJSON::Value oRoot{ rlJSON::Object() };
	auto &oRootData = *oRoot.object();

	oRootData[L"testnull"]   = {};
	oRootData[L"testarray"]  = rlJSON::Array({ 1, L"2", 3.1 });
	oRootData[L"testbool"]   = false;
	oRootData[L"testnumber"] = 12;
	oRootData[L"teststring"] = L"ASDF";
	oRootData[L"testobject"] = rlJSON::Object(
		{
			{ L"named-item", 0 },
			{ L"teststring", L"Hello World!" }
		}
	);

	const wchar_t szTestFilePath[] = LR"(C:\[TempDel]\test.json)";

	if (oRoot.saveToFile(szTestFilePath))
		printf("Test JSON file \"%ls\" successfully created.\n", szTestFilePath);
	else
		printf("Error creating test JSON file \"%ls\".\n", szTestFilePath);

	return 0;
}

void NumberTest()
{
	rlJSON::Number no;
	printf("Default: %ls\n", no.toString().c_str());

	no = 82953454.0;
	printf("82953452.0 = %ls\n", no.toString().c_str());

	no = -no;
	printf("* -1 = %ls\n", no.toString().c_str());

	no /= 2.0;
	printf("/= 2.0 = %ls\n", no.toString().c_str());

	no /= 2.0;
	printf("/= 2.0 = %ls\n", no.toString().c_str());

	no *= 200000000000000000000.0;
	printf("*= 200000000000000000000.0 = %ls\n", no.toString().c_str());
}
