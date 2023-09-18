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
	public: // types

		enum class Type
		{
			UInt,
			Int,
			Float
		};


	public: // methods

		Number() = default;
		Number(const Number &) = default;
		Number(Number&&) = default;
		Number(uint64_t i);
		Number(int64_t i);
		Number(double f);
		~Number() = default;

		Number &operator=(const Number &) = default;
		Number &operator=(Number &&) = default;

		Type numberType() const { return m_eType; }

		uint64_t valueUInt()  const { return m_iUIntValue;  }
		int64_t  valueInt()   const { return m_iIntValue;   }
		double   valueFloat() const { return m_dFloatValue; }

		std::string encode() const override;


	private: // variables

		Type m_eType = Type::UInt;

		uint64_t m_iUIntValue = 0;
		int64_t m_iIntValue   = 0;
		double m_dFloatValue  = 0;

	};

}





#endif // RLJSON_NUMBER