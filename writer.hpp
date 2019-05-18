#pragma once
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>

#include "ValueHolder.hpp"

namespace serializable
{
	class SerializableBase;

	class Writer
	{
	private:
		std::ostringstream m_stream;
	public:
		// constructors
		Writer() = default;
		Writer(const Writer&) = default;
		Writer(Writer&&) = default;
		Writer& operator=(const Writer&) = default;
		Writer& operator=(Writer&&) = default;
		// destructor
		~Writer()
		{

		}
	private:
		template<class _T>
		bool _write_trivial(const _T& data)
		{
			static_assert(std::is_trivially_copyable_v<_T>, "_T needs to be trivially copyable");
			return m_stream.write((const char*)&data, sizeof(_T)).good();
		}
		template<class _T>
		bool _write_serializable(const _T& data)
		{
			data.write_to(*this);
			return m_stream.good();
		}
	public:
		const std::ostringstream& _get_stream() const
		{
			return m_stream;
		}
	public:
		template<class _T>
		bool write(const _T& data)
		{
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
				return _write_serializable(data);
			else
				return _write_trivial(data);
		}
		bool append(const Writer& other)
		{
			auto data = other._get_stream().str();
			write(data.length());
			write(data.c_str(), data.length());
			return true;
		}
		template<class _T>
		bool write(const _T* data, size_t size)
		{
			return m_stream.write((const char*)data, size).good();
		}
		template<class _T>
		bool write(const _T* data)
		{
			return m_stream.write((const char*)data, sizeof(_T)).good();
		}
	public:
		bool to_file(const path& path)
		{
			std::ofstream out(path, 
				  std::ios::binary 
				| std::ios::out 
				| std::ios::trunc);
			if (!out.is_open())
				return false;
			out << m_stream.str();
			if (!out.good())
				return false;
			out.close();
			return true;
		}
	};

	template<class _T>
	Writer& operator<<(Writer& writer, const _T& data)
	{
		writer.write(data);
		return writer;
	}
}

namespace ser = serializable;