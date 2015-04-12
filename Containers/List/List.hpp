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
		template <class T, class Allocator> class List;

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

			private:
				template <bool flag_is_const, class True, class False> struct type_picker {};
				template <class True, class False> struct type_picker<true, True, False>
				{
					using type = True;
				};

				template <class True, class False> struct type_picker<false, True, False>
				{
					using type = False;
				};
				template <class U, bool is_const = false> class Iterator
				{
					using iterator_category = fl::iterators::BidirectionalIteratorTag;
					using value_type = U;
					using difference_type = std::ptrdiff_t;
					using pointer = typename type_picker<is_const, const U*, U*>::type;
					using reference = typename type_picker<is_const, const U&, U&>::type;
					using node_ptr = typename type_picker<is_const, const ListNode<U>*, ListNode<U>*>::type;

					private:
						node_ptr m_node;	
						
					public:
						Iterator() {}
						Iterator(node_ptr p = 0) : m_node(p) {}
						Iterator(const Iterator& other) : m_node(other.m_node) {}
					
				};	
			public:
				using iterator = Iterator<T>;
				using const_iterator = Iterator<T, true>;
				using reverse_iterator = fl::iterators::ReverseIterator<iterator>;
				using const_reverse_iterator = fl::iterators::ReverseIterator<const_iterator>;

				List() {}
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
					pointer p = alloc.allocate(count);
					m_count = count;
					m_alloc = alloc;

				}

				/**
				  template <class InputIterator>
				  List(InputIterator first, InputIterator last)
				  {
				  }

				*/
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
