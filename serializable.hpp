#pragma once
#include <type_traits>
#include "Writer.hpp"
#include "Reader.hpp"

#ifndef OffsetOf
#define OffsetOf(c, mv) ((size_t) & (static_cast<c*>(nullptr)->mv))
#endif

#ifndef make_member
#define make_member(n, mv) \
	ser::MemberVariable<decltype(std::remove_pointer_t<decltype(this)>::mv)>\
	(n, OffsetOf(std::remove_pointer_t<decltype(this)>, mv))
#endif

namespace xml = tinyxml2;

namespace serializable
{
	class SerializableBase
	{
	public:
		SerializableBase() = default;
		SerializableBase(SerializableBase&&) = default;
		SerializableBase(const SerializableBase&) = default;
		SerializableBase& operator=(SerializableBase&&) = default;
		SerializableBase& operator=(const SerializableBase&) = default;
		virtual ~SerializableBase() = default;
	protected:
		virtual void write_to(Writer&, const char*) const = 0;
		virtual void read_from(Reader&, const char*) = 0;
		virtual void on_read() {}
		virtual void on_write() const {}
	};
	template<class F, class...Ts, std::size_t...Is>
	void for_each_in_tuple(const std::tuple<Ts...> & tuple, F func, std::index_sequence<Is...>) {
		using expander = int[];
		(void)expander {
			0, ((void)func(std::get<Is>(tuple)), 0)...
		};
	}
	template<class F, class...Ts>
	void for_each_in_tuple(const std::tuple<Ts...>& tuple, F func) {
		for_each_in_tuple(tuple, func, std::make_index_sequence<sizeof...(Ts)>());
	}
	template<class _T>
	class MemberVariable
	{
	public:
		/* Type info */
		using member_type = _T;
	public:
		/* Constructors */
		MemberVariable(const char* const name, uint32_t offset)
			: m_offset(offset), m_name(name)
		{}
		/* Public member fields */
		uint32_t m_offset;
		const char* const m_name;
	};
	template<class ..._Types>
	class DataType
	{
	public:
		using _Container = std::tuple<MemberVariable<_Types>...>;
		/* Constructors */
		explicit DataType(_Container&& init)
			: m_data(std::move(init)) {}
		explicit DataType(const _Container& init)
			: m_data(init) {}
	private:
		/* Private member data */
		_Container m_data;
	public:
		/* Data access functions */
		const _Container& data() const { return m_data; }
	};
	template<class ..._Types>
	class Serializable : public SerializableBase
	{
	public:
		using _Container = std::tuple<MemberVariable<_Types>...>;
		using _Serializer = Serializable<_Types...>;
		/* Constructors */
		explicit Serializable(DataType<_Types...>&& datatype)
			: m_datatype(std::move(datatype)) {}
		explicit Serializable(const DataType<_Types...>& datatype)
			: m_datatype(datatype) {}
	public:
		/* Serialization functions */
		void write_to(Writer& writer, const char* e_name = "element") const override
		{
			on_write();
			auto my_node = writer.doc.NewElement(e_name);
			writer.current->InsertEndChild(my_node);
			writer.current = my_node;
			const auto data = m_datatype.data();
			for_each_in_tuple(data, [&](auto i)
			{
				using type = typename decltype(i)::member_type;
				const auto offset = i.m_offset;
				const auto& value = *ptr_data<type>(offset);
				writer << WriterValueHolder<type>(value, i.m_name);
			});
			writer.current = reinterpret_cast<xml::XMLElement*>(my_node->Parent());
		}
		void read_from(Reader& reader, const char* e_name = "element") override
		{
			on_read();
			auto data = m_datatype.data();
			auto my_node = reader.current->FirstChildElement(e_name);
			reader.current = my_node;
			auto xChild = my_node->FirstChildElement();
			while (xChild)
			{
				auto my_name = xChild->Name();
				for_each_in_tuple(data, [&](auto var) {
					if (strcmp(var.m_name, my_name) == 0) {
						using type = typename decltype(var)::member_type;
						const auto offset = var.m_offset;
						auto value = ptr_data<type>(offset);
						reader >> ReaderValueHolder<type>(value, xChild);
					}
				});
				xChild = xChild->NextSiblingElement();
			}
			reader.current = reinterpret_cast<xml::XMLElement*>(my_node->Parent());
		}
	private:
		/* Helper functions */
		template<class _T>
		_T* ptr_data(uint32_t offset) const
		{
			return const_cast<_T*>(reinterpret_cast<const _T*>([](const void* ptr, uint32_t o)
			{
				return static_cast<const uint8_t*>(ptr) + o;
			}(this, offset)));
		}
	protected:
		/* Private member data */
		DataType<_Types...> m_datatype;
	};
	template<class ..._Types>
	DataType<_Types...> make_datatype(ser::MemberVariable<_Types>&& ... args)
	{
		return ser::DataType(std::make_tuple(args...));
	}
}

namespace ser = serializable;