#include "rlJSON/Value.hpp"
#include <rlTextDLL/Decode.hpp>
#include <rlTextDLL/Encode.hpp>
#include <rlTextDLL/FileIO.hpp>
#include <rlTextDLL/Types.h>
#include <rlTextDLL/UTF8StringHelper.hpp>
#include <rlTextDLL/Windows.hpp>

#include <algorithm>
#include <regex>

using namespace std::string_literals;

namespace rlJSON
{

	namespace
	{

		std::u8string Indent(std::u8string &&rval)
		{
			std::u8string sResult = std::move(rval);

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
			sResult.insert(0, 1, '\t'); // start of text
			for (size_t i = sResult.length(); i > 1; --i)
			{
				const auto c = sResult[i - 1];
				if (c == '\n')
					sResult.insert(i, 1, '\t');
			}

			return sResult;
		}

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

		std::u8string EncodeString(const std::u8string &s)
		{
			std::u8string sEncoded = s;

			// 1. count characters to be escaped
			size_t iEscapedCount = 0;
			for (char8_t c : sEncoded)
			{
				if (c & 0x80)
					continue; // only ASCII characters are escaped

				const auto it = oEscapeChars.find((char)c);
				if (it != oEscapeChars.end())
					++iEscapedCount;
			}

			// 2. escape the characters
			sEncoded.reserve(sEncoded.length() + iEscapedCount + 2);
			for (size_t i = sEncoded.length(); i > 0; --i)
			{
				const char8_t c = sEncoded[i - 1];

				if (c & 0x80)
					continue; // only ASCII characters are escaped

				const auto it = oEscapeChars.find((char)c);
				if (it != oEscapeChars.end())
				{
					sEncoded[i - 1] = '\\';
					sEncoded.insert(i, 1, it->second);
				}
			}

			sEncoded.insert(0, 1, '"');
			sEncoded += '"';

			return sEncoded;
		}

		bool IsWordChar(char c)
		{
			return
				c == '_' || ((c ^ 0x80) && (std::isalpha(c) || std::isdigit(c)));
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

	Value::Value(const char8_t *sz) noexcept :
		m_eType(Type::String),
		m_sValue_String(sz) {}

	Value::Value(const std::u8string &val) noexcept :
		m_eType(Type::String),
		m_sValue_String(val) {}

	Value::Value(std::u8string &&rval) noexcept :
		m_eType(Type::String),
		m_sValue_String(std::move(rval)) {}

	Value &Value::operator=(const Value &other) noexcept
	{
		clear();

		m_eType = other.m_eType;
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
				o.push_back(EncodeString(it.first) + u8" => "s + it.second.toString(true));
			}
			return o;
		}
		case Type::String:
		{
			Array o;
			o.reserve(m_sValue_String.length());
			for (size_t i = 0; i < m_sValue_String.length(); ++i)
			{
				char32_t chDummy;
				const auto len = rlText::DecodeUTF8(&m_sValue_String[i], chDummy);

				o.push_back(m_sValue_String.substr(0, len));
				i += (size_t)len - 1;
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
			const auto sASCII = rlText::ToString(m_sValue_String);

			// positive int
			std::regex regex(R"REGEX(\d+)REGEX");
			if (std::regex_match(sASCII, regex))
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

					return iValue;
				}
			}

