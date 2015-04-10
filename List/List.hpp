#ifndef _LIST_HPP
#define _LIST_HPP

#include <cstddef>
#include <iterator>

namespace fl
{
	namespace containers
	{
		
		template <class T> struct Iterator
		{
			bool operator==(const Iterator& rhs);

		};

		template <class T> struct InputIterator
		{
		};

		template <class T> struct ReverseIterator
		{
		};

		template <class T> struct Node
		{

			Node() 
			{
				m_next = this;
				m_prev = this;
				m_value = nullptr;
			}
			~Node() {}
			T* m_value;
			Node* m_next;
			Node* m_prev;
		};

		template <class T> class List
		{
			using difference_type = std::ptrdiff_t;
			using node_ptr = Node<T>*;
			using size_type = std::size_t;
			using value_type = T;
			using pointer_type = value_type*;
			using reference = value_type&;
			using const_reference = const value_type&;
			using node_reference = Node<T>&;
			using const_node_reference = const Node<T>&;

			public:
			List() {}
			List( std::size_t count, const T& value);
			explicit List( size_type count );
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

			const_reference front() const { return &(*m_head); }
			reference front() { return &(*m_head); }
			const_reference back() const { return &(*m_tail); }
			reference back() { return &(*m_tail); }

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
