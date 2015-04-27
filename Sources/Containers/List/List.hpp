#ifndef _LIST_HPP
#define _LIST_HPP

#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <algorithm>
#include "../../Iterators/Iterator.hpp"

namespace fl
{
	namespace containers
	{
		struct Node_base;
		template <class T> struct ListNode;
		template <class T, class Allocator> class List;
		template <class T, bool is_const = false> struct Iterator;

		struct Node_base 
		{
			Node_base() : m_next(this), m_prev(this) {}

			virtual ~Node_base() {}
			Node_base* m_next;
			Node_base* m_prev;
		};

		template <class T> struct ListNode : public Node_base
		{
			using value_type = T;
			ListNode(const T& t) : m_data(t) {}
			ListNode(T&& t) : m_data(std::move(t)) {}
			~ListNode() override {}
			value_type m_data;
		};

		template <bool flag_is_const, class Const, class No_const> struct type_picker {};
		template <class Const, class Non_const> struct type_picker<true, Const, Non_const>
		{
			using type = Const;
		};

		template <class Const, class Non_const> struct type_picker<false, Const, Non_const>
		{
			using type = Non_const;
		};
		template <class T, bool is_const> struct Iterator
		{
			using iterator_category = fl::iterators::BidirectionalIteratorTag;
			using value_type = typename type_picker<is_const, const T, T>::type;
			using difference_type = std::ptrdiff_t;
			using pointer = typename type_picker<is_const, const T*, T*>::type;
			using reference = typename type_picker<is_const, const T&, T&>::type;
			using node_ptr = ListNode<T>*;

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

			bool operator==(const Iterator& rhs)
			{
				return this->m_node == rhs.m_node;
			}

			bool operator!=(const Iterator& rhs)
			{
				return this->m_node != rhs.m_node;
			}

		};	

		template < class T, class Allocator = std::allocator<T> >
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

				template <class ...Args>
				void m_construct_node(node_ptr node, Args&&... args)
				{
					typename decltype(m_alloc)::template rebind<node_type>::other m_node_alloc;
					m_node_alloc.construct(node, std::forward<Args>(args)...);
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

			public:

				using iterator = Iterator<T>;
				using const_iterator = Iterator<T, true>;
				using reverse_iterator = fl::iterators::ReverseIterator<iterator>;
				using const_reverse_iterator = fl::iterators::ReverseIterator<const_iterator>;

			protected:
				void m_fill_assign(size_type count, const T& value)
				{
					clear();
					m_initilize_dispatch(count, value, std::true_type());
				}

				template <class Integer>
				void m_assign_dispatch(Integer count, Integer& value, std::true_type)
				{
					m_fill_assign(static_cast<size_type>(count), static_cast<value_type>(value));
				}

				template <class InputIt>
				void m_assign_dispatch(InputIt first, InputIt last, std::false_type)
				{
					clear();
					m_initilize_dispatch(first, last, std::false_type());
				}
				void m_insert(iterator pos, const T& value)
				{
					if (pos.m_node == m_head)
					{
						push_front(value);
					}
					else if (pos == end())
					{
						push_back(value);
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
						{
							m_tail = new_node;
							m_tail->m_next = m_sentinal;
							m_sentinal->m_prev = m_tail;
						}
						m_count++;
					}
				}
				void m_insert(const_iterator pos, const T& value)
				{
					if (pos == cbegin())
					{
						push_front(value);
					}
					else if (pos == cend())
					{
						push_back(value);
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
						{
							m_tail = new_node;
							m_tail->m_next = m_sentinal;
							m_sentinal->m_prev = m_tail;
						}
						m_count++;
					}
				}

				void m_fill_insert(const_iterator pos, size_type count, const T& value)
				{
					for (int i = 0; i < (int) count; i++)
					{
						m_insert(pos, value);
					}
				}

				template <class Integer>
				void m_insert_dispatch(const_iterator pos, Integer count, Integer value, std::true_type)
				{
					m_fill_insert(pos, static_cast<size_type>(count), static_cast<value_type>(value));
				}

				template <class InputIt>
				void m_insert_dispatch(const_iterator pos, InputIt first, InputIt last, std::false_type)
				{

					for (; first != last; ++first)
					{
						m_insert(pos, *first);
					}
				}

				template <class ...Args>
				void m_insert(const_iterator pos, Args&&... args)
				{

					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, std::forward<Args>(args)...);
					
					if (m_count == 0)
					{
						m_head = new_node;
						m_tail = new_node;
						m_head->m_prev = m_sentinal;
						m_tail->m_next = m_sentinal;
						m_sentinal->m_prev = m_tail;
						m_sentinal->m_next = m_head;
					}
					else
					{
						if (pos == cbegin())
						{
							m_head = new_node;
							m_head->m_prev = m_sentinal;
							m_sentinal->m_next = m_head;
						}
						if (pos == cend())
						{
							m_tail = new_node;
							m_tail->m_next = m_sentinal;
							m_sentinal->m_prev = m_tail;
						}

						new_node->m_prev = pos.m_node->m_prev;
						new_node->m_next = pos.m_node;

						pos.m_node->m_prev = new_node;
						new_node->m_prev->m_next = new_node;
					}
					
					m_count++;

				}

