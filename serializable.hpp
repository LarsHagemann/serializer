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
		virtual size_t write_to(Writer&) const = 0;
		virtual void read_from(Reader&) = 0;
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
			: m_offset(offset), m_hash(std::hash<std::string>()(name))
		{}
		/* Public member fields */
		uint32_t m_offset;
		hash_t m_hash;
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
		size_t write_to(Writer& writer) const override
		{
			on_write();
			const auto data = m_datatype.data();
			Writer tmp;
			for_each_in_tuple(data, [&](auto i)
			{
				using type = typename decltype(i)::member_type;
				const auto offset = i.m_offset;
				const auto& value = *ptr_data<type>(offset);
				const hash_t hash = i.m_hash;
				tmp.write(hash);
				tmp << value;
			});
			writer.append(tmp);
			return 0;
		}
		void read_from(Reader& self) override
		{
			size_t size;
			self.read(&size);
			if (size > self.get_size())
				return;
			Reader reader;
			reader.setup(self, size);
			on_read();
			auto data = m_datatype.data();
			hash_t hash;
			while (reader.read_hash(hash))
			{
				for_each_in_tuple(data, [&](auto i)
				{
					if (hash == i.m_hash)
					{
						using type = typename decltype(i)::member_type;
						const auto offset = i.m_offset;
						const auto value = ptr_data<type>(offset);
						reader >> value;
					}
				});
			}
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