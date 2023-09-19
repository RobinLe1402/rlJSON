#include "rlJSON/Value.hpp"
#include "rlText/Core.hpp"

#include <algorithm>
#include <regex>

using namespace std::string_literals;

namespace rlJSON
{

	namespace
	{

		std::wstring Indent(std::wstring &&rval)
		{
			std::wstring sResult = std::move(rval);

			if (sResult.length() == 0)
				return sResult;

			// 1. count lines
			size_t iLineCount = 1;
			for (auto c : sResult)
			{
				if (c == '\n')
					++iLineCount;
			}

			// 2. indent the lines
			sResult.reserve(sResult.length() + iLineCount);
			sResult.insert(0, 1, L'\t'); // start of text
			for (size_t i = sResult.length(); i > 1; --i)
			{
				const auto c = sResult[i - 1];
				if (c == '\n')
					sResult.insert(i, 1, L'\t');
			}

			return sResult;
		}

		std::wstring EncodeString(const std::wstring &s)
		{
			std::wstring sEncoded = s;

			const std::map<char, char> oEscapeChars =
			{
				{ '"',  '"'  },
				{ '\\', '\\' },
				{ '/',  '/'  },
				{ '\b', 'b'  },
				{ '\f', 'f'  },
				{ '\n', 'n'  },
				{ '\r', 'r'  },
				{ '\t', 't'  }
			};

			// 1. count characters to be escaped
			size_t iEscapedCount = 0;
			for (wchar_t c : sEncoded)
			{
				if (c & 0xFF00)
					continue; // only ASCII characters are escaped

				const auto it = oEscapeChars.find((char)c);
				if (it != oEscapeChars.end())
					++iEscapedCount;
			}

			// 2. escape the characters
			sEncoded.reserve(sEncoded.length() + iEscapedCount + 2);
			for (size_t i = sEncoded.length(); i > 0; --i)
			{
				const wchar_t c = sEncoded[i - 1];

				if (c & 0xFF00)
					continue; // only ASCII characters are escaped

				const auto it = oEscapeChars.find((char)c);
				if (it != oEscapeChars.end())
				{
					sEncoded[i - 1] = L'\\';
					sEncoded.insert(i, 1, it->second);
				}
			}

			sEncoded.insert(0, 1, L'"');
			sEncoded += L'"';

			return sEncoded;
		}

	}





	Value::Value(const Array &val) noexcept :
		m_eType(Type::Array),
		m_oValue_Array(val) {}

	Value::Value(Array &&rval) noexcept :
		m_eType(Type::Array),
		m_oValue_Array(std::move(rval)) {}

	Value::Value(bool val) noexcept :
		m_eType(Type::Boolean),
		m_bValue_Boolean(val) {}

	Value::Value(const Number &val) noexcept :
		m_eType(Type::Number),
		m_oValue_Number(val) {}

	Value::Value(Number &&rval) noexcept :
		m_eType(Type::Number),
		m_oValue_Number(std::move(rval)) {}

	Value::Value(const Object &val) noexcept :
		m_eType(Type::Object),
		m_oValue_Object(val) {}

	Value::Value(Object &&rval) noexcept :
		m_eType(Type::Object),
		m_oValue_Object(std::move(rval)) {}

	Value::Value(const wchar_t *sz) noexcept :
		m_eType(Type::String),
		m_sValue_String(sz) {}

	Value::Value(const std::wstring &val) noexcept :
		m_eType(Type::String),
		m_sValue_String(val) {}

	Value::Value(std::wstring &&rval) noexcept :
		m_eType(Type::String),
		m_sValue_String(std::move(rval)) {}

	Value &Value::operator=(const Value &other) noexcept
	{
		clear();

		switch (other.m_eType)
		{
		case Type::Array:
			m_oValue_Array = other.m_oValue_Array;
			break;
			
		case Type::Boolean:
			m_bValue_Boolean = other.m_bValue_Boolean;
			break;

		default:
		case Type::Null:
			break;

		case Type::Number:
			m_oValue_Number = other.m_oValue_Number;
			break;

		case Type::Object:
			m_oValue_Object = other.m_oValue_Object;
			break;

		case Type::String:
			m_sValue_String = other.m_sValue_String;
			break;
		}

		return *this;
	}

	Value &Value::operator=(Value &&rval) noexcept
	{
		clear();

		m_eType = rval.m_eType;
		switch (rval.m_eType)
		{
		case Type::Array:
			m_oValue_Array = std::move(rval.m_oValue_Array);
			break;

		case Type::Boolean:
			m_bValue_Boolean = std::move(rval.m_bValue_Boolean);
			break;

		default:
		case Type::Null:
			break;

		case Type::Number:
			m_oValue_Number = std::move(rval.m_oValue_Number);
			break;

		case Type::Object:
			m_oValue_Object = std::move(rval.m_oValue_Object);
			break;

		case Type::String:
			m_sValue_String = std::move(rval.m_sValue_String);
			break;
		}

		return *this;
	}

