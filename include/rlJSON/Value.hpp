#pragma once
#ifndef RLJSON_VALUE
#define RLJSON_VALUE





#include <map>
#include <string>
#include <variant>
#include <vector>



namespace rlJSON
{

	class Value final
	{
	public: // types

		enum class Type
		{
			Array,
			Boolean,
			Null,
			Number,
			Object,
			String
		};


	public: // static methods

		/// <summary>
		/// Create a <c>Value</c> instance from a string.
		/// </summary>
		/// <param name="szString">
		/// On entry, a pointer to the string to be parsed.<para />
		/// On exit, a pointer to the unparsed remainder of the string.
		/// </param>
		static Value FromString(const wchar_t *&szString);

		/// <summary>
		/// Try to create a <c>Value</c> instance from a string.
		/// </summary>
		/// <param name="szString">
		/// On entry, a pointer to the string to be parsed.<para />
		/// On exit (if the return value is <c>true</c>), a pointer to the unparsed remainder of
		/// the string.
		/// </param>
		/// <param name="oVal">The parsed value.</param>
		/// <returns></returns>
		static bool TryFromString(const wchar_t *&szString, Value &oVal);


	public: // methods

		std::wstring toString();


	private: // variables

		Type m_eType = Type::Null;

		union ValUnion
		{
			// Array
			std::vector<Value> oArray;
			// Boolean
			bool b;
			// Number
			uint64_t, int64_t, double;
			// Object
			std::map<std::wstring, Value> oObject;
			// String
			std::wstring s;
		} m_uVal;

	};

}





#endif // RLJSON_VALUE