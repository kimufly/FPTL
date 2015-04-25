#include <iostream>
#include <type_traits>
#include "../List.h"

int main()
{
	std::cout << "Testing List constructors. \n";
	ftd::List<int> lst1(3, 5);
	ftd::List<int> lst2{1,2,3};
	ftd::List<int> lst3(lst2.begin(), lst2.end());

	
	ftd::List<int>::iterator it = lst1.begin();
	ftd::List<int>::const_iterator cit = lst1.cbegin();
	ftd::List<int>::reverse_iterator rit = lst1.rbegin();
	ftd::List<int>::const_reverse_iterator rcit = lst1.rcbegin();

	std::cout << "Testing iterator for lst1:\n";
	for (; it != lst1.end(); ++it)
	{
		std::cout << *it << std::endl;
	}

	std::cout << "Testing const iterator for lst1:\n";
	for (; cit != lst1.cend(); ++cit)
	{
		std::cout << *cit << std::endl;
	}

	std::cout << "Testing reverse iterator for lst1: \n";
	for (; rit != lst1.rend(); ++rit)
	{
		std::cout << *rit << std::endl;
	}
	std::cout << "Testing const reverse iterator for lst1: \n";
	for (; rcit != lst1.rcend(); ++rcit)
	{
		std::cout << *rcit << std::endl;
	}

	it = lst2.begin();
	cit = lst2.cbegin();
	rit = lst2.rbegin();
	rcit = lst2.rcbegin();
	
	std::cout << "Testing iterator for lst2:\n";
	for (; it != lst2.end(); ++it)
	{
		std::cout << *it << std::endl;
	}

	std::cout << "Testing const_iterator for lst2:\n";
	for (; cit != lst2.cend(); ++cit)
	{
		std::cout << *cit << std::endl;
	}

	std::cout << "Testing reverse_iterator for lst2:\n";
	for (; rit != lst2.rend(); ++rit)
	{
		std::cout << *rit << std::endl;
	}

	std::cout << "Testing const_reverse_iterator for lst2:\n";
	for (; rcit != lst2.rcend(); ++rcit)
	{
		std::cout << *rcit << std::endl;
	}
	
	return 0;
}
