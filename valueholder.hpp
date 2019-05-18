#pragma once
#include <fstream>

using hash_t = size_t;
#ifdef SER_ENABLE_FILESYSTEM
#include <filesystem>
using path = std::filesystem::path;
#else
using path = std::string;
#endif

static bool read_file(const path& path, std::string& content)
{
#ifdef SER_ENABLE_FILESYSTEM
	if(!std::filesystem::exists(path))
		return false;
#endif
	std::ifstream file(path, std::ios::binary | std::ios::in);
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			content.append(line);
			content += '\n';
		}
		file.close();
		return true;
	}
	return false;
}

struct ValueInfo
{
	hash_t hash;
	size_t size;
	char* val;
};

template<class _T>
struct ReaderValueHolder
{
	using value_type = _T*;
	ReaderValueHolder(_T* v, ValueInfo i) : value(v), info(i) {}
	_T* value;
	ValueInfo info;
};
