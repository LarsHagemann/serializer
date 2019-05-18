#pragma once
#include <vector>
#include "Reader.hpp"
#include "Writer.hpp"

namespace serializable
{
	class SerializableBase;

	///
	// WRITING
	///
	class Writer;

	template<class _T>
	static Writer& operator<<(Writer& writer, const std::vector<_T>& in)
	{
		Writer tmp;
		tmp.write(in.size());
		for (auto& elem : in)
			tmp << elem;
		writer.append(tmp);
		return writer;
	}

	///
	// READING
	///
	class Reader;

	template<class _T>
	static Reader& operator>>(Reader& stream, std::vector<_T>* value)
	{
		auto size = 0u;
		stream.read(&size);

		auto items = 0u;
		stream.read(&items);

		value->reserve(items);
		for (auto i = 0u; i < items; ++i)
		{
			_T t;
			stream >> &t;
			value->emplace_back(t);
		}
		return stream;
	}
}