	Array Value::toArray() const noexcept
	{
		switch (m_eType)
		{
		case Type::Array:
			return m_oValue_Array;
		case Type::Boolean:
			return Array({ *this });
		case Type::Null:
			return Array();
		case Type::Number:
			return Array({ *this });
		case Type::Object:
		{
			Array o;
			o.reserve(m_oValue_Object.size());
			for (const auto &it : m_oValue_Object)
			{
				o.push_back(EncodeString(it.first) + L" => "s + it.second.toString(true));
			}
			return o;
		}
		case Type::String:
		{
			Array o;
			o.reserve(m_sValue_String.length());
			for (size_t i = 0; i < m_sValue_String.length(); ++i)
			{
				const wchar_t c = m_sValue_String[i];
				if ((c & 0xFC00) == 0xD800)
				{
					++i;
					std::wstring s(2, ' ');
					s[0] = c;
					s[1] = m_sValue_String[i];
					o.push_back(std::move(s));
				}
				else
					o.push_back(c);
			}
			o.shrink_to_fit();
			return o;
		}
		}

		return {};
	}

	bool Value::toBool() const noexcept
	{
		switch (m_eType)
		{
		case Type::Array:
			return m_oValue_Array.size() > 0;
		case Type::Boolean:
			return m_bValue_Boolean;
		case Type::Null:
			return false;
		case Type::Number:
			return (bool)m_oValue_Number;
		case Type::Object:
			return m_oValue_Object.size() > 0;
		case Type::String:
			return m_sValue_String.length() > 0;
		}

		return false; // to avoid compiler warning
	}

	Number Value::toNumber() const noexcept
	{
		switch (m_eType)
		{
		case Type::Array:
			return m_oValue_Array.size();
		case Type::Boolean:
			return m_bValue_Boolean ? 1 : 0;
		case Type::Null:
			return 0;
		case Type::Number:
			return m_oValue_Number;
		case Type::Object:
			return m_oValue_Object.size();
		case Type::String:
		{
			// positive int
			std::wregex regex(LR"REGEX(\d+)REGEX");
			if (std::regex_match(m_sValue_String, regex))
			{
				// approximated value as float
				double dValue = 0;
				double dMultiplicator = 1;
				for (size_t i = 0; i < m_sValue_String.length(); ++i)
				{
					dValue +=
						(m_sValue_String[m_sValue_String.length() - 1 - i] - '0') * dMultiplicator;
					dMultiplicator *= 10;
				}

				if (dValue > UINTMAX_MAX)
					return dValue;
				else
				{
					uintmax_t iValue = 0;
					uintmax_t iMultiplicator = 1;
					for (size_t i = 0; i < m_sValue_String.length(); ++i)
					{
						iValue +=
							uintmax_t(m_sValue_String[m_sValue_String.length() - 1 - i] - '0') *
							iMultiplicator;
						iMultiplicator *= 10;
					}
				}
			}

			// negative int
			regex = LR"REGEX(-\d+)REGEX";
			if (std::regex_match(m_sValue_String, regex))
			{
				// approximated value as float
				double dValue = 0;
				double dMultiplicator = 1;
				for (size_t i = 0; i < m_sValue_String.length() - 1; ++i)
				{
					dValue -=
						(m_sValue_String[m_sValue_String.length() - 1 - i] - '0') * dMultiplicator;
					dMultiplicator *= 10;
				}

				if (dValue < INTMAX_MIN)
					return dValue;
				else
				{
					uintmax_t iValue = 0;
					uintmax_t iMultiplicator = 1;
					for (size_t i = 0; i < m_sValue_String.length() - 1; ++i)
					{
						iValue +=
							uintmax_t(m_sValue_String[m_sValue_String.length() - 1 - i] - '0') *
							iMultiplicator;
						iMultiplicator *= 10;
					}
				}
			}

			// float
			regex = LR"REGEX(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)REGEX";
			if (std::regex_match(m_sValue_String, regex))
			{
				std::string s;
				s.reserve(m_sValue_String.length());
				for (size_t i = 0; i < m_sValue_String.length(); ++i)
				{
					s += (char)m_sValue_String[i];
				}

				return atof(s.c_str());
			}
		}
		}

		return 0; // to avoid compiler warning
	}

