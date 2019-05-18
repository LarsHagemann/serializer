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
		
		static std::string narrow( const std::wstring& str ){
			if (str.empty()) return std::string();
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &strTo[0], size_needed, NULL, NULL);
			return strTo;
		}
		
		static std::wstring widen(const std::string& str) {
			return std::wstring(str.begin(), str.end());
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