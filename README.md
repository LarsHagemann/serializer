### serializer

Dependends on tinyxml2 at the moment. I'm planning to make it independent from external libraries. 

The C++ Serializer makes it (hopefully) easy to serialize and deserialize objects.
This is a header only library comprised of the following files:
* [serializable.hpp](/serializable.hpp)
* [reader.hpp](/reader.hpp)
* [writer.hpp](/writer.hpp)
* [valueholder.hpp](/valueholder.hpp)
* [serarr.hpp](/serarr.hpp)
* [servec.hpp](/servec.hpp)
* [sermap.hpp](/sermap.hpp)

In serializable.hpp is most of the magic happening. Most interesting for users are the reader and the writer though.
The three files serarr.hpp, servec.hpp and sermap.hpp contain helper functions that make it easy to serialize
std::map, std::array and std::vector. The Library will work for every type with an ostringstream::operator<< overload and an istringstream::operator>> overload.

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
	MyClass() : _Serializer(ser::make_datatype(
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
	
	MyClass() : _Serializer(make_datatype(
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
		ser::Writer writer("object"); // "object" is the name of the root xml element
		obj.write_to(writer);
		writer.write_to("dummy.data"); // Filename
	}
	{
		ser::Reader reader("object"); // "object" needs to match the name from above
		reader.read_from("dummy.data");
		MyClass obj;
		obj.read_from(reader);
		obj.print();
	}
}

```

### Planned:
* make it independent of tinyxml2
* support for getters/setters
