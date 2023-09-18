#pragma once
#ifndef RLJSON_VARIABLE
#define RLJSON_VARIABLE





#include <string>



namespace rlJSON
{

	class Value
	{
	public: // types

		enum class Type
		{
			String,
			Number,
			Object,
			Array,
			Boolean,
			Null
		};


	public: // methods

		virtual ~Value() = default;

		Type type() const { return m_eType; }

		virtual std::string encode() const = 0;
		

	protected: // methods

		Value(Type eType) : m_eType(eType) {}


	private: // variables

		const Type m_eType;

	};

}





#endif // RLJSON_VARIABLE