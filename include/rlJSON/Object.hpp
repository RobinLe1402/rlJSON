#pragma once
#ifndef RLJSON_OBJECT
#define RLJSON_OBJECT





#include "String.hpp"
#include "Value.hpp"
#include <map>



namespace rlJSON
{

	class Object : public Value
	{
	public: // methods

		Object();
		Object(const Object &) = default;
		Object(Object&&) = default;
		Object(std::map<String, Value *> oValues);
		~Object() = default;

		Object &operator=(const Object &) = default;
		Object &operator=(Object &&) = default;

		const std::map<String, Value *> &value() const { return m_oValues; }
		std::map<String, Value *> &value() { return m_oValues; }

		std::string encode() const;


	private: // variables

		std::map<String, Value *> m_oValues;

	};

}





#endif // RLJSON_OBJECT