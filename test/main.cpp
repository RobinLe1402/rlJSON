#include <rlJSON/JSON.hpp>
#include <rlJSON/Array.hpp>
#include <rlJSON/Number.hpp>
#include <rlJSON/Object.hpp>
#include <rlJSON/String.hpp>

#include <rlJSON/SmartPointers.hpp>

int main(int argc, char* argv[])
{
	rlJSON::JSON json;
	rlJSON::Object &rootobj = *dynamic_cast<rlJSON::Object *>(json.root());

	auto testval = rlJSON::MakeUnique(5ui64);
	auto testarr = rlJSON::MakeUniqueArray();

	auto testarv1 = rlJSON::MakeUnique(1.0);
	auto testarv2 = rlJSON::MakeUnique(2ui64);
	auto testarv3 = rlJSON::MakeUnique(3.0);

	testarr->value().push_back(testarv1.get());
	testarr->value().push_back(testarv2.get());
	testarr->value().push_back(testarv3.get());

	rootobj.value()["testvalue"] = testval.get();
	rootobj.value()["testarray"] = testarr.get();

	json.saveToFile(LR"(E:\[TempDel]\test.json)");

	return 0;
}
