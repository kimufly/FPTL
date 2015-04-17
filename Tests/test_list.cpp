#include <iostream>
#include <type_traits>
#include "../List.h"

int main()
{
	ftd::List<int> lst(4, 5);
	ftd::List<int>::iterator it = lst.begin();
	ftd::List<int>::const_iterator cit = lst.cbegin();
	ftd::List<int>::reverse_iterator rit = lst.rbegin();
	ftd::List<int>::const_reverse_iterator rcit = lst.rcbegin();

	std::cout << "Testing iterator:\n";
	for (; it != lst.end(); ++it)
	{
		std::cout << *it << std::endl;
	}

	std::cout << "Testing const iterator:\n";
	for (; cit != lst.cend(); ++cit)
	{
		std::cout << *cit << std::endl;
	}

	std::cout << "Testing reverse iterator: \n";
	for (; rit != lst.rend(); ++rit)
	{
		std::cout << *rit << std::endl;
	}

	std::cout << "Testing const reverse iterator: \n";
	for (; rcit != lst.rcend(); ++rcit)
	{
		std::cout << *rcit << std::endl;
	}
	
	
	
	return 0;
}