	Object Value::toObject() const noexcept
	{
		switch (m_eType)
		{
		case Type::Array:
		{
			Object o;
			for (size_t i = 0; i < m_oValue_Array.size(); ++i)
			{
				o[std::to_wstring(i)] = m_oValue_Array[i];
			}
			return o;
		}
		case Type::Boolean:
		case Type::Null:
		case Type::Number:
		case Type::String:
			return { { L"value", *this }};

		case Type::Object:
			return m_oValue_Object;

		}

		return {}; // to avoid compiler warning
	}

	std::wstring Value::toString(bool bEncodeString) const noexcept
	{
		switch (m_eType)
		{
		case Type::Array:
		{
			std::wstring sResult = L"[";
			for (const auto &val : m_oValue_Array)
			{
				sResult += L"\n"s + Indent(val.toString(true)) + L",";
			}
			if (m_oValue_Array.size() > 0)
				sResult[sResult.length() - 1] = L'\n'; // replace last comma with linebreak
			sResult += L"]";

			return sResult;
		}

		case Type::Boolean:
			return m_bValue_Boolean ? L"true" : L"false";

		case Type::Null:
			return L"null";

		case Type::Number:
			return m_oValue_Number.toString();

		case Type::Object:
		{
			std::wstring sResult = L"{";
			for (const auto &val : m_oValue_Object)
			{
				sResult += L"\n"s + L"\t"s + EncodeString(val.first) + L":\n" +
					Indent(Indent(val.second.toString(true))) + L",";
			}
			if (m_oValue_Object.size() > 0)
				sResult[sResult.length() - 1] = L'\n'; // replace last comma with linebreak
			sResult += L"}";

			return sResult;
		}

		case Type::String:
			if (!bEncodeString)
				return m_sValue_String;
			else
				return EncodeString(m_sValue_String);
		}

		return L""; // to avoid compiler warning
	}

	const Array *Value::array() const noexcept
	{
		if (m_eType == Type::Array)
			return &m_oValue_Array;
		else
			return nullptr;
	}

	Array *Value::array() noexcept
	{
		if (m_eType == Type::Array)
			return &m_oValue_Array;
		else
			return nullptr;
	}

	bool Value::boolean() const noexcept
	{
		if (m_eType == Type::Boolean)
			return &m_bValue_Boolean;
		else
			return false;
	}

	const Number *Value::number() const noexcept
	{
		if (m_eType == Type::Number)
			return &m_oValue_Number;
		else
			return nullptr;
	}

	Number *Value::number() noexcept
	{
		if (m_eType == Type::Number)
			return &m_oValue_Number;
		else
			return nullptr;
	}

	const Object *Value::object() const noexcept
	{
		if (m_eType == Type::Object)
			return &m_oValue_Object;
		else
			return nullptr;
	}

	Object *Value::object() noexcept
	{
		if (m_eType == Type::Object)
			return &m_oValue_Object;
		else
			return nullptr;
	}

	const std::wstring *Value::string() const noexcept
	{
		if (m_eType == Type::String)
			return &m_sValue_String;
		else
			return nullptr;
	}

	std::wstring *Value::string() noexcept
	{
		if (m_eType == Type::String)
			return &m_sValue_String;
		else
			return nullptr;
	}

	void Value::clear() noexcept
	{
		m_eType = Type::Null;
		m_oValue_Array.clear();
		m_bValue_Boolean = false;
		m_oValue_Number  = 0;
		m_oValue_Object.clear();
		m_sValue_String.clear();
	}

	bool Value::loadFromString(const wchar_t *& szJSON) noexcept
	{
		return false; // TODO
	}

	bool Value::loadFromFile(const wchar_t *szPath) noexcept
	{
		std::vector<std::wstring> oLines;
		rlText::ReadAllLines(szPath, oLines);

		if (oLines.size() == 0)
			return false;

		std::wstring sText;
		size_t iLen = oLines.size();
		for (const auto &s : oLines)
		{
			iLen += s.length();
		}

		sText.reserve(iLen);
		for (const auto &s : oLines)
		{
			sText += s + L"\n";
		}
		sText.erase(sText.length() - 1);
		auto sz = sText.c_str();

		if (!loadFromString(sz))
			return false;

		// check if EOF comes right after loaded element
		if (sz[0] != 0)
		{
			clear();
			return false;
		}
		return true;
	}

	bool Value::saveToString(std::wstring &sJSON) const noexcept
	{
		if (m_eType != Type::Array && m_eType != Type::Object)
			return false;

		sJSON = toString(true);
		return true;
	}

	bool Value::saveToFile(const wchar_t *szPath) const noexcept
	{
		std::wstring s;
		if (!saveToString(s))
			return false;

		return rlText::WriteTextFile(szPath, { s });
	}

}