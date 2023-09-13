#pragma once
#ifndef RLJSON_JSON
#define RLJSON_JSON





#include <map>
#include <memory>

#include "Value.hpp"

// todo: array + object
// https://www.json.org/json-en.html



namespace rlJSON
{


	class JSON
	{
	public: // methods

		bool loadFromFile(const wchar_t *szFilename); // todo
		bool saveToFile(const wchar_t *szFilename);

		const Value *root() const { return m_upRootValue.get(); }
		Value *root() { return m_upRootValue.get(); }


	private: // variables

		std::unique_ptr<Value> m_upRootValue = nullptr;

	};


}






#endif // RLJSON_JSON