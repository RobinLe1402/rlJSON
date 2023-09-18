#pragma once
#ifndef RLJSON_ARRAY
#define RLJSON_ARRAY





#include "Value.hpp"
#include <vector>



namespace rlJSON
{

	class Array : public Value
	{
	public: // methods

		Array();
		Array(const Array &) = default;
		Array(Array&&) = default;
		Array(std::initializer_list<Value*> oValues);
		~Array() = default;

		Array &operator=(const Array &) = default;
		Array &operator=(Array &&) = default;

		const std::vector<Value *> &value() const { return m_oValues; }
		std::vector<Value *> &value() { return m_oValues; }

		std::string encode() const override;


	private: // variables

		std::vector<Value *> m_oValues;

	};

}





#endif // RLJSON_ARRAY