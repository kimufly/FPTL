#ifndef _LIST_HPP
#define _LIST_HPP

#include <iostream>
#include <stdexcept>
#include <type_traits>
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
			~ListNode() override {}
			value_type m_data;
		};


		template <
				   class T, 
				   class Allocator = std::allocator<T> >
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
				node_ptr m_acquire_node()
				{
					try
					{
						typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
						return m_node_alloc.allocate(1);
					}
					catch (const std::bad_alloc& e)
					{
						std::cerr << "Allocation of node failed." << std::endl;
						return nullptr;
					}
				}

				void m_construct_node(node_ptr node, const_reference value)
				{
					typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
					m_node_alloc.construct(node, value);
				}

				void m_destroy_node(node_ptr node)
				{
					typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
					m_node_alloc.destroy(node);
				}

				void m_put_node(node_ptr node)
				{
					typename decltype(m_alloc)::template rebind<node_type>::other dealloc;
					dealloc.destroy(node);
					dealloc.deallocate(node, 1);
					node = nullptr;
				}
				template <class Integer>
				void m_initilize_dispatch(Integer count, Integer& value, std::true_type)
				{
					m_fill_initialize(static_cast<size_type>(count), static_cast<value_type>(value));
				}

				template <class InputIt>
				void m_initilize_dispatch(InputIt first, InputIt last, std::false_type)
				{
					for (; first != last; ++first)
					{
						push_back(*first);
					}
				}

				void m_fill_initialize(size_type count, const T& value)
				{
					for (int i = 0; i < (int) count; i++)
					{
						push_back(value);
					}
				}

				template <bool flag_is_const, class Const, class No_const> struct type_picker {};
				template <class Const, class No_const> struct type_picker<true, Const, No_const>
				{
					using type = Const;
				};

				template <class Const, class No_const> struct type_picker<false, Const, No_const>
				{
					using type = No_const;
				};
				template <class U, bool is_const = false> struct Iterator
				{

					using iterator_category = fl::iterators::BidirectionalIteratorTag;
					using value_type = typename type_picker<is_const, const U, U>::type;
					using difference_type = std::ptrdiff_t;
					using pointer = typename type_picker<is_const, const U*, U*>::type;
					using reference = typename type_picker<is_const, const U&, U&>::type;
					using node_ptr = typename type_picker<is_const, const ListNode<U>*, ListNode<U>*>::type;

					node_ptr m_node;	

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
						return &(operator*());
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

					Iterator operator-(int n) const
					{
						node_ptr tmp = this->m_node;
						for (int i = 0 ; i < n; i++)
						{
							tmp = tmp->m_prev;
						}
						return Iterator(tmp);
					}

					Iterator operator+(int n) const
					{
						node_ptr tmp = this->m_node;
						for (int i = 0 ; i < n; i++)
						{
							tmp = tmp->m_next;
						}
						return Iterator(tmp);
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

			protected:
				void m_insert(iterator pos, const T& value)
				{
					if (pos.m_node == m_head)
					{
						push_front(value);
					}
					else
					{
						node_ptr new_node = m_acquire_node();
						m_construct_node(new_node, value);
						new_node->m_next = pos.m_node;
						new_node->m_prev = pos.m_node->m_prev;
						pos.m_node->m_prev->m_next = new_node;
						pos.m_node->m_prev = new_node;
						if (pos.m_node == m_tail)
							m_tail = new_node;
						m_count++;
					}
				}
				void m_insert(const_iterator pos, const T& value)
				{
					if (pos.m_node == m_head)
					{
						push_front(value);
					}
					else
					{
						iterator temp(pos.m_node);
						node_ptr new_node = m_acquire_node();
						m_construct_node(new_node, value);
						new_node->m_next = temp.m_node;
						new_node->m_prev = temp.m_node->m_prev;
						temp.m_node->m_prev->m_next = new_node;
						temp.m_node->m_prev = new_node;
						if (temp.m_node == m_tail)
							m_tail = new_node;
						m_count++;
					}
				}

				void m_fill_insert(iterator pos, size_type count, const T& value)
				{
					iterator temp(pos.m_node);
					for (int i = 0; i < (int) count; i++)
					{
						m_insert(temp, value);
						--temp;
					}
				}

				template <class Integer>
				void m_insert(iterator pos, Integer count, Integer value, std::true_type)
				{
					m_fill_insert(pos, static_cast<size_type>(count), static_cast<value_type>(value));
				}

				template <class InputIt>
				void m_insert(iterator pos, InputIt first, InputIt last, std::false_type)
				{
				}

			public:
				explicit List(const Allocator& alloc = Allocator()) 
				{
					m_alloc = alloc;
					m_count = 0;
					m_sentinal = m_acquire_node();
					m_construct_node(m_sentinal, 0);
					m_head = m_sentinal;
					m_tail = m_sentinal;
				}

				List(List&& other)
				{
					m_head = other.m_head;
					m_tail = other.m_tail;
					m_sentinal = other.m_sentinal;

					other.m_head = nullptr;
					other.m_tail = nullptr;
					other.m_sentinal = nullptr;

					m_count = std::move(other.m_count);
				}
				List(List&& other, const Allocator& alloc)
				{
					m_head = other.m_head;
					m_tail = other.m_tail;
					m_sentinal = other.m_sentinal;

					other.m_head = nullptr;
					other.m_tail = nullptr;
					other.m_sentinal = nullptr;

					m_count = std::move(other.m_count);
					m_alloc = alloc;
				}

				template <class InputIterator>
				List( InputIterator first, 
					  InputIterator last,
					  const Allocator& alloc = Allocator())
				{
					using Integral = typename std::is_integral<InputIterator>::type;	
					m_alloc = alloc;
					m_initilize_dispatch(first, last, Integral());
				}

				List(std::initializer_list<T> init)
				{
					m_count = init.size();
					iterator it = init.begin();
					for (; it != init.end(); ++it)
					{
						push_back(*it);
					}
				}
				~List() 
				{
					clear();	
					m_destroy_node(m_sentinal);
					m_put_node(m_sentinal);
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

				size_type size() const { return m_count; }

				void clear()
				{
					node_ptr current = m_head;
					while (current != m_sentinal)
					{
						node_ptr temp = current;
						m_destroy_node(temp);
						m_put_node(temp);
						current = dynamic_cast<node_ptr>(current->m_next);
					}
					current = nullptr;
					m_head = m_sentinal;
					m_tail = m_sentinal;
				}

				iterator insert(iterator pos, const T& value)
				{
					m_insert(pos, value);
					return --pos;
				}
				iterator insert(const_iterator pos, const T& value)
				{
					iterator temp(pos);
					m_insert(temp, value);
					return --temp;
				}

				iterator insert(const_iterator pos, T&& value);

				template <class InputIt>
				iterator insert(iterator pos, InputIt first, InputIt last)
				{
					using Integer = typename std::is_integral<InputIt>::type;
					m_insert(pos, first, last, Integer());
				}

				void push_front(const T& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, value);
					if (m_count == 0 && m_head == m_tail)
					{
						m_head = new_node;
						m_tail = new_node;
						m_head->m_prev = m_sentinal;
						m_tail->m_next = m_sentinal;
					}
					else
					{
						m_head->m_prev = new_node;
						new_node->m_next = m_head;
						new_node->m_prev = m_sentinal;
						m_head = new_node;
					}
					m_count++;

				}
				void push_front(T&& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, std::move(value));
					if (m_count == 0 && m_head == m_tail)
					{
						m_head = new_node;
						m_tail = new_node;
						m_head->m_prev = m_sentinal;
						m_tail->m_next = m_sentinal;
					}
					else
					{
						m_head->m_prev = new_node;
						new_node->m_next = m_head;
						new_node->m_prev = m_sentinal;
						m_head = new_node;
					}
					m_count++;
				}

				void push_back(const T& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, value);
					if (m_count == 0 && m_head == m_tail)
					{
						m_head = new_node;
						m_tail = new_node;
						m_head->m_prev = m_sentinal;
						m_tail->m_next = m_sentinal;
					}
					else
					{
						new_node->m_prev = m_tail;
						new_node->m_next = m_sentinal;
						m_tail->m_next = new_node;
						m_tail = new_node;
					}
					m_count++;
				}
				void push_back(T&& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, std::move(value));
					if (m_count == 0 && m_head == m_tail)
					{
						m_head = new_node;
						m_tail = new_node;
						m_head->m_prev = m_sentinal;
						m_tail->m_next = m_sentinal;
					}
					else
					{
						new_node->m_prev = m_tail;
						new_node->m_next = m_sentinal;
						m_tail->m_next = new_node;
						m_tail = new_node;
					}
					m_count++;
				}

				void pop_front()
				{
					node_ptr new_head = m_head->m_next;
					new_head->m_prev = m_sentinal;
					m_destroy_node(m_head);
					m_put_node(m_head);
					m_head = new_head;
				}
				void pop_back()
				{
					node_ptr new_tail = m_tail->m_prev;
					new_tail->m_next = m_sentinal;
					m_destroy_node(m_tail);
					m_put_node(m_tail);
					m_tail = new_tail;
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
