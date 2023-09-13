#pragma once
#ifndef RLJSON_NUMBER
#define RLJSON_NUMBER





// TODO: RegEx
// (-?(?:0|[1-9]\d*))((?:\.\d+)?)((?:e[-+]?\d+)?)
// Group 1: int part, including optional -
// Group 2: decimal part, including period (optional, might be empty string)
// Group 3: exponent, including e/E and optional + or -



#include "Value.hpp"



namespace rlJSON
{

	class Number : public Value
	{
	public: // methods

		Number() = default;
		Number(const Number &) = default;
		Number(Number&&) = default;
		Number(double d);
		~Number() = default;

		Number &operator=(const Number &) = default;
		Number &operator=(Number &&) = default;

		double value() const { return m_dValue; }

		std::string encode() const override;


	private: // variables

		double m_dValue = 0;

	};

}





#endif // RLJSON_NUMBER