			// negative int
			regex = R"REGEX(-\d+)REGEX";
			if (std::regex_match(sASCII, regex))
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
			regex = R"REGEX(-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)REGEX";
			if (std::regex_match(sASCII, regex))
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
				o[rlText::ToU8String(std::to_string(i))] = m_oValue_Array[i];
			}
			return o;
		}
		case Type::Boolean:
		case Type::Null:
		case Type::Number:
		case Type::String:
			return { { u8"value", *this }};

		case Type::Object:
			return m_oValue_Object;

		}

		return {}; // to avoid compiler warning
	}

	std::u8string Value::toString(bool bEncodeString) const noexcept
	{
		switch (m_eType)
		{
		case Type::Array:
		{
			std::u8string sResult = u8"[";
			for (const auto &val : m_oValue_Array)
			{
				sResult += u8"\n"s + Indent(val.toString(true)) + u8",";
			}
			if (m_oValue_Array.size() > 0)
				sResult[sResult.length() - 1] = u8'\n'; // replace last comma with linebreak
			sResult += u8"]";

			return sResult;
		}

		case Type::Boolean:
			return m_bValue_Boolean ? u8"true" : u8"false";

		case Type::Null:
			return u8"null";

		case Type::Number:
			return m_oValue_Number.toString();

		case Type::Object:
		{
			std::u8string sResult = u8"{";
			for (const auto &val : m_oValue_Object)
			{
				sResult += u8"\n\t"s + EncodeString(val.first) + u8":\n" +
					Indent(Indent(val.second.toString(true))) + u8",";
			}
			if (m_oValue_Object.size() > 0)
				sResult[sResult.length() - 1] = '\n'; // replace last comma with linebreak
			sResult += u8"}";

			return sResult;
		}

		case Type::String:
			if (!bEncodeString)
				return m_sValue_String;
			else
				return EncodeString(m_sValue_String);
		}

		return u8""; // to avoid compiler warning
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
			return m_bValue_Boolean;
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

	const std::u8string *Value::string() const noexcept
	{
		if (m_eType == Type::String)
			return &m_sValue_String;
		else
			return nullptr;
	}

	std::u8string *Value::string() noexcept
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

	namespace
	{
		void SkipWhitespace(const char8_t *&sz)
		{
			while (*sz <= 0xFF && isspace(*sz))
				++sz;
		}

		bool ParseArray(const char8_t *&sz, Array &oDest)
		{
			if (*sz != u8'[')
				return false;

			oDest.clear();

			++sz;
			SkipWhitespace(sz);
			while (*sz)
			{
				if (*sz == u8']')
				{
					++sz;
					return true;
				}

				Value o;
				if (!o.loadFromString(sz))
					return false; // value could not be parsed

				oDest.push_back(o);

				if (*sz == u8',')
					++sz;
				else if (*sz != u8']')
					return false; // invalid syntax. After a value must be either a "," or a "]".
			}

			return true;
		}

		bool ParseBoolean(const char8_t *&sz, bool &bDest)
		{
			if (sz[0] == 't')
			{
				if (
					sz[1] == 'r' &&
					sz[2] == 'u' &&
					sz[3] == 'e' &&
					!IsWordChar(sz[4])
				)
				{
					bDest = true;
					sz   += 4;
					return true;
				}

				return false;
			}
			else if (sz[0] == 'f')
			{
				if (
					sz[1] == 'a' &&
					sz[2] == 'l' &&
					sz[3] == 's' &&
					sz[4] == 'e' &&
					!IsWordChar(sz[5])
				)
				{
					bDest = false;
					sz   += 5;
					return true;
				}

				return false;
			}

			return false;
		}

		bool ParseNull(const char8_t *&sz)
		{
			if (sz[0] == 'n' &&
				sz[1] == 'u' &&
				sz[2] == 'l' &&
				sz[3] == 'l' &&
				!IsWordChar(sz[4]))
			{
				sz += 4;
				return true;
			}

			return false;
		}

		bool ParseNumber(const char8_t *&sz, Number &oDest)
		{
			std::regex regex(R"REGEX(^-?\d+(?:\.\d+)?(?:e[+-]?\d+)?)REGEX");
			std::cmatch match;
			if (!std::regex_search(reinterpret_cast<const char *>(sz), match, regex))
				return false; // no number found
			
			Value o = rlText::ToU8String(match[0].str());
			sz += match[0].length();
			oDest = o.toNumber();
			return true;
		}

		bool ParseObject(const char8_t *&sz, Object &oDest)
		{
			if (*sz != u8'{')
				return false;

			oDest.clear();

			++sz;
			SkipWhitespace(sz);
			while (*sz)
			{
				if (*sz == u8'}')
				{
					++sz;
					return true;
				}

				Value o;
				if (!o.loadFromString(sz))
					return false; // value could not be parsed

				if (o.type() != Value::Type::String)
					return false; // string expected

				std::u8string sKey = o.toString();
				
				SkipWhitespace(sz);
				if (*sz != u8':')
					return false; // after the value name always comes the ":"
				++sz;

				SkipWhitespace(sz);
				if (!o.loadFromString(sz))
					return false; // value couldn't be parsed

				oDest[sKey] = o;

				if (*sz == u8',')
					++sz;
				else if (*sz != u8'}')
					return false; // invalid syntax. After a value must be either a "," or a "}".
			}

			return true;
		}

		bool ParseHexDigit(char8_t c, unsigned char &iDest)
		{
			if (c >= u8'0' && c <= u8'9')
			{
				iDest = c - u8'0';
				return true;
			}
			else if (c >= 'A' && c <= 'F')
			{
				iDest = c - u8'A' + 0xA;
				return true;
			}
			else if (c >= 'a' && c <= 'f')
			{
				iDest = c - u8'a' + 0xA;
				return true;
			}

			return false;
		}

		bool ParseString(const char8_t *&sz, std::u8string &sDest)
		{
			if (*sz != u8'"')
				return false;

			sDest.clear();
			while (*(++sz))
			{
				// handle escape sequences
				if (*sz == u8'\\')
				{
					++sz;
					if (*sz == u8'u')
					{
						unsigned char iHexValues[4]{};
						if (!ParseHexDigit(sz[1], iHexValues[0]) ||
							!ParseHexDigit(sz[2], iHexValues[1]) ||
							!ParseHexDigit(sz[3], iHexValues[2]) ||
							!ParseHexDigit(sz[4], iHexValues[3]))
							return false; // invalid Unicode escape sequence

						char32_t c = iHexValues[3];
						c |= char32_t(iHexValues[2]) << 4;
						c |= char32_t(iHexValues[1]) << 8;
						c |= char32_t(iHexValues[0]) << 12;

						rlText::UTF8Codepoint cp;
						rlText::EncodeUTF8(c, cp);
						sDest.reserve(sDest.length() + cp.count);
						switch (cp.count)
						{
						case 1:
							sDest += cp.ch[0];
							break;
						case 2:
							sDest += cp.ch[0];
							sDest += cp.ch[1];
							break;
						case 3:
							sDest += cp.ch[0];
							sDest += cp.ch[1];
							sDest += cp.ch[2];
							break;
						case 4:
							sDest += cp.ch[0];
							sDest += cp.ch[1];
							sDest += cp.ch[2];
							sDest += cp.ch[3];
							break;
						}

						sz += 5; // u + 4 digits
					}

					else
					{
						bool bFound = false;
						for (const auto &it : oEscapeChars)
						{
							if (it.second == *sz)
							{
								bFound = true;
								sDest += it.first;
								break;
							}
						}
						if (!bFound)
							return false; // unknown escape sequence
					}
				}

				// end of string
				else if (*sz == u8'"')
				{
					++sz;
					return true;
				}

				// regular character
				else
					sDest += *sz;
			}

			return false; // unexpected EOF
		}
	}

	bool Value::loadFromString(const char8_t *& szJSON) noexcept
	{
		clear();

		SkipWhitespace(szJSON);
		if (*szJSON == 0)
			return false; // unexpected EOF

		switch (*szJSON)
		{
		case u8'[': // [ --> Array
		{
			Array o;
			if (!ParseArray(szJSON, o))
				return false; // invalid syntax (array expected)

			*this = o;
			break;
		}

		case u8't': // t --> "true"  --> Boolean
		case u8'f': // f --> "false" --> Boolean
		{
			bool b;
			if (!ParseBoolean(szJSON, b))
				return false; // invalid syntax (boolean expected)

			*this = b;
			break;
		}

		case u8'n': // n --> "null"
		{
			if (!ParseNull(szJSON))
				return false; // invalid syntax (null expected)

			break;
		}

		default: // Number
		{
			Number o;
			if (!ParseNumber(szJSON, o))
				return false; // invalid syntax (number expected)

			*this = o;
			break;
		}

		case u8'{': // { --> Object
		{
			Object o;
			if (!ParseObject(szJSON, o))
				return false; // invalid syntax (object expected)

			*this = o;
			break;
		}

		case u8'\"': // " --> String
		{
			std::u8string s;
			if (!ParseString(szJSON, s))
				return false; // invalid syntax (string expected)

			*this = s;
			break;
		}
		}

		SkipWhitespace(szJSON);
		return true;
	}

	bool Value::loadFromFile(const char8_t *szPath) noexcept
	{
		std::vector<std::u8string> oLines;

		{
			rlText::File file(szPath);
			if (!file)
				return false;
			const auto iLineCount = file.getLineCount();
			oLines.reserve(iLineCount);
			for (size_t i = 0; i < iLineCount; ++i)
			{
				oLines.push_back(file.getLine(i));
			}
		}

		if (oLines.size() == 0)
			return false;

		std::u8string sText;
		size_t iLen = oLines.size();
		for (const auto &s : oLines)
		{
			iLen += s.length();
		}

		sText.reserve(iLen);
		for (const auto &s : oLines)
		{
			sText += s + u8"\n";
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

	bool Value::saveToString(std::u8string &sJSON) const noexcept
	{
		if (m_eType != Type::Array && m_eType != Type::Object)
			return false;

		sJSON = toString(true);
		return true;
	}

	bool Value::saveToFile(const char8_t *szPath) const noexcept
	{
		std::u8string s;
		if (!saveToString(s))
			return false;

		rlText::File file(RLTEXT_LINEBREAK_WINDOWS);
		return (file.setAsSingleString(s.c_str()) &&
			file.save(szPath));
	}

}