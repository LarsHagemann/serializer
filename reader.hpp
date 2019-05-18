#pragma once
#include <filesystem>
#include "ValueHolder.hpp"
#include "Writer.hpp"

namespace serializable
{
	class SerializableBase;
	class Reader
	{
	private:
		std::string buffer;
		unsigned current;
	private:
		const char* get_current()
		{
			return buffer.data() + current;
		}
		template<class _T>
		bool _read_serializable(_T* into)
		{
			into->read_from(*this);
			return true;
		}
		template<class _T>
		bool _read_trivial(_T* into)
		{
			std::string tmp = typeid(_T).name();
			tmp.append(" is not trivially copyable.");
			static_assert(std::is_trivially_copyable_v<_T>, "_T needs to be trivially copyable");
			auto size = sizeof(_T);
			if (current + size > buffer.length())
				return false;
			memcpy_s(into, size, get_current(), size);
			current += size;
			return current < buffer.length();
		}
	public:
		Reader() : current(0) {}
		void read_raw(const std::string& data)
		{
			buffer = data;
			current = 0u;
		}
		std::string get_data()
		{
			return buffer;
		}
		bool read_from(const path& filepath)
		{
			if(!read_file(filepath, buffer))
				return false;
			current = 0u;
			return true;
		}
		bool skip(size_t bytes)
		{
			current += bytes;
			return current < buffer.length();
		}
		template<class _T>
		Reader& read(_T* into)
		{
			if constexpr (std::is_base_of_v<SerializableBase, _T>)
				_read_serializable(into);
			else
				_read_trivial(into);
			return *this;
		}
		template<class _T>
		Reader& read(_T* into, size_t length)
		{
			memcpy_s(into, length, get_current(), length);
			current += length;
			return *this;
		}
		bool read_hash(hash_t & hash)
		{
			return _read_trivial(&hash);
		}
		bool setup(Reader& other, size_t length)
		{
			buffer = std::string(other.get_current(), length);
			current = 0u;
			return other.skip(length);
		}
		size_t get_size()
		{
			return buffer.length() - current;
		}
	};

	template<class _T>
	Reader& operator>>(Reader& reader, _T* data)
	{
		return reader.read(data);
	}
}

namespace ser = serializable;