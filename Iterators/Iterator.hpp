#ifndef _ITERATOR_HPP
#define _ITERATOR_HPP

#include <cstddef>
#include <memory>

namespace fl
{
	namespace iterators
	{

		struct InputIteratorTag {};
		struct OutputIteratorTag {};
		struct ForwardIteratorTag {};
		struct BidirectionalIteratorTag : public InputIteratorTag {};
		struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

		template <class Iterator>
		struct IteratorTraits
		{
			using value_type = typename Iterator::value_type;
			using difference_type = typename Iterator::difference_type;
			using pointer = typename Iterator::pointer;
			using reference = typename Iterator::reference;
			using iterator_category = typename Iterator::iterator_category;
		};

		template <class T>
		struct IteratorTraits<T*>
		{
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = RandomAccessIteratorTag;
		};

		template <class T>
		struct IteratorTraits<const T*>
		{
			using difference_type = std::ptrdiff_t;
			using value_type = T;
			using pointer = const T*;
			using reference = const T&;
			using iterator_category = RandomAccessIteratorTag;
		};

		template <
			class Category,
			class T,
			class Distance = std::ptrdiff_t,
			class Pointer = T*,
			class Reference = T&
		> struct Iterator
		{
			using value_type = T;
			using pointer = Pointer;
			using reference = Reference;
			using difference_type = Distance;
			using iterator_category = Category;
		};

		template <class Iter>
		class ReverseIterator : public Iterator<
									typename IteratorTraits<Iter>::iterator_category,
									typename IteratorTraits<Iter>::value_type,
									typename IteratorTraits<Iter>::difference_type,
									typename IteratorTraits<Iter>::pointer,
									typename IteratorTraits<Iter>::reference >
		{
			using iterator_type = Iter;
			using difference_type = typename IteratorTraits<Iter>::difference_type;
			using pointer = typename IteratorTraits<Iter>::pointer;
			using reference = typename IteratorTraits<Iter>::reference;

			protected:
				iterator_type current;

			public:
				ReverseIterator() {}
				explicit ReverseIterator( iterator_type x ) : current(x) {}

				iterator_type base() const { return current; }

				reference operator*() const
				{
					Iter tmp = current;
					return *--tmp;
				}

				pointer operator->() const
				{
					return std::addressof(operator*());
				}

				ReverseIterator& operator++()
				{
					--current;
					return *this;
				}
				ReverseIterator& operator--()
				{
					++current;
					return *this;
				}

				ReverseIterator operator++(int)
				{
					ReverseIterator rit = *this;
					--current;
					return rit;
				}
				ReverseIterator operator--(int)
				{
					ReverseIterator rit = *this;
					++current;
					return rit;
				}

				ReverseIterator operator+(difference_type n) const
				{
					ReverseIterator rit = *this;
					current = current - n;
					return rit;
				}
				ReverseIterator operator-(difference_type n) const
				{
					ReverseIterator rit = *this;
					current = current + n;
					return rit;
				}

				ReverseIterator& operator+=(difference_type n)
				{
					current = current - n;
					return *this;
				}
				ReverseIterator& operator-=(difference_type n)
				{
					current = current + n;
					return *this;
				}
			
		};

		template <class I1, class I2>
		bool operator==(const ReverseIterator<I1>& lhs, const ReverseIterator<I2>& rhs)
		{
			return lhs.base() == rhs.base();
		}

		template <class I1, class I2>
		bool operator!=(const ReverseIterator<I1>& lhs, const ReverseIterator<I2>& rhs)
		{
			return lhs.base() != rhs.base();
		}

		template <class I1, class I2>
		bool operator>=(const ReverseIterator<I1>& lhs, const ReverseIterator<I2>& rhs)
		{
			return lhs.base() >= rhs.base();
		}

		template <class I1, class I2>
		bool operator<=(const ReverseIterator<I1>& lhs, const ReverseIterator<I2>& rhs)
		{
			return lhs.base() <= rhs.base();
		}

		template <class I1, class I2>
		bool operator>(const ReverseIterator<I1>& lhs, const ReverseIterator<I2>& rhs)
		{
			return lhs.base() > rhs.base();
		}

		template <class I1, class I2>
		bool operator<(const ReverseIterator<I1>& lhs, const ReverseIterator<I2>& rhs)
		{
			return lhs.base() < rhs.base();
		}
	}
}
#endif //Iterator.hpp
