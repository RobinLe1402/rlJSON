#pragma once
#ifndef RLJSON_VALUE
#define RLJSON_VALUE





#include "Number.hpp"

#include <map>
#include <string>
#include <vector>



namespace rlJSON
{

	class Value;

	using Array  = std::vector<Value>;
	using Object = std::map<std::u8string, Value>;

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


	public: // methods

		Value()              = default;
		Value(const Value &) = default;
		Value(Value&&)       = default;
		~Value()             = default;

		Value(const Array &val)             noexcept;
		Value(Array &&rval)                 noexcept;
		Value(bool val)                     noexcept;
		Value(int val)                      noexcept : Value(Number(val)) {}
		Value(uintmax_t val)                noexcept : Value(Number(val)) {}
		Value(intmax_t val)                 noexcept : Value(Number(val)) {}
		Value(double val)                   noexcept : Value(Number(val)) {}
		Value(const Number &val)            noexcept;
		Value(Number &&rval)                noexcept;
		Value(const Object &val)            noexcept;
		Value(Object &&rval)                noexcept;
		Value(const char8_t *sz)    noexcept;
		Value(const std::u8string &val) noexcept;
		Value(std::u8string &&rval)     noexcept;

		Value &operator=(const Value &other) noexcept;
		Value &operator=(Value &&rval) noexcept;

		Array         toArray()  const noexcept;
		bool          toBool()   const noexcept;
		Number        toNumber() const noexcept;
		Object        toObject() const noexcept;
		std::u8string toString(bool bEncodeString = false) const noexcept;

		const Array         *array()   const noexcept;
		      Array         *array()         noexcept;
		      bool           boolean() const noexcept;
		const Number        *number()  const noexcept;
		      Number        *number()        noexcept;
		const Object        *object()  const noexcept;
		      Object        *object()        noexcept;
		const std::u8string *string()  const noexcept;
		      std::u8string *string()        noexcept;

		Type type() const noexcept { return m_eType; }

		void clear() noexcept;



		/// <summary>Try to load from a string.</summary>
		/// <param name="szString">
		/// On entry, a pointer to the string to be parsed.<para />
		/// On exit (if the return value is <c>true</c>), a pointer to the unparsed remainder of the
		/// string.
		/// </param>
		/// <returns>Was an element successfully parsed?</returns>
		bool loadFromString(const char8_t *&szJSON) noexcept;

		/// <summary>Try to load from a file.</summary>
		/// <param name="szPath">The path of the filename to load.</param>
		/// <returns>Was an element successfully parsed?</returns>
		bool loadFromFile(const char8_t *szPath) noexcept;

		bool saveToString(std::u8string &sJSON) const noexcept;
		bool saveToFile(const char8_t *szPath) const noexcept;


	private: // variables

		Type m_eType = Type::Null;

		Array         m_oValue_Array;
		bool          m_bValue_Boolean = false;
		Number        m_oValue_Number;
		Object        m_oValue_Object;
		std::u8string m_sValue_String;

	};

}





#endif // RLJSON_VALUE