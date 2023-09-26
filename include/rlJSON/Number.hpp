#pragma once
#ifndef RLJSON_NUMBER
#define RLJSON_NUMBER





#include <rlTextDLL/Types.hpp>

#include <cstdint>
#include <string>



namespace rlJSON
{

	class Number final
	{
	public: // types

		enum class Type
		{
			UInt,
			Int,
			Float
		};


	public: // methods

		constexpr Number()              noexcept = default;
		constexpr Number(const Number&) noexcept = default;
		constexpr Number(Number&&)      noexcept = default;
		~Number()                                = default;

		Number(int i)       noexcept : Number((intmax_t)i) {}
		Number(uintmax_t i) noexcept;
		Number(intmax_t i)  noexcept;
		Number(double d)    noexcept;

		Number &operator=(const Number &) = default;
		Number &operator=(Number &&)      = default;

		Type type() const noexcept { return m_eType; }

		RLTEXT_UTF8STRING toString() const noexcept;

		uintmax_t asUInt()  const noexcept;
		intmax_t  asInt()   const noexcept;
		double    asFloat() const noexcept;

		bool isInt()   const noexcept { return !isFloat(); }
		bool isFloat() const noexcept { return m_eType == Type::Float; }

		explicit operator bool() const noexcept;
		bool operator!() const noexcept { return !((bool)*this); }

		explicit operator uintmax_t() const noexcept { return asUInt(); }
		explicit operator intmax_t()  const noexcept { return asInt(); }
		explicit operator double()    const noexcept { return asFloat(); }

		Number  operator+ (const Number &other) const noexcept;
		Number &operator+=(const Number &other) noexcept { return *this = *this + other; }
		Number  operator- (const Number &other) const noexcept;
		Number &operator-=(const Number &other) noexcept { return *this = *this - other; }
		Number  operator* (const Number &other) const noexcept;
		Number &operator*=(const Number &other) noexcept { return *this = *this * other; }
		Number  operator/ (const Number &other) const;
		Number &operator/=(const Number &other) { return *this = *this / other; }
		Number  operator% (const Number &other) const noexcept;
		Number &operator%=(const Number &other) noexcept { return *this = *this % other; }

		bool operator< (const Number &other) const noexcept
		{
			return this->asFloat() < other.asFloat();
		}
		bool operator<=(const Number &other) const noexcept
		{
			return this->asFloat() <= other.asFloat();
		}
		bool operator==(const Number &other) const noexcept;
		bool operator>=(const Number &other) const noexcept
		{
			return this->asFloat() >= other.asFloat();
		}
		bool operator> (const Number &other) const noexcept
		{
			return this->asFloat() > other.asFloat();
		}

		Number operator-() const noexcept;

		Number absolute() const noexcept;


	private: // types

		union ValueUnion
		{
			uintmax_t iUnsigned;
			intmax_t  iSigned;
			double    dFloat;
		};


	private: // variables

		Type m_eType        = Type::UInt;
		ValueUnion m_uValue = { 0 };

	};

}





#endif // RLJSON_NUMBER