# C++ Makefile for HW5
#
# Replace the LastName-FirstName in the package section with
# your actual first and last name.

CXX=g++
CXXFLAGS=-Wall -Wextra -Wpedantic -Werror -Wfatal-errors -Winit-self -Wshadow -Wlogical-op -Wcomment -Wctor-dtor-privacy -Wold-style-cast -D_GLIBCXX_DEBUG -fno-diagnostics-show-option

all: GPS.cc 
	$(CXX) $(CXXFLAGS) -o GPS GPS.cc -lgps
		
clean:
	rm -f GPS *.o *.zip
	
package:
	zip GPS.zip README.txt Makefile GPS.cc
