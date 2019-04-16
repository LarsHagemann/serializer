#pragma once
#include <Tinyxml2/tinyxml2.h>
#include <sstream>
#include "ValueHolder.hpp"

namespace xml = tinyxml2;

namespace serializable
{
	class SerializableBase;
	class Writer
	{
	public:
		xml::XMLDocument doc;
		xml::XMLElement* current = nullptr;
	public:
		Writer(const char* root = "root")
		{
			current = doc.NewElement(root);
			doc.InsertEndChild(current);
		}
		void write_to(const std::string& filepath, bool compact = false)
		{
			doc.SaveFile(filepath.data(), compact);
		}
	};
	template<class _T>
	Writer& operator<<(Writer& writer, const WriterValueHolder<_T>& in)
	{
		if constexpr (std::is_base_of_v<SerializableBase, _T>)
		{
			auto my_node = writer.doc.NewElement(in.name);
			writer.current->InsertEndChild(my_node);
			writer.current = my_node;
			in.value.write_to(writer);
			writer.current = my_node->Parent();
		}
		else
		{
			auto my_node = writer.doc.NewElement(in.name);
			std::stringstream ss;
			ss << in.value;
			my_node->SetAttribute("value", ss.str().data());
			writer.current->InsertEndChild(my_node);
		}
		return writer;
	}	
}

namespace ser = serializable;