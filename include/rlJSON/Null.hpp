#pragma once
#ifndef RLJSON_NULL
#define RLJSON_NULL





#include "Value.hpp"



namespace rlJSON
{

	class Null : public Value
	{
	public: // methods

		Null();

		std::string encode() const;

	};

}





#endif // RLJSON_NULL