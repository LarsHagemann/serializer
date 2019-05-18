#pragma once
#include <string>
#include <codecvt>
#include <locale>
#ifdef SER_ENABLE_FILESYSTEM
#include <filesystem>
#endif
#include <Windows.h>
#include "Reader.hpp"
#include "Writer.hpp"

//////////////////////////////////////////
//
// Currently this file supports the following conversions:
// 	std::string
// 	std::wstring
// 	std::filesystem::path
//
//////////////////////////////////////////

namespace serializable
{
	class SerializableBase;
	
	namespace priv
	{
	
		///
		// _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
		// those two functions need the aforementioned preprocessor definition
		// to work.
		static std::string narrow(const std::wstring& win)
		{
			std::wstring_convert<
				std::codecvt_utf8_utf16< std::wstring::value_type >,
				std::wstring::value_type
			> utf16conv;
			return utf16conv.to_bytes(win);
		}
		static std::wstring widen(const std::string& ain)
		{
			std::wstring_convert<
				std::codecvt_utf8_utf16< std::wstring::value_type >,
				std::wstring::value_type
			> utf16conv;
			return utf16conv.from_bytes(ain);
		}
		
	}
	
	///
	// WRITING
	///
	class Writer;

	static Writer& operator<<(Writer& writer, const std::string& in)
	{
		auto length = in.length();
		writer.write(length);
		writer.write(in.c_str(), in.length());
		return writer;
	}
	
	class Writer;

	static Writer& operator<<(Writer& writer, const std::wstring& in)
	{
		return writer << priv::narrow(in);
	}
	
#ifdef SER_ENABLE_FILESYSTEM
	
	static Writer& operator<<(Writer& writer, const path& in)
	{
		return writer << priv::narrow(in.c_str());
	}
	
#endif

	/// 
	// READING
	///
	class Reader;

	static Reader& operator>>(Reader& stream, std::string* value)
	{
		size_t length;
		stream.read(&length);
		value->resize(length);
		stream.read(value->data(), length);
		return stream;
	}
	
	static Reader& operator>>(Reader& reader, std::wstring* in)
	{
		std::string tmp;
		reader >> &tmp;
		*in = priv::widen(tmp);
		return reader;
	}
	
#ifdef SER_ENABLE_FILESYSTEM
	
	static Reader& operator>>(Reader& reader, path* in)
	{
		std::string tmp;
		reader >> &tmp;
		*in = tmp;
		return reader;
	}
	
#endif
}