#pragma once
#include <map>
#include <unordered_map>
#include "Reader.hpp"
#include "Writer.hpp"

//////////////////////////////////////////
//
// Currently this file supports the following conversions:
// 	std::map
// 	std::unordered_map
//
//////////////////////////////////////////

namespace serializable
{
	class SerializableBase;

	///
	// std::map
	///

	///
	// WRITING
	///
	class Writer;

	template<class _T, class _R>
	Writer& operator<<(Writer& writer, const std::map<_T, _R>& in)
	{
		Writer tmp;
		tmp.write(in.size());
		for (auto& elem : in)
		{
			tmp.write(elem.first);
			tmp.write(elem.second);
		}
		writer.append(tmp);
		return writer;
	}

	///
	// READING
	///
	class Reader;

	template<class _T, class _R>
	static Reader& operator>>(Reader & stream, std::map<_T, _R> * value)
	{
		auto size = 0u;
		stream.read(&size);

		auto items = 0u;
		stream.read(&items);

		for (auto i = 0u; i < items; ++i)
		{
			_T t;
			_R r;
			stream.read(&t);
			stream.read(&r);
			value->emplace(t, r);
		}
		return stream;
	}

	///
	// std::unordered_map
	///

	///
	// WRITING
	///
	class Writer;

	template<class _T, class _R>
	static Writer& operator<<(Writer& writer, const std::unordered_map<_T, _R>& in)
	{
		Writer tmp;
		tmp.write(in.size());
		for (auto& elem : in)
		{
			tmp << elem.first;
			tmp << elem.second;
		}
		writer.append(tmp);
		return writer;
	}

	///
	// READING
	///
	class Reader;

	template<class _T, class _R>
	static Reader& operator>>(Reader& stream, std::unordered_map<_T, _R>* value)
	{
		auto size = 0u;
		stream.read(&size);

		auto items = 0u;
		stream.read(&items);

		for (auto i = 0u; i < items; ++i)
		{
			_T t;
			_R r;
			stream >> &t;
			stream >> &r;
			value->insert({ t, r });
		}
		return stream;
	}
}