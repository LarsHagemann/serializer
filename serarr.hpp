#pragma once
#include <array>
#include <Tinyxml2/tinyxml2.h>
#include <string>

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

	template<class _T, size_t _S>
	Writer& operator<<(Writer& writer, const WriterValueHolder<std::array<_T, _S>>& in)
	{
		auto my_node = writer.doc.NewElement(in.name);
		writer.current->InsertEndChild(my_node);
		writer.current = my_node;
		unsigned i = 0;
		for (auto& elem : in.value)
		{
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
				elem.write_to(writer);
			else
				writer << WriterValueHolder<decltype(elem)>(elem, ("elem" + std::to_string(i)).data());
			++i;
		}
		writer.current = reinterpret_cast<xml::XMLElement*>(my_node->Parent());
		return writer;
	}

	/// 
	// READING
	///
	class Reader;

	template<class _T, size_t _S>
	static Reader& operator>>(Reader& stream, const ReaderValueHolder<std::array<_T, _S>>& value)
	{
		auto xChild = value.element->FirstChildElement();
		while (xChild)
		{
			stream.current = xChild;
			std::string iStr = std::string(xChild->Name()).substr(4);
			auto index = atoi(iStr.data());
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
			{
				_T tmp;
				tmp.read_from(stream);
				(*value.value)[index] = tmp;
			}
			else
			{
				if (xChild->Attribute("value"))
				{
					_T tmp;
					std::istringstream iss(xChild->Attribute("value"));
					iss >> tmp;
					(*value.value)[index] = tmp;
				}
			}
			if (!xChild->NextSiblingElement())
				stream.current = reinterpret_cast<xml::XMLElement*>(xChild->Parent());
			xChild = xChild->NextSiblingElement();
		}
		return stream;
	}
}