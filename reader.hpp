#pragma once
#include <Tinyxml2/tinyxml2.h>
#include "ValueHolder.hpp"

namespace xml = tinyxml2;

namespace serializable
{
	class SerializableBase;
	class Reader
	{
	public:
		xml::XMLDocument doc;
		xml::XMLElement* current = nullptr;
		const char* root = nullptr;
	public:
		Reader(const char* r = "root")
			: root(r)
		{}
		void read_from(const std::string& filepath)
		{
			doc.LoadFile(filepath.data());
			current = doc.FirstChildElement(root);
		}
	};
	template<class _T>
	static Reader& operator>>(Reader& stream, ReaderValueHolder<_T> value)
	{
		if constexpr (std::is_base_of_v<SerializableBase, _T>)
		{
			auto my_node = stream.current->FirstChildElement();
			if (my_node) {
				stream.current = my_node;
				value.value->read_from(stream);
				stream.current = my_node->Parent();
			}
		}
		else
		{
			std::istringstream iss(value.element->Attribute("value"));
			iss >> *value.value;
		}
		return stream;
	}
}

namespace ser = serializable;