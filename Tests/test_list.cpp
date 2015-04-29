#include <iostream>
#include <type_traits>
#include "../List.h"

int main()
{
	// Testing constructors
	std::cout << "Testing List constructors. \n";
	std::cout << "Initialize lst1 with three 5's.\n";
	ftd::List<int> lst1(3, 5);
	std::cout << "Initialize lst2 with Initializer_list {1,2,3}\n";
	ftd::List<int> lst2{1,2,3};
	std::cout << "Initialize lst3 with copy of lst2\n";
	ftd::List<int> lst3(lst2.begin(), lst2.end());
	std::cout << "Initialize lst4 with copy constructor from lst3\n";
	ftd::List<int> lst4(lst3);

	// Testing Iterators

	std::cout << "Testing iterators.\n";
	
	ftd::List<int>::iterator it = lst2.begin();
	ftd::List<int>::const_iterator cit = lst2.cbegin();
	ftd::List<int>::reverse_iterator rit = lst2.rbegin();
	ftd::List<int>::const_reverse_iterator rcit = lst2.rcbegin();

	std::cout << "Iterator: \n";
	for (; it != lst2.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;

	std::cout << "Const_iterator: \n";
	for (; cit != lst2.cend(); ++cit)
		std::cout << *cit << " ";
	std::cout << std::endl;

	std::cout << "Reverse_iterator: \n";
	for (; rit != lst2.rend(); ++rit)
		std::cout << *rit << " ";
	std::cout << std::endl;

	std::cout << "Const_reverse_iterator: \n";
	for (; rcit != lst2.rcend(); ++rcit)
		std::cout << *rcit << " ";
	std::cout << std::endl;

	std::cout << "Testing insertion: \n";
	std::cout << "Single insert: \n";
	lst2.insert(lst2.end(), 4);
	lst2.insert(lst2.end(), 5);
	it = lst2.begin();
	for (; it != lst2.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;

	std::cout << "Copy insert: \n";
	lst2.insert(lst2.cend(), 5, 6);
	it = lst2.begin();
	for (; it != lst2.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;

	std::cout << "Testing emplacement:\n";
	ftd::List<char> lst5;
	lst5.emplace_back('a');
	lst5.emplace_back('b');
	lst5.emplace_back('c');
	lst5.emplace_front('d');
	lst5.emplace_front('e');
	lst5.emplace_front('3');
	auto pos = lst5.cbegin();
	++pos;
	lst5.emplace(pos, '4');
	for (auto &e: lst5)
		std::cout << e << " ";
	std::cout << std::endl;

	std::cout << "Testing assignment:\n";
	std::cout << "lst4 = lst2\n";
	lst4 = lst2;
	it = lst4.begin();
	std::cout << "lst4: \n";
	for (; it != lst4.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;

	std::cout << "Testing erase functions: \n";
	cit = lst4.cend();
	--cit;
	lst4.erase(cit);
	std::cout << "Last element deleted.\n";

	for (it = lst4.begin(); it != lst4.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl << std::endl;

	auto itbegin = lst4.cbegin();
	auto itend = lst4.cend();

	++itbegin;
	--itend;

	lst4.erase(itbegin, itend);

	for (it = lst4.begin(); it != lst4.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;

/*
	std::cout << "Creating lst6 with {1,2,3,4,5,6,7,8,9,0}.\n";
	ftd::List<int> lst6{1,2,3,4,5,6,7,8,9,0};
	for (auto &e : lst6)
	{
		std::cout << e << " ";
	}
	std::cout << std::endl;

	std::cout << "Testing pop front and pop back:\n";
	lst5.pop_front();
	lst5.pop_back();
	for (auto &e : lst6)
		std::cout << e << " ";
	std::cout << std::endl;

	std::cout << "Testing clear function\n";
	lst5.clear();
	if (lst5.empty())
		std::cout << "List cleared.\n";
	for (auto &e : lst6)
		std::cout << e << " ";
	std::cout << std::endl;

	std::cout << "Testing push back and push front: \n";
	std::cout << "Pushing 5 at the back\n";
	lst6.push_back(5);
	std::cout << "Pushing 6 at the back\n";
	lst6.push_back(6);
	std::cout << "Pushing 1 at the front\n";
	lst6.push_front(1);
	std::cout << "Pushing 12 at the front\n";
	lst6.push_front(12);

	for (auto &e : lst6)
		std::cout << e << " ";
	std::cout <<std::endl;
	
	std::cout << "Testing size: \n";
	lst6 = {2,2,2};
	std::cout << "Inserting 100 elements of 2.\n";
	lst6.insert(lst6.cbegin(), 97, 2);
	std::cout << "lst6 has " << lst6.size() << " elements of value " << *lst6.begin() << std::endl;

	std::cout << "Testing resize:\n";
	std::cout << "Size of lst6 after resize: \n";
	lst6.resize(5);
	std::cout << lst6.size() << std::endl;
	for (auto &e : lst6)
		std::cout << e << " ";
	std::cout << std::endl;
	lst6.resize(10);
	std::cout << "Size of lst6 after resize: \n";
	std::cout << lst6.size() << std::endl;
	for (auto &e : lst6)
		std::cout << e << " ";
	std::cout << std::endl;
	lst6.resize(20, 5);
	std::cout << "Size of lst6 after resize: \n";
	std::cout << lst6.size() << std::endl;
	for (auto &e : lst6)
		std::cout << e << " ";
	std::cout << std::endl;

	std::cout << "Testing max size: \n";
	std::cout << lst6.max_size() << std::endl;

	std::cout << "Testing operators\n";
	ftd::List<char> lst7{'a', 'b', 'c', 'd'};
	ftd::List<char> lst8{'z', 'y', 'r', 'x'};

	ftd::List<char> lst9{'a', 'b', 'c', 'd'};
	ftd::List<char> lst10{'a', 'b', 'f', 'd'};

	bool less1 = lst7 < lst8;
	bool less2 = lst9 < lst10;
	bool equal = lst7 == lst9;
	bool greater1 = lst8 > lst7;
	bool greater2 = lst10 > lst9;
	bool lessequal = lst7 <= lst9;
	bool greaterequal = lst7 >= lst9;

	std::cout << "lst7: ";
	for (auto &e : lst7)
		std::cout << e << " ";
	std::cout << std::endl;
	std::cout << "lst8: ";
	for (auto &e : lst8)
		std::cout << e << " ";
	std::cout << std::endl;
	std::cout << "lst9: ";
	for (auto &e : lst9)
		std::cout << e << " ";
	std::cout << std::endl;
	std::cout << "lst10: ";
	for (auto &e : lst10)
		std::cout << e << " ";
	std::cout << std::endl;

	std::cout << "lst7 < lst8? " << std::boolalpha << less1 << std::endl;
	std::cout << "lst8 > lst7? " << std::boolalpha << greater1 << std::endl;
	std::cout << "lst7 == lst9? " << std::boolalpha << equal << std::endl;
	std::cout << "lst7 <= lst9? " << std::boolalpha << lessequal << std::endl;
	std::cout << "lst7 >= lst9? " << std::boolalpha << greaterequal << std::endl;
	std::cout << "lst9 < lst10? " << std::boolalpha << less2 << std::endl;
	std::cout << "lst10 > lst9? " << std::boolalpha << greater2 << std::endl;
	*/
	return 0;
}
