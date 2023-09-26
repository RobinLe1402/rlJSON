#include "rlJSON/Number.hpp"

#include <rlTextDLL/UTF8StringHelper.hpp>

#include <cmath>
#include <iomanip>
#include <sstream>

namespace rlJSON
{

	Number::Number(uintmax_t i) noexcept : m_eType(Type::UInt) { m_uValue.iUnsigned = i; }

	Number::Number(intmax_t i) noexcept : m_eType(Type::Int)
	{
		if (i < 0)
			m_uValue.iSigned = i;
		else
		{
			m_eType = Type::UInt;
			m_uValue.iUnsigned = uintmax_t(i);
		}
	}

	Number::Number(double d) noexcept : m_eType(Type::Float)
	{
		double dTMP;
		if (std::modf(d, &dTMP) == 0.0)
		{
			// no decimal part --> could be integer-compatible

			if (dTMP >= 0)
			{
				if (dTMP <= UINTMAX_MAX)
				{
					m_eType = Type::UInt;
					m_uValue.iUnsigned = uintmax_t(dTMP);
					return;
				}
			}
			else // dTMP < 0
			{
				if (dTMP >= INTMAX_MIN)
				{
					m_eType = Type::Int;
					m_uValue.iSigned = intmax_t(dTMP);
					return;
				}
			}
		}
		
		m_uValue.dFloat = d;
	}

	RLTEXT_UTF8STRING Number::toString() const noexcept
	{
		switch (m_eType)
		{
		case Type::UInt:
			return rlText::ToU8String(std::to_string(m_uValue.iUnsigned));
		case Type::Int:
			return rlText::ToU8String(std::to_string(m_uValue.iSigned));
		case Type::Float:
		{
			std::stringstream ss;
			ss << std::defaultfloat << std::setprecision(17) << m_uValue.dFloat;
			const auto sASCIIResult = ss.str();
			RLTEXT_UTF8STRING sResult(sASCIIResult.length(), 0);
			for (size_t i = 0; i < sASCIIResult.length(); ++i)
			{
				sResult[i] = sASCIIResult[i];
			}

			// if not exponential...
			if (sResult.find('e') == sResult.npos)
			{
				// ... and if there is a decimal part...
				double dTMP;
				if (std::modf(m_uValue.dFloat, &dTMP) != 0.0)
				{
					// ... remove all the trailing zeros.
					sResult = sResult.substr(0, sResult.find_last_not_of('0') + 1);
				}
			}
			return sResult;
		}
		}

		return u8""; // to avoid compiler warning
	}

	uintmax_t Number::asUInt() const noexcept
	{
		switch (m_eType)
		{
		case Type::UInt:
			return m_uValue.iUnsigned;
		case Type::Int:
			return uintmax_t(m_uValue.iSigned);
		case Type::Float:
			return uintmax_t(m_uValue.dFloat);
		}

		return 0; // to avoid compiler warning
	}

	intmax_t Number::asInt() const noexcept
	{
		switch (m_eType)
		{
		case Type::UInt:
			return intmax_t(m_uValue.iUnsigned);
		case Type::Int:
			return m_uValue.iSigned;
		case Type::Float:
			return intmax_t(m_uValue.dFloat);
		}

		return 0; // to avoid compiler warning
	}

	double Number::asFloat() const noexcept
	{
		switch (m_eType)
		{
		case Type::UInt:
			return double(m_uValue.iUnsigned);
		case Type::Int:
			return double(m_uValue.iSigned);
		case Type::Float:
			return m_uValue.dFloat;
		}

		return 0.0; // to avoid compiler warning
	}

	Number::operator bool() const noexcept
	{
		switch (m_eType)
		{
		case Type::UInt:
			return m_uValue.iUnsigned != 0;
		case Type::Int:
			return m_uValue.iSigned   != 0;
		case Type::Float:
			return m_uValue.dFloat    != 0.0;
		}

		return false; // to avoid compiler warning
	}

	Number Number::operator+(const Number &other) const noexcept
	{
		// one of the two is float --> return float
		if (m_eType == Type::Float || other.m_eType == Type::Float)
			return Number(this->asFloat() + other.asFloat());

		// both are integers

		// other is signed
		if (other.m_eType == Type::Int)
			return *this - intmax_t(other.m_uValue.iSigned * -1);

		// other is unsigned
		switch (m_eType)
		{
		case Type::UInt: // UInt + UInt

			// 0 <= value <= UINTMAX_MAX
			if (UINTMAX_MAX - this->m_uValue.iUnsigned >= other.m_uValue.iUnsigned)
				return uintmax_t(this->m_uValue.iUnsigned - other.m_uValue.iUnsigned);

			// UINTMAX_MAX < value
			return this->asFloat() + other.m_uValue.iUnsigned;

		case Type::Int: // Int + UInt
		{
			const uintmax_t iThisAbs = uintmax_t(this->m_uValue.iSigned * -1);

			// INTMAX_MIN <= value < 0
			if (other.m_uValue.iUnsigned < iThisAbs)
				return intmax_t(this->m_uValue.iSigned + other.m_uValue.iUnsigned);

			// 0 <= value <= UINTMAX_MAX
			return uintmax_t(this->m_uValue.iSigned + other.m_uValue.iUnsigned);
		}
		}

		return 0; // to avoid compiler warning
	}

