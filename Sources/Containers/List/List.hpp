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

				template <class InputIt>
				void m_iterator_initialize(InputIt first,
						InputIt last,
						const Allocator& alloc = Allocator())
				{

				}

				void m_fill_initialize(size_type count,
									   const T& value,
									   const Allocator& alloc = Allocator())
				{
					m_count = count;
					m_alloc = alloc;
					m_sentinal = m_acquire_node();
					m_construct_node(m_sentinal, 0);
					node_ptr np;
					for (int i = 0; i < (int) count; i++)
					{
						np = m_acquire_node();
						m_construct_node(np, value);
						np++;
					}
					np = np - count + 1;
					for (int i = 0; i < (int) count-1; i++)
					{
						np[i].m_next = &np[i+1];
						np[i+1].m_prev = &np[i];
					}

					m_head = np;
					m_tail = &np[count-1];

					m_head->m_prev = m_sentinal;
					m_tail->m_next = m_sentinal;
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

			private:
				void m_insert(iterator pos, const T& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, value);
					new_node->m_next = pos.m_node;
					new_node->m_prev = pos.m_node->m_prev;
					pos.m_node->m_prev->m_next = new_node;
					pos.m_node->m_prev = new_node;
					m_count++;
				}
				void m_insert(const_iterator pos, const T& value)
				{
					iterator temp(pos.m_node);
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, value);
					new_node->m_next = temp.m_node;
					new_node->m_prev = temp.m_node->m_prev;
					temp.m_node->m_prev->m_next = new_node;
					temp.m_node->m_prev = new_node;
					m_count++;
				}
				void m_insert(iterator pos, size_type count, const T& value)
				{
					m_count += count;
					node_ptr np;
					for (int i = 0; i < (int) count; i++)
					{
						np = m_acquire_node();
						m_construct_node(np, value);
						np++;
					}
					np = np - count + 1;
					for (int i = 0; i < (int) count-1; i++)
					{
						np[i].m_next = &np[i+1];
						np[i+1].m_prev = &np[i];	
					}
					np[0].m_prev = pos.m_node->m_prev;
					np[count-1].m_next = pos.m_node;
					pos.m_node->m_prev->m_next = &np[0];
					pos.m_node->m_prev = &np[count-1];
				}
				template <class InputIt>
				void m_insert(iterator pos, InputIt first, InputIt last)
				{

					pos.m_node->m_prev = last.m_node->m_prev;
					
					for (InputIt it = first; it != last; ++it)
					{
						--pos;
					}

					

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

				List( size_type count, 
						const T& value, 
						const Allocator& alloc = Allocator() )
				{
					m_fill_initialize(count, value, alloc);
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
					m_alloc = std::move(alloc);
				}

				template <class InputIterator>
					List(InputIterator first, 
							InputIterator last,
							const Allocator& alloc = Allocator())
					{
						if (std::is_integral<InputIterator>::value)
						{
							m_fill_initialize(first, last, alloc);
						}
						else
						{
							m_iterator_initialize(first, last, alloc);
						}
					}

				List(std::initializer_list<T> init)
				{
					m_count = init.size();
				}
				~List() 
				{
					clear();	
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

				iterator insert(iterator pos, size_type count, const T& value) 
				{
					if (count == 0)
						return pos;
					m_insert(pos, count, value);
					for (int i = 0; i < count+1; i++)
					{
						--pos;
					}
					return iterator(pos.m_node);
				}

				template <class InputIt>
					iterator insert(iterator pos, InputIt first, InputIt last)
					{
						if (std::is_integral<InputIt>::value)
						{
							return insert(pos, first, last);
						}
						else
						{

						}

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
