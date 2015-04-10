#ifndef _LIST_HPP
#define _LIST_HPP

#include <cstddef>
#include <iostream>
#include <memory>
#include <iterator>
#include <stdexcept>

namespace fl
{
	namespace containers
	{
		
		template <class Node>
		struct Iterator
		{
			using pointer = Node*;
			using reference = Node&;
			pointer m_ptr;

			Iterator(const Iterator& it) : m_ptr(it.m_ptr) {}
			~Iterator() {}
			Iterator& operator=(const Iterator& it)
			{
				this->m_ptr = it.m_ptr;
				return *this;
			}
			
			bool operator==(const Iterator& rhs)
			{
				return this->m_ptr == rhs.m_ptr;
			}
			bool operator!=(const Iterator& rhs)
			{
				return this->m_ptr != rhs.m_ptr; 
			}

			Iterator& operator++()
			{
				m_ptr = m_ptr->m_next;
				return *this;
			}
			Iterator operator++(int)
			{
				Iterator it = *this;
				m_ptr = m_ptr->m_next;
				return it;
			}

			reference operator*()
			{
				return *m_ptr;
			}
		};

		template <class T>
		struct ConstIterator
		{
		};

		template <class T>
		struct InputIterator
		{
		};

		template <class T> 
		struct ListNode 
		{
			ListNode() 
			{
				m_next = this;
				m_prev = this;
			}
			ListNode(const T& t) :
				m_value(t),
				m_next(this),
				m_prev(this) {}

			ListNode(T&& t) :
				m_value(std::move(t)),
				m_next(this),
				m_prev(this) {}
			~ListNode() {}
			T m_value;
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
			using node_type = ListNode<T>;
			using node_ptr = node_type*;
			using size_type = std::size_t;
			using value_type = T;
			using pointer = typename std::allocator_traits<alloc_type>::pointer;
			using const_pointer = typename std::allocator_traits<alloc_type>::const_pointer;
			using reference = value_type&;
			using node_reference = node_type&;
			using const_reference = const value_type&;
			using const_node_reference = const ListNode<T>&;

			public:
			explicit List() 
			{
				m_head = 0;
				m_tail = 0;
				m_count = 0;
			}
			List( size_type count, 
					const T& value, 
					const Allocator& alloc = Allocator() )
			{
				m_count = count;
				pointer p = alloc.allocate(count);
				int i = 0;
				while (i < (int) count)
				{
					alloc.construct(p, value);
					p++;
				}
				m_head = new ListNode<T>(std::move(p[0]));
				i = 1;
				node_ptr temp = m_head;
				while (i < (int) count)
				{
					temp->m_next = new ListNode<T>(std::move(p[i]));
					temp->m_next->m_prev = temp;
					temp = temp->m_next;
				}	
				m_tail = temp;
			}
			// C++14
			explicit List( size_type count,
						   const Allocator& alloc = Allocator()	)
			{
			}
			template <class InputIterator>
			List(InputIterator first, InputIterator last)
			{
			}

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

			
			const_reference front() const { return m_head->m_value; }
			reference front() { return m_head->m_value; }
			const_reference back() const { return m_tail->m_value; }
			reference back() { return m_tail->m_value; }

			bool empty() { return m_count == 0; }
			size_type size() { return m_count; }

			void clear()
			{

			}

			const alloc_type get_allocator() const { return m_alloc; }
			private:
			node_ptr m_head;
			node_ptr m_tail;
			size_type m_count;
			alloc_type m_alloc;
		};
	}

}


#endif // List.hpp