	Number Number::operator-(const Number &other) const noexcept
	{
		// one of the two is float --> return float
		if (m_eType == Type::Float || other.m_eType == Type::Float)
			return Number(this->asFloat() - other.asFloat());

		// both are integers

		// other is signed
		if (other.m_eType == Type::Int)
			return *this + intmax_t(other.m_uValue.iSigned * -1);

		// other is unsigned
		switch (m_eType)
		{
		case Type::UInt: // UInt - UInt
		{
			// 0 <= value < UINTMAX_MAX
			if (other.m_uValue.iUnsigned <= this->m_uValue.iUnsigned)
				return uintmax_t(this->m_uValue.iUnsigned - other.m_uValue.iUnsigned);

			const uintmax_t iResultPositive = other.m_uValue.iUnsigned - this->m_uValue.iUnsigned;

			// INTMAX_MIN <= value < 0
			if (iResultPositive <= uintmax_t(INTMAX_MIN) * -1)
				return intmax_t(iResultPositive * -1);

			// value < INTMAX_MIN
			return double(iResultPositive * -1.0);
		}

		case Type::Int: // Int - UInt
			// INTMAX_MIN <= value < 0
			if (uintmax_t((INTMAX_MIN - this->m_uValue.iSigned) * -1) >= other.m_uValue.iUnsigned)
				return intmax_t(this->m_uValue.iSigned - other.m_uValue.iUnsigned);

			// value < INTMAX_MIN
			return this->asFloat() - other.m_uValue.iUnsigned;
		}

		return 0; // to avoid compiler warning
	}

	Number Number::operator*(const Number &other) const noexcept
	{
		// one of the two is zero --> return zero
		if (!*this || !other)
			return 0;

		// one of the two is float --> return float
		if (this->m_eType == Type::Float || other.m_eType == Type::Float)
			return this->asFloat() * other.asFloat();

		// both are non-zero integers

		const uintmax_t iThisAbs =
			(this->m_eType == Type::UInt)
			? this->m_uValue.iUnsigned
			: uintmax_t(this->m_uValue.iSigned * -1);
		const uintmax_t iOtherAbs =
			(other.m_eType == Type::UInt)
			? other.m_uValue.iUnsigned
			: uintmax_t(other.m_uValue.iSigned * -1);

		if (this->m_eType == other.m_eType)
		{
			// same sign --> result is positive

			if (UINTMAX_MAX / iOtherAbs >= iThisAbs)
				return uintmax_t(iThisAbs * iOtherAbs);
			else
				return double(iThisAbs) * iOtherAbs;
		}
		else
		{
			// different signs --> result is negative

			if (uintmax_t(INTMAX_MIN) * -1 / iOtherAbs >= iThisAbs)
				return intmax_t(iThisAbs * iOtherAbs * -1);
			else
				return double(iThisAbs) * iOtherAbs * -1.0;
		}
	}

	Number Number::operator/(const Number &other) const
	{
		// one of the two is float --> return float
		if (this->m_eType == Type::Float || other.m_eType == Type::Float)
			return this->asFloat() / other.asFloat();

		// both are integers

		const uintmax_t iThisAbs =
			(this->m_eType == Type::UInt)
			? this->m_uValue.iUnsigned
			: uintmax_t(this->m_uValue.iSigned * -1);
		const uintmax_t iOtherAbs =
			(other.m_eType == Type::UInt)
			? other.m_uValue.iUnsigned
			: uintmax_t(other.m_uValue.iSigned * -1);

		if (iThisAbs % iOtherAbs) // no multiple of divisor --> float
			return (this->asFloat() / other.asFloat());
		if (this->m_eType == other.m_eType)
			return iThisAbs / iOtherAbs;
		return intmax_t((iThisAbs / iOtherAbs) * -1);
	}

	Number Number::operator%(const Number &other) const noexcept
	{
		// one of the two is float --> return float
		if (this->m_eType == Type::Float || other.m_eType == Type::Float)
			return std::fmod(this->asFloat(), other.asFloat());

		const uintmax_t iOtherAbs = (other.m_eType == Type::UInt)
			? other.m_uValue.iUnsigned
			: uintmax_t(other.m_uValue.iSigned * -1);

		switch (m_eType)
		{
		case Type::UInt:
			return m_uValue.iUnsigned % iOtherAbs;
		case Type::Int:
			return m_uValue.iSigned % iOtherAbs;
		}

		return 0; // to avoid compiler warning
	}

	bool Number::operator==(const Number &other) const noexcept
	{
		if (this->m_eType != other.m_eType)
			return false;

		switch (m_eType)
		{
		case Type::UInt:
			return m_uValue.iUnsigned == other.m_uValue.iUnsigned;
		case Type::Int:
			return m_uValue.iSigned   == other.m_uValue.iSigned;
		case Type::Float:
			return m_uValue.dFloat    == other.m_uValue.dFloat;
		}

		return false; // to avoid compiler warning
	}

	Number Number::operator-() const noexcept
	{
		switch (m_eType)
		{
		case Type::UInt:
			if (m_uValue.iUnsigned <= uintmax_t(INTMAX_MIN) * -1)
				return intmax_t(m_uValue.iUnsigned) * -1;
			else
				return double(m_uValue.iUnsigned) * -1;

		case Type::Int:
			return uintmax_t(m_uValue.iSigned * -1);

		case Type::Float:
			return m_uValue.dFloat * -1;
		}

		return 0; // to avoid compiler warning
	}

	Number Number::absolute() const noexcept
	{
		if (*this >= 0)
			return *this;
		else
			return -(*this);
	}

}