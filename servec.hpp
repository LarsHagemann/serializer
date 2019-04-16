#pragma once
#include <vector>
#include <Tinyxml2/tinyxml2.h>

namespace xml = tinyxml2;

template<class T>
struct WriterValueHolder;
template<class T>
struct ReaderValueHolder;

namespace serializable
{
	class SerializableBase;

	///
	// WRITING
	///
	class Writer;

	template<class _T>
	Writer& operator<<(Writer& writer, const WriterValueHolder<std::vector<_T>>& in)
	{
		auto my_node = writer.doc.NewElement(in.name);
		writer.current->InsertEndChild(my_node);
		writer.current = my_node;
		for (auto& elem : in.value)
		{
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
				elem.write_to(writer);
			else
				writer << WriterValueHolder<decltype(elem)>(elem, "element");
		}
		writer.current = reinterpret_cast<xml::XMLElement*>(my_node->Parent());
		return writer;
	}

	/// 
	// READING
	///
	class Reader;

	template<class _T>
	static Reader& operator>>(Reader& stream, const ReaderValueHolder<std::vector<_T>>& value)
	{
		auto xChild = value.element->FirstChildElement();
		while (xChild)
		{
			stream.current = xChild;
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
			{
				_T tmp;
				tmp.read_from(stream);
				(*value.value).emplace_back(tmp);
			}
			else
			{
				if (xChild->Attribute("value"))
				{
					_T tmp;
					std::istringstream iss(xChild->Attribute("value"));
					iss >> tmp;
					(*value.value).emplace_back(tmp);
				}
			}
			if (!xChild->NextSiblingElement())
				stream.current = reinterpret_cast<xml::XMLElement*>(xChild->Parent());
			xChild = xChild->NextSiblingElement();
		}
		return stream;
	}
}