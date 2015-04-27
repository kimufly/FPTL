
TARGET = test_list

CXX = g++-4.9
CXXFLAGS = -std=c++14 -ggdb -pthread -Wall -Wextra -pedantic -fno-elide-constructors

all : $(TARGET)

$(TARGET) : test_list.cpp ../Sources/Containers/List/List.hpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) test_list.cpp

clean:
	rm -rf *.o *~ core $(TARGET) *dSYM
