#pragma once
#include <map>
#include <Tinyxml2/tinyxml2.h>

namespace xml = tinyxml2;

///
// WRITING
///

namespace serializable
{

	class Writer;
	template<class T>
	struct WriterValueHolder;

	template<class _T, class _R>
	static Writer& operator<<(Writer& writer, const WriterValueHolder<std::pair<_T, _R>>& in)
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
			std::stringstream kss, vss;
			kss << in.value.first;
			vss << in.value.second;
			my_node->SetAttribute("key", kss.str().data());
			my_node->SetAttribute("value", vss.str().data());
			writer.current->InsertEndChild(my_node);
		}
		return writer;
	}

	template<class _T, class _R>
	Writer& operator<<(Writer& writer, const WriterValueHolder<std::map<_T, _R>>& in)
	{
		auto my_node = writer.doc.NewElement(in.name);
		writer.current->InsertEndChild(my_node);
		writer.current = my_node;
		for (auto& elem : in.value)
		{
			auto e_node = writer.doc.NewElement("element");
			writer.current->InsertEndChild(e_node);
			writer.current = e_node;
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
				elem.first.write_to(writer);
			else
				writer << WriterValueHolder<decltype(elem.first)>(elem.first, "key");
			if constexpr (std::is_base_of_v<SerializableBase, _R>)
				elem.second.write_to(writer);
			else
				writer << WriterValueHolder<decltype(elem.second)>(elem.second, "value");
			writer.current = reinterpret_cast<xml::XMLElement*>(e_node->Parent());
		}
		writer.current = reinterpret_cast<xml::XMLElement*>(my_node->Parent());
		return writer;
	}

	///
	// READING
	///

}