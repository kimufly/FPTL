#include <stddef>
#include <iterator>

#ifndef _LIST_HPP
#define _LIST_HPP

namespace fl
{
	template <class T> class List
	{
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;
		using pointer_type = T*;
		using reference = T&;
		using const_reference = const T&;

		public:
			List() {}
			List( std::size_t count, const T& value);
			explicit List( size_type count );
			template <class InputIterator>
				List(InputIterator first, InputIterator last);
			List(const List& other);
			List(List&& other);
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
			size_type max_size() { return std::distance(begin(), end()); }

			void clear();



			
		private:
			pointer_type m_head;
			pointer_type m_tail;
			size_type m_count;
			
	};

}


#endif // List.hpp
