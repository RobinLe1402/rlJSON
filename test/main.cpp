#include "rlJSON/Number.hpp"

int main(int argc, char* argv[])
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

	return 0;
}
