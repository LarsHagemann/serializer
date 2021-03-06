### serializer

The C++ Serializer makes it (hopefully) easy to serialize and deserialize objects.
This is a header only library comprised of the following files:
* [serializable.hpp](/serializable.hpp)
* [reader.hpp](/reader.hpp)
* [writer.hpp](/writer.hpp)
* [valueholder.hpp](/valueholder.hpp)
* [serstr.hpp](/serstr.hpp)
* [serarr.hpp](/serarr.hpp)
* [servec.hpp](/servec.hpp)
* [sermap.hpp](/sermap.hpp)

Add the preprocessor definition SER_ENABLE_FILESYSTEM if your compiler supports C++17 and the C++ filesystem.
Additionally the serstr.hpp file needs the definition _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING because it uses
the C++11 wstring_convert class. That solution is momentarily safe and portable and will be used until there is a better standard.

In serializable.hpp is most of the magic happening. Most interesting for users are the reader and the writer though.
The four files serarr.hpp, servec.hpp, serstr.hpp and sermap.hpp contain helper functions that make it easy to serialize
std::map, std::array, std::string, std::wstring, std::filesystem::path (if enabled) and std::vector. 
The Library will work for every trivially copyable type. You can write your own helper functions for other types as well.
Just have a look at serstr.hpp for example.

If you want to make a class serializable do the following.

```C++
class MyClass
{
	/* member variables */
	int my_number;
	std::string my_string;
	// ...
public:
	/* member functions */
};

/*
	To use the serializer, just include "serializable.hpp". Then let your class derive from ser::Serializable.
	The class templates are the types of your classes member variables.
*/

class MyClass : public ser::Serializable<int, std::string>
{
	// ...
};

/*
	Now your class constructor needs to initialize ser::Serializable<int, std::string>.
	Since it's a hassle to write all those template arguments you can use _Serializer inside your class.
*/

class MyClass : public ser::Serializable<int, std::string>
{
	// ...
public:
	MyClass() : _Serializer(serializable::make_datatype(
		make_member("MyClass::my_number", my_number),
		make_member("MyClass::my_string", my_string))), ... {}
};

/*
	That is all we need to do, to make our class serializable.
	- let it derive from Serializable<>
	- tell it which members to serialize
	
	Now you can serialize this class. 
	
	Final file:
*/

#include <string>
#include "serializable.hpp"

class MyClass : public ser::Serializable<int, std::string>
{
	int my_number;
	std::string my_string;
public:
	
	MyClass() : _Serializer(serializable::make_datatype(
		make_member("MyClass::my_number", my_number),
		make_member("MyClass::my_string", my_string))) {}
		
	void set_number(int number) { my_number = number; }
	void set_string(std::string str) { my_string = str; }
	void print()
	{
		printf_s(
			"MyClass::my_number: %d\nMyClass::my_string: %s\n\n", 
			my_number, 
			my_string.data()
		);
	}
};

int main()
{
	{
		MyClass obj;
		obj.set_number(100);
		obj.set_string("string");
		ser::Writer writer; 
		obj.write_to(writer);
		writer.write_to("dummy.data"); // Filename
	}
	{
		ser::Reader reader;
		reader.read_from("dummy.data");
		MyClass obj;
		obj.read_from(reader);
		obj.print();
	}
}
```

### Planned:
* support for getters/setters

Performance tests:
Serialiazation of an std::vector&lt;double&gt; with 1000000 elements generates a ~7.62MB file.
```C++
Serialization took 157709 microseconds (0.157709 seconds)
Deserialization took 288595 microseconds (0.288595 seconds)
```

Tested with: MSVC++ x86 Release C++17. The serializer takes a lot longer for debug builds.

