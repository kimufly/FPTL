#ifndef _LIST_HPP
#define _LIST_HPP

#include <iostream>
#include <stdexcept>
#include "../../Iterators/Iterator.hpp"

namespace fl
{
	namespace containers
	{
		
		struct Node_base;
		template <class T> struct ListNode;
		template <class T, class Allocator> class List;

		struct Node_base 
		{
			using pointer = Node_base*;
			Node_base() : m_next(this), m_prev(this) {}

			Node_base& operator=(Node_base& other) 
			{
				m_next = other.m_next;
				m_prev = other.m_prev;	

				return *this;
			}
			virtual ~Node_base() {}
			pointer m_next;
		    pointer m_prev;
		};

		template <class T> struct ListNode : public Node_base
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
			public:
				using value_type = T;
				using alloc_type = Allocator;
				using size_type = std::size_t;
				using difference_type = std::ptrdiff_t;
				using node_type = ListNode<T>;
				using node_base_type = Node_base;
				using node_ptr = node_type*;
				using node_base_ptr = node_base_type*;
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

					friend class List<U>;

					public:
						using iterator_category = fl::iterators::BidirectionalIteratorTag;
						using value_type = typename type_picker<is_const, const U, U>::type;
						using difference_type = std::ptrdiff_t;
						using pointer = typename type_picker<is_const, const U*, U*>::type;
						using reference = typename type_picker<is_const, const U&, U&>::type;
						using node_ptr = typename type_picker<is_const, const ListNode<U>*, ListNode<U>*>::type;
					
					private:
						node_ptr m_node;	

					public:
						Iterator() {}
						Iterator(node_ptr p) : m_node(p) {}
						Iterator(const Iterator& other) : m_node(other.m_node) {}
						~Iterator() {}

						reference operator*() const
						{
							return m_node->m_data;
						}
						pointer operator->() const
						{

						}
						Iterator& operator++()
						{
							m_node = dynamic_cast<node_ptr>(m_node->m_next);
							return *this;
						}
						Iterator operator++(int)
						{
							Iterator it(*this);
							m_node = dynamic_cast<node_ptr>(m_node->m_next);
							return it;
						}

						Iterator& operator--()
						{
							m_node = dynamic_cast<node_ptr>(m_node->m_prev);
							return *this;
						}
						Iterator operator--(int)
						{
							Iterator it(*this);
							m_node = dynamic_cast<node_ptr>(m_node->m_prev);
							return it;
						}

						bool operator==(const Iterator& rhs)
						{
							return this->m_node == rhs.m_node;
						}

						bool operator!=(const Iterator& rhs)
						{
							return this->m_node != rhs.m_node;
						}

						
					
				};	
			public:
				using iterator = Iterator<T>;
				using const_iterator = Iterator<T, true>;
				using reverse_iterator = fl::iterators::ReverseIterator<iterator>;
				using const_reverse_iterator = fl::iterators::ReverseIterator<const_iterator>;

				explicit List(const Allocator& alloc = Allocator()) 
				{ 
					m_alloc = alloc;
					typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
					node_base_ptr np = m_node_alloc.allocate(1);
					m_node_alloc.construct(np, 0);
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
					typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;

					try
					{
						node_ptr np = m_node_alloc.allocate(count);
						size_type i = 0;
						while (i < count)
						{
							m_node_alloc.construct(np+i, value);
							i++;
						}
						m_sentinal = m_node_alloc.allocate(1);
						m_node_alloc.construct(m_sentinal, 0);
						m_head = &np[0];
						m_head->m_prev = m_sentinal;
						m_sentinal->m_next = m_head;
						i = 1;
						node_ptr temp = m_head;
						while (i < count)
						{
							temp->m_next = &np[i];
							temp->m_next->m_prev = temp;
							temp = (node_ptr) temp->m_next;
							i++;
						}
						m_tail = temp;
						m_tail->m_next = m_sentinal;
						m_sentinal->m_prev = m_tail;
						temp = nullptr;
					}
					catch (const std::bad_alloc& ex)
					{
						std::cout << "allocation failed." << std::endl;
					}

				}

				// C++14
				explicit List( size_type count,
						const Allocator& alloc = Allocator())
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

				iterator begin() { return m_head; }
				iterator begin() const { return m_head; }
				const_iterator cbegin() const { return const_iterator(m_head); }

				iterator end() { return iterator(dynamic_cast<node_ptr>(m_tail->m_next)); }
				iterator end() const { return iterator(dynamic_cast<node_ptr>(m_tail->m_next)); }
				const_iterator cend() const { return const_iterator(dynamic_cast<node_ptr>(m_tail->m_next)); }

				reverse_iterator rbegin() { return reverse_iterator(end()); }
				reverse_iterator rbegin() const { return reverse_iterator(end()); }
				const_reverse_iterator rcbegin() const { return const_reverse_iterator(cend()); }

				reverse_iterator rend() { return reverse_iterator(begin()); }
				reverse_iterator rend() const { return reverse_iterator(begin()); }
				const_reverse_iterator rcend() const { return const_reverse_iterator(cbegin()); }

				const_reference front() const { return m_head->m_data; }
				reference front() { return m_head->m_data; }
				const_reference back() const { return m_tail->m_data; }
				reference back() { return m_tail->m_data; }

				bool empty()
				{
					return m_count == 0 && begin() == end();
				}
				size_type size() { return m_count; }

				void clear()
				{
					if (!empty())
						delete this;
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
