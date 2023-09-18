#pragma once
#ifndef RLJSON_BOOLEAN
#define RLJSON_BOOLEAN





#include "rlJSON/Value.hpp"



namespace rlJSON
{

	class Boolean : public Value
	{
	public: // methods

		Boolean();
		Boolean(const Boolean &) = default;
		Boolean(Boolean&&) = default;
		Boolean(bool b);
		~Boolean() = default;

		Boolean &operator=(bool b);
		Boolean &operator=(const Boolean &) = default;
		Boolean &operator=(Boolean &&) = default;

		bool value() const { return m_bValue; }

		std::string encode() const;


	private: // variables

		bool m_bValue = false;

	};

}





#endif // RLJSON_BOOLEAN