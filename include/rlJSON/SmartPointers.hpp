#pragma once
#ifndef RLJSON_SMARTPOINTERS
#define RLJSON_SMARTPOINTERS





#include "Array.hpp"
#include "Boolean.hpp"
#include "Null.hpp"
#include "Number.hpp"
#include "Object.hpp"
#include "String.hpp"
#include <memory>



namespace rlJSON
{

#define DECLARE_SMART_PTR_MAKER(pointer_type, pointer_type_name, class_name, argtype, argname) \
	std::##pointer_type##_ptr<class_name> Make##pointer_type_name(argtype argname) \
	{\
		return std::make_##pointer_type##<class_name>(argname);\
	}

#define DECLARE_SMART_PTR_MAKERS(class_name, argtype, argname) \
	DECLARE_SMART_PTR_MAKER(unique, Unique, class_name, argtype, argname) \
	DECLARE_SMART_PTR_MAKER(shared, Shared, class_name, argtype, argname)



	DECLARE_SMART_PTR_MAKER(unique, UniqueArray, Array, void, )
	DECLARE_SMART_PTR_MAKER(shared, SharedArray, Array, void, )

	DECLARE_SMART_PTR_MAKERS(Boolean, bool, b)

	DECLARE_SMART_PTR_MAKER(unique, Unique, Null, void, )
	DECLARE_SMART_PTR_MAKER(shared, Shared, Null, void, )

	DECLARE_SMART_PTR_MAKERS(Number, uint64_t, i)
	DECLARE_SMART_PTR_MAKERS(Number, int64_t,  i)
	DECLARE_SMART_PTR_MAKERS(Number, double,   d)

	DECLARE_SMART_PTR_MAKER(unique, UniqueObject, Object, void, )
	DECLARE_SMART_PTR_MAKER(shared, SharedObject, Object, void, )

	DECLARE_SMART_PTR_MAKERS(String, const char *, sz)
	DECLARE_SMART_PTR_MAKERS(String, const wchar_t *, sz)



#undef DECLARE_SMART_PTR_MAKER
#undef DECLARE_SMART_PTR_MAKERS

}





#endif // RLJSON_SMARTPOINTERS