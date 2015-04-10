#ifndef _LIST_HPP
#define _LIST_HPP

#include <cstddef>
#include <memory>
#include <iterator>
#include <stdexcept>

namespace fl
{
	namespace containers
	{
		
		template <class T> struct Iterator
		{
			bool operator==(const Iterator& rhs);

		};

		template <class T> struct ConstIterator
		{
		};

		template <class T> struct InputIterator
		{
		};

		using ReverseIterator = std::reverse_iterator<Iterator>;
		using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

		template <
			class T, 
			class Allocator = std::allocator<T> 
		> 
		struct ListNode 
		{

			using alloc_type = Allocator;
			using pointer = std::allocator_traits<alloc_type>::pointer;
			ListNode() 
			{
				m_next = this;
				m_prev = this;
				m_value = nullptr;
			}

			ListNode(const T& value, 
					 Allocator alloc = Allocator())
			{
			}
			~Node() {}
			pointer m_value;
			ListNode* m_next;
			ListNode* m_prev;
		};

		template <
			class T, 
			class Allocator = std::allocator<T> 
		> 
		class List
		{
			using alloc_type = Allocator;
			using difference_type = std::ptrdiff_t;
			using node_type = ListNode<T, alloc_type>;
			using node_ptr = node_type*;
			using size_type = std::size_t;
			using value_type = T;
			using pointer = std::allocator_traits<alloc_type>::pointer;
			using const_pointer = std::allocator_traits<alloc_type>::const_pointer;
			using reference = value_type&;
			using const_reference = const value_type&;
			using const_node_reference = const ListNode<T, alloc_type>&;

			public:
			explicit List(const Allocator& alloc = Allocator()) 
			{
				try
				{

				}
				catch (const std::bad_alloc& ex)
				{
					std::cerr << "Allocation Failed!" << std::endl;
				}
			}
			List( size_type count, 
					const T& value, 
					const Allocator& alloc = Allocator() )
			{
				m_count = count;
				for (int i = 0; i < count; i++)
				{
					pointer p = alloc.allocate(1);
				}
				


			}
			// C++14
			explicit List( size_type count,
						   const Allocator& alloc = Allocator()	)
			{

			}
			template <class InputIterator>
				List(InputIterator first, InputIterator last);
			List(const List& other) : 
				m_head(other.m_head),
				m_tail(other.m_tail),
				m_count(other.m_count) {}
			List(List&& other) 
			{
				this->m_head = other.m_head;
				this->m_tail = other.m_tail;
				this->m_count = std::move(other.m_count);

				other.m_head = nullptr;
				other.m_tail = nullptr;
			}
			List(std::initializer_list<T> init);
			~List() 
			{

			}

			const_reference front() const { return &(m_head->m_value); }
			reference front() { return &(m_head->m_value); }
			const_reference back() const { return &(m_tail->m_value); }
			reference back() { return &(*m_tail->m_value); }

			bool empty() { return m_count == 0; }
			size_type size() { return m_count; }
			// size_type max_size() { return std::distance(begin(), end()); }

			void clear()
			{

			}

			private:
			node_ptr m_head;
			node_ptr m_tail;
			size_type m_count;

		};
	}

}


#endif // List.hpp
