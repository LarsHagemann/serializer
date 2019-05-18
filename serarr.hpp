#pragma once
#include <array>
#include "Reader.hpp"
#include "Writer.hpp"

namespace serializable
{
	class SerializableBase;

	///
	// WRITING
	///
	class Writer;

	template<class _T, unsigned N>
	static Writer& operator<<(Writer& writer, const std::array<_T, N>& in)
	{
		Writer tmp;
		for (auto& elem : in)
			tmp << elem;
		writer.append(tmp);
		return writer;
	}

	///
	// READING
	///
	class Reader;

	template<class _T, unsigned N>
	static Reader& operator>>(Reader & stream, std::array<_T, N> * value)
	{
		auto size = 0u;
		stream.read(&size);

		for (auto i = 0u; i < N; ++i)
		{
			_T t;
			stream >> &t;
			value->at(i) = t;
		}
		return stream;
	}
}