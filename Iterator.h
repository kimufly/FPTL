#ifndef _ITERATOR_H
#define _ITERATOR_H

#include "Sources/Iterators/Iterator.hpp"
namespace ftd
{
	template <class Iter>
	using IteratorTraits = fl::iterators::IteratorTraits<Iter>;

	using InputIteratorTag = fl::iterators::InputIteratorTag;
	using OutputIteratorTag = fl::iterators::OutputIteratorTag;
	using BidirectionalIteratorTag = fl::iterators::BidirectionalIteratorTag;
	using RandomAccessIteratorTag = fl::iterators::RandomAccessIteratorTag;

	
	template <
		class Category,
		class T,
		class Distance = std::ptrdiff_t,
		class Pointer = T*,
		class Reference = T&
	>
	using Iterator = fl::iterators::Iterator< Category,
											  T,
											  Distance,
											  Pointer,
											  Reference >;
	template <class Iter>
	using ReverseIterator = fl::iterators::ReverseIterator<Iter>;
}


#endif
