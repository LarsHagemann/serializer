#pragma once
#include <Tinyxml2/tinyxml2.h>

namespace xml = tinyxml2;

template<class _T>
struct WriterValueHolder
{
	using value_type = _T;
	WriterValueHolder(const _T& v, const char* const n) : value(v), name(n) {}
	const _T& value;
	const char* const name;
};

template<class _T>
struct ReaderValueHolder
{
	using value_type = _T*;
	ReaderValueHolder(_T* v, xml::XMLElement* elem) : value(v), element(elem) {}
	_T* value;
	xml::XMLElement* element;
};
