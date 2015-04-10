#ifndef _ITERATORS_HPP
#define _ITERATORS_HPP

#include <cstddef>

namespace fl
{
	namespace iterator
	{
		template <bool flag, class T, class F> struct choose;
		template <class T, class F> struct choose<true, T, F>
		{
			using type = T;
		};
		template <class T, class F> struct choose<false, T, F>
		{
			using type = F;
		};

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
			pointer current;

			ReverseIterator() {}
			explicit ReverseIterator( Iter x );
			template <class U>
			ReverseIterator(const ReverseIterator<U>& other);

			ReverseIterator& operator++();
			ReverseIterator& operator--();

			ReverseIterator operator++(int);
			ReverseIterator operator--(int);

			ReverseIterator operator+(difference_type n) const;
			ReverseIterator operator-(difference_type n) const;

			ReverseIterator& operator+=(difference_type n);
			ReverseIterator& operator-=(difference_type n);
			
		};

	}
}
#endif //Iterators.hpp
