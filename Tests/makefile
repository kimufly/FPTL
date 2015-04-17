
TARGET = test_list

CXX = clang++
CXXFLAGS = -std=c++11 -ggdb -pthread -Wall -Wextra -pedantic -fno-elide-constructors

all : $(TARGET)

$(TARGET) : test_list.cpp ../Sources/Containers/List/List.hpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) test_list.cpp

clean:
	rm -rf *.o *~ core $(TARGET) *dSYM
