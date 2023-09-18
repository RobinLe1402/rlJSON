#pragma once
#ifndef RLJSON_STRING
#define RLJSON_STRING





#include <string>

#include "Value.hpp"



namespace rlJSON
{

	class String : public Value
	{
	public: // methods

		String() = default;
		String(const String &) = default;
		String(String&&) = default;
		String(const wchar_t *sz);
		String(const char *sz);
		~String() = default;

		String &operator=(const String &) = default;
		String &operator=(String &&) = default;

		int operator< (const String &other) const;
		int operator<=(const String &other) const;
		int operator> (const String &other) const;
		int operator>=(const String &other) const;
		int operator==(const String &other) const;
		int operator!=(const String &other) const;

		const std::wstring &value() const { return m_sValue; }

		std::string encode() const override;


	private: // methods

		void removeIllegalChars();


	private: // variables

		std::wstring m_sValue;

	};

}





#endif // RLJSON_STRING