				void m_erase(const_iterator pos)
				{
					node_ptr tmp = pos.m_node;
					if (pos.m_node->m_next == m_sentinal)
					{
						m_tail = dynamic_cast<node_ptr>(pos.m_node->m_prev);
					}
					
					tmp->m_prev->m_next = pos.m_node->m_next;
					tmp->m_next->m_prev = pos.m_node->m_prev;
					m_destroy_node(pos.m_node);
					m_put_node(pos.m_node);
					m_count--;
				}
				void m_erase(const_iterator first, const_iterator last)
				{
					for (; first != last; ++first)
					{
						m_erase(first);
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

				List(const List& other)
				{
					m_alloc = std::allocator_traits<alloc_type>::select_on_container_copy_construction(other.get_allocator());
					m_sentinal = m_acquire_node();
					m_construct_node(m_sentinal, 0);
					m_initilize_dispatch(other.begin(), other.end(), std::false_type());
					m_count = other.m_count;
				}

				List(const List& other, const Allocator& alloc)
				{
					m_alloc = alloc;
					m_initilize_dispatch(other.begin(), other.end(), std::false_type());
					m_count = other.m_count;
				}

				List(List&& other)
				{
					m_initilize_dispatch(other.begin(), other.end(), std::false_type());
					for (auto it = other.begin(); it != other.end(); ++it)
					{
						it.m_node = nullptr;
					}
					other.m_head = nullptr;
					other.m_tail = nullptr;
					m_count = std::move(other.m_count);
					m_alloc = std::move(other.get_allocator());

				}

				List(List&& other, const Allocator& alloc)
				{
					m_alloc = std::move(alloc);
					m_count = std::move(other.m_count);
					m_initilize_dispatch(other.begin(), other.end(), std::false_type());
					for (auto it = other.begin(); it != other.end(); ++it)
					{
						it.m_node = nullptr;
					}
					other.m_head = nullptr;
					other.m_tail = nullptr;
				}

				template <class InputIterator>
				List( InputIterator first, 
					  InputIterator last,
					  const Allocator& alloc = Allocator())
				{
					using Integral = typename std::is_integral<InputIterator>::type;	
					m_alloc = alloc;
					m_sentinal = m_acquire_node();
					m_construct_node(m_sentinal, 0);
					m_head = m_sentinal;
					m_tail = m_sentinal;
					m_count = 0;
					m_initilize_dispatch(first, last, Integral());
				}

				List(std::initializer_list<T> init, const Allocator& alloc = Allocator())
				{
					m_alloc = alloc;
					m_sentinal = m_acquire_node();
					m_construct_node(m_sentinal, 0);
					m_head = m_sentinal;
					m_tail = m_sentinal;
					for (auto it = init.begin(); it != init.end(); ++it)
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

				List& operator=(const List& other)
				{
					assign(other.begin(), other.end());
					return *this;
				}
				List& operator=(List&& other)
				{
					assign(other.begin(), other.end());
					return *this;
				}
				List& operator=(std::initializer_list<T>& ilist)
				{
					this->clear();
					for (auto it = ilist.begin(); it != ilist.end(); ++it)
					{
						push_back(*it);
					}
					return *this;
				}

				template <class InputIt>
				void assign(InputIt first, InputIt last)
				{
					using Integer = typename std::is_integral<InputIt>::type;
					m_assign_dispatch(first, last, Integer());
				}

				void assign(std::initializer_list<T> ilist);

				iterator begin() { return m_head; }
				iterator begin() const { return m_head; }
				const_iterator cbegin() const { return const_iterator(m_head); }

				iterator end() { return iterator(dynamic_cast<node_ptr>(m_sentinal)); }
				iterator end() const { return iterator(dynamic_cast<node_ptr>(m_sentinal)); }
				const_iterator cend() const { return const_iterator(dynamic_cast<node_ptr>(m_sentinal)); }

				reverse_iterator rbegin() { return reverse_iterator(end()); }
				reverse_iterator rbegin() const { return reverse_iterator(end()); }
				const_reverse_iterator rcbegin() const { return const_reverse_iterator(cend()); }

				reverse_iterator rend() { return reverse_iterator(begin()); }
				reverse_iterator rend() const { return reverse_iterator(begin()); }
				const_reverse_iterator rcend() const { return const_reverse_iterator(cbegin()); }

				const_reference front() const { return *cbegin(); }
				reference front() { return *begin(); }
				const_reference back() const { return *(--cend()); }
				reference back() { return *(--end()); }

				bool empty()
				{
					return m_count == 0 && m_head == m_tail;
				}

				size_type size() const { return m_count; }

				size_type max_size() const { return m_alloc.max_size(); }

				void resize(size_type count)
				{
					int iteration = ((int) m_count - (int) count) >= 0 ? m_count - count : count - m_count;
					if (count == 0)
						this->clear();

					if (size() > count)
					{
						for (int i = 0; i < iteration; i++)
						{
							pop_back();
						}
					}
					if (size() < count)
					{
						for (int i = 0; i < iteration; i++)
							push_back(0);
					}
				}
				void resize(size_type count, const value_type& value)
				{
					int iteration = ((int) m_count - (int) count) >= 0 ? m_count - count : count - m_count;
					if (count == 0)
						this->clear();

					if (size() > count)
					{
						for (int i = 0; i < iteration; i++)
						{
							pop_back();
						}
					}
					if (size() < count)
					{
						for (int i = 0; i < iteration; i++)
							push_back(value);
					}

				}

				void clear()
				{
					if (!empty())
					{
						node_ptr current = m_head;
						while (current != m_sentinal)
						{
							node_ptr temp = current;
							current = dynamic_cast<node_ptr>(current->m_next);
							if (current)
							{
								m_destroy_node(temp);
								m_put_node(temp);
							}
						}
						current = nullptr;
						m_count = 0;
						m_head = m_sentinal;
						m_tail = m_sentinal;
					}
				}

				iterator insert(iterator pos, const T& value)
				{
					m_insert(pos, value);
					return --pos;
				}
				iterator insert(const_iterator pos, const T& value)
				{
					iterator ret(pos.m_node);
					m_insert(ret, value);
					return --ret;
				}

				iterator insert(const_iterator pos, T&& value)
				{
					iterator ret(pos.m_node);
					m_insert(ret, std::move(value));
					return --ret;
				}

				template <class InputIt>
				iterator insert(const_iterator pos, InputIt first, InputIt last)
				{
					using Integer = typename std::is_integral<InputIt>::type;
					iterator ret(dynamic_cast<node_ptr>(pos.m_node->m_prev));
					m_insert_dispatch(pos, first, last, Integer());
					return ++ret;
				}

				template <class ...Args>
				iterator emplace(const_iterator pos, Args&&... args)
				{
					m_insert(pos, std::forward<Args>(args)...);
					iterator ret(pos.m_node);
					return --ret;
				}

				template <class ...Args>
				void emplace_back(Args&&... args)
				{
					m_insert(cend(), std::forward<Args>(args)...);
				}

				template <class ...Args>
				void emplace_front(Args&&... args)
				{
					m_insert(cbegin(), std::forward<Args>(args)...);
				}

				iterator erase(const_iterator pos)
				{
					iterator ret(dynamic_cast<node_ptr>(pos.m_node->m_next));
					m_erase(pos);
					return ret;
				}
				iterator erase(const_iterator first, const_iterator last)
				{
					for (; first != last; ++first)
					{
						m_erase(first);
					}
					return iterator(last.m_node);
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
					m_sentinal->m_prev = m_tail;
					m_sentinal->m_next = m_head;
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
					m_sentinal->m_prev = m_tail;
					m_sentinal->m_next = m_head;
					m_count++;
				}

				void push_back(const T& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, value);
					if (m_count == 0 && m_head == m_tail)
					{
						new_node->m_next = m_sentinal;
						new_node->m_prev = m_sentinal;
						m_head = new_node;
						m_tail = new_node;
					}
					else
					{
						new_node->m_prev = m_tail;
						new_node->m_next = m_sentinal;
						m_tail->m_next = new_node;
						m_tail = new_node;
					}
					m_sentinal->m_prev = m_tail;
					m_sentinal->m_next = m_head;
					m_count++;
				}

				void push_back(T&& value)
				{
					node_ptr new_node = m_acquire_node();
					m_construct_node(new_node, std::move(value));
					if (m_count == 0 && m_head == m_tail)
					{
						new_node->m_next = m_sentinal;
						new_node->m_prev = m_sentinal;
						m_head = new_node;
						m_tail = new_node;
					}
					else
					{
						new_node->m_prev = m_tail;
						new_node->m_next = m_sentinal;
						m_tail->m_next = new_node;
						m_tail = new_node;
					}
					m_sentinal->m_prev = m_tail;
					m_sentinal->m_next = m_head;
					m_count++;
				}

				void pop_front()
				{
					node_ptr new_head = dynamic_cast<node_ptr>(m_head->m_next);
					new_head->m_prev = m_sentinal;
					if (m_head)
					{
						m_destroy_node(m_head);
						m_put_node(m_head);
					}
					m_head = new_head;
					m_count--;
				}
				void pop_back()
				{
					node_ptr new_tail = dynamic_cast<node_ptr>(m_tail->m_prev);
					new_tail->m_next = m_sentinal;
					if (m_tail)
					{
						m_destroy_node(m_tail);
						m_put_node(m_tail);
					}
					m_tail = new_tail;
					m_count--;
				}

				alloc_type get_allocator() const { return m_alloc; }

			private:
				node_ptr m_head;
				node_ptr m_tail;
				node_ptr m_sentinal;
				size_type m_count;
				alloc_type m_alloc;
		};

		template <class T, class Alloc>
		bool operator==(const List<T, Alloc>& lhs, const List<T, Alloc>& rhs)
		{
			if (lhs.size() == rhs.size())
			{
				auto it1 = lhs.cbegin();
				auto it2 = rhs.cbegin();
				for (; it1 != lhs.cend() && it2 != rhs.cend(); ++it1)
				{
					if (*it1 != *it2)
						return false;
					++it2;
				}
				return true;
			}
			return false;
		}
		template <class T, class Alloc>
		bool operator!=(const List<T, Alloc>& lhs, const List<T, Alloc>& rhs)
		{
			if (lhs.size() == rhs.size())
			{
				auto it1 = lhs.cbegin();
				auto it2 = rhs.cbegin();
				for (; it1 != lhs.cend() && it2 != rhs.cend(); ++it1)
				{
					if (*it1 != *it2)
						return true;
					++it2;
				}
				return false;
			}
			return true;
		}

		template <class T, class Alloc>
		bool operator<(const List<T, Alloc>& lhs, const List<T, Alloc>& rhs)
		{
			if (lhs.size() < rhs.size())
			{
				return true;
			}
			else if (lhs.size() > rhs.size())
			{
				return false;
			}
			else
			{
				auto it1 = lhs.cbegin();
				auto it2 = rhs.cbegin();
				for (; it1 != lhs.cend() && it2 != rhs.cend(); ++it1)
				{
					if (*it1 > *it2)
						return false;
					++it2;
				}
				return true;
			}
		}
		template <class T, class Alloc>
		bool operator<=(const List<T, Alloc>& lhs, const List<T, Alloc>& rhs)
		{
			if (lhs.size() < rhs.size())
			{
				return true;
			}
			else if (lhs.size() > rhs.size())
			{
				return false;
			}
			else
			{
				auto it1 = lhs.cbegin();
				auto it2 = rhs.cbegin();
				for (; it1 != lhs.cend() && it2 != rhs.cend(); ++it1)
				{
					if (*it1 > *it2)
						return false;
					++it2;
				}
				return true;
			}

		}
		template <class T, class Alloc>
		bool operator>(const List<T, Alloc>& lhs, const List<T, Alloc>& rhs)
		{
			if (lhs.size() > rhs.size())
			{
				return true;
			}
			else if (lhs.size() < rhs.size())
			{
				return false;
			}
			else
			{
				auto it1 = lhs.cbegin();
				auto it2 = rhs.cbegin();
				for (; it1 != lhs.cend() && it2 != rhs.cend(); ++it1)
				{
					if (*it1 < *it2)
						return false;
					++it2;
				}
				return true;
			}
		}

		template <class T, class Alloc>
		bool operator>=(const List<T, Alloc>& lhs, const List<T, Alloc>& rhs)
		{
			if (lhs.size() > rhs.size())
			{
				return true;
			}
			else if (lhs.size() < rhs.size())
			{
				return false;
			}
			else
			{
				auto it1 = lhs.cbegin();
				auto it2 = rhs.cbegin();
				for (; it1 != lhs.cend() && it2 != rhs.cend(); ++it1)
				{
					if (*it1 < *it2)
						return false;
					++it2;
				}
				return true;
			}
		}
	}

}

#endif // List.hpp
