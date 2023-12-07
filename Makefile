# Compiler
CXX = g++

# Flags
CXXFLAGS = -pthread -Wall -Wextra -Wpedantic -Werror -Wfatal-errors -Winit-self -Wshadow -Wlogical-op -Wcomment -Wctor-dtor-privacy -Wold-style-cast -D_GLIBCXX_DEBUG -fno-diagnostics-show-option
LDFLAGS = -lgps

# Targets
all: GPS WiPi

GPS: GPS.cpp
	$(CXX) GPS.cpp -o GPS $(LDFLAGS)

WiPi: Main.cpp wifi.cpp
	$(CXX) -o WiPi Main.cpp wifi.cpp $(CXXFLAGS)

clean:
	rm -f GPS WiPi

