#include "rlJSON/String.hpp"

#include <map>
#include <memory>

namespace rlJSON
{

	String::String(const wchar_t *sz) : Value(Value::Type::String), m_sValue(sz)
	{
		removeIllegalChars();
	}

	String::String(const char *sz) : Value(Value::Type::String), m_sValue(strlen(sz), 0)
	{
		for (size_t i = 0; i < m_sValue.length(); ++i)
		{
			m_sValue[i] = sz[i];
		}
		removeIllegalChars();
	}

	int String::operator< (const String &other) const { return m_sValue <  other.m_sValue; }
	int String::operator<=(const String &other) const { return m_sValue <= other.m_sValue; }
	int String::operator> (const String &other) const { return m_sValue >  other.m_sValue; }
	int String::operator>=(const String &other) const { return m_sValue >= other.m_sValue; }
	int String::operator==(const String &other) const { return m_sValue == other.m_sValue; }
	int String::operator!=(const String &other) const { return m_sValue != other.m_sValue; }

	std::string String::encode() const
	{
		if (m_sValue.empty())
			return "\"\"";

		auto up_szUTF32 = std::make_unique<char32_t[]>(m_sValue.length() + 1);
		auto szUTF32 = up_szUTF32.get();

		memset(szUTF32, 0, (m_sValue.length() + 1) * sizeof(wchar_t));

		size_t iLen = 0;
		for (size_t i = 0; i < m_sValue.length(); ++i, ++iLen)
		{
			char32_t &cDest = szUTF32[i];

			wchar_t c = m_sValue[i];
			if ((c & 0xFC00) == 0xD800)
			{
				cDest = char32_t(c & 0x03FF) << 10;
				++i;
				c = m_sValue[i];
				cDest |= c & 0x03FF;
			}
			else
				cDest = c;
		}

		size_t iEncodedLen = iLen;
		for (size_t i = 0; i < iLen; ++i)
		{
			if (szUTF32[i] & 0xFFFFFF00)
				iEncodedLen += 5; // replace with '\', add 'u' + value as 4 hex digits
			else
			{
				switch (szUTF32[i])
				{
				case '"':
				case '\\':
				case '/':
				case '\b':
				case '\f':
				case '\n':
				case '\r':
				case '\t':
					++iEncodedLen; // '\' as prefix
					break;
				}
			}
		}

		std::string sResult;
		sResult.reserve(iEncodedLen + 2);
		sResult +=  '\"';
		for (size_t i = 0; i < iLen; ++i)
		{
			const char32_t c = szUTF32[i];

			if (c & 0xFFFFFF00)
			{
				constexpr char szHEX[] = "0123456789ABCDEF";
				sResult += "\\u";

				sResult += (szHEX[(c >> 12) & 0x0F]);
				sResult += (szHEX[(c >> 8 ) & 0x0F]);
				sResult += (szHEX[(c >> 4 ) & 0x0F]);
				sResult += (szHEX[ c        & 0x0F]);
			}
			else
			{
				switch (c)
				{
				case '"':
					sResult += "\\\"";
					break;

				case '\\':
					sResult += "\\\\";
					break;

				case '/':
					sResult += "\\/";
					break;

				case '\b':
					sResult += "\\b";
					break;

				case '\f':
					sResult += "\\f";
					break;

				case '\n':
					sResult += "\\n";
					break;

				case '\r':
					sResult += "\\r";
					break;

				case '\t':
					sResult += "\\t";
					break;

				default:
					sResult += (char)c;
					break;
				}
			}
		}

		sResult += '\"';
		return sResult;
	}

	void String::removeIllegalChars()
	{
		for (size_t i = m_sValue.length(); i > 0; --i)
		{
			wchar_t c = m_sValue[i - 1];

			if ((c & 0xFFFFFF00) == 0) // ASCII (all illegal chars are ASCII)
			{
				if (iscntrl(c)) // only control characters are strictly forbidden (not all though)
				{
					switch (c)
					{
					case '\b':
					case '\f':
					case '\n':
					case '\r':
					case '\t':
						break;
					default:
						m_sValue.erase(i - 1);
						continue;
					}
				}
			}
		}
	}

}