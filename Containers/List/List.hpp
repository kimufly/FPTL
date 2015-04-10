#ifndef _LIST_HPP
#define _LIST_HPP

#include <iostream>
#include <memory>
#include <stdexcept>
#include "../../Iterators/Iterator.hpp"

namespace fl
{
	namespace container
	{
		
		struct Node_base;
		template <class T> struct ListNode;

		struct Node_base 
		{
			using pointer = Node_base*;
			Node_base() : m_next(this), m_prev(this) {}
			virtual ~Node_base() {}
			pointer m_next;
		    pointer m_prev;
		};

		template <class T> struct ListNode : Node_base
		{
			using value_type = T;
			ListNode(const T& t) : m_data(t) {}
			ListNode(T&& t) : m_data(std::move(t)) {}
			value_type m_data;
		};

		template <
			class T, 
			class Allocator = std::allocator<T> 
		> 
		class List
		{
			using value_type = T;
			using alloc_type = Allocator;
			using size_type = std::size_t;
			using difference_type = std::ptrdiff_t;
			using node_type = ListNode<T>;
			using node_ptr = node_type*;
			using pointer = typename std::allocator_traits<alloc_type>::pointer;
			using const_pointer = typename std::allocator_traits<alloc_type>::const_pointer;
			using reference = value_type&;
			using const_reference = const value_type&;

			public:
			explicit List(const Allocator& alloc = Allocator()) 
			{
				m_alloc = alloc;
				typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
			    node_ptr np = m_node_alloc.allocate(0);
				m_sentinal = np;
				m_head = m_sentinal;
				m_tail = m_sentinal;
				m_count = 0;
			}
			List( size_type count, 
					const T& value, 
					const Allocator& alloc = Allocator() )
			{
				m_count = count;
				m_alloc = alloc;
				pointer p = m_alloc.allocate(count);
				int i = 0;
				while (i < (int) count)
				{
					m_alloc.construct(p, value);
					p++;
				}
				typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
				node_ptr np = m_node_alloc.allocate(count);
				i = 0;
				while (i < (int) count)
				{
					m_node_alloc.construct(np, ListNode<T>(std::move(p[i])));
					np++;
				}
				p = nullptr;
				m_sentinal = m_node_alloc.allocate(1);
				m_node_alloc.construct(m_sentinal, Node_base());
				m_head = std::move(np[0]);
				m_head->m_prev = m_sentinal;
				i = 1;
				pointer temp = m_head;
				while (i < (int) count)
				{
					temp->m_next = std::move(np[i]);
					temp->m_next->m_prev = temp;
					temp = temp->m_next;
				}
				m_tail = temp;
				m_tail->m_next = m_sentinal;
				temp = nullptr;
			}

			// C++14
			explicit List( size_type count,
						   const Allocator& alloc = Allocator()	)
			{
				m_count = 0;

			}

			/**
			template <class InputIterator>
			List(InputIterator first, InputIterator last)
			{
			}

			*/
			List(const List& other) : 
				m_head(other.m_head),
				m_tail(other.m_tail),
				m_sentinal(other.m_sentinal),
				m_count(other.m_count),
		        m_alloc(other.m_alloc) {}

			List(const List& other,
				 const Allocator& alloc) :
				m_head(other.m_head),
				m_tail(other.m_tail),
				m_count(other.m_count),
				m_alloc(alloc) {}
			List(List&& other) 
			{
				this->m_head = other.m_head;
				this->m_tail = other.m_tail;
				this->m_count = std::move(other.m_count);
				this->m_alloc = std::move(other.m_alloc);

				other.m_head = nullptr;
				other.m_tail = nullptr;
			}
			List(std::initializer_list<T> init);
			~List() 
			{

			}
			
			const_reference front() const { return m_head->m_data; }
			reference front() { return m_head->m_data; }
			const_reference back() const { return m_tail->m_data; }
			reference back() { return m_tail->m_data; }

			bool empty();
			size_type size() { return m_count; }

			void clear()
			{

			}

			const alloc_type get_allocator() const { return m_alloc; }
			private:
			node_ptr m_head;
			node_ptr m_tail;
			node_ptr m_sentinal;
			size_type m_count;
			alloc_type m_alloc;
		};
	}

}


#endif // List.hpp
