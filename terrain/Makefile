CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall
LDFLAGS = -lGL -lGLEW -lglfw -lpthread -ldl -lm

SOURCES = terrain.cpp noise.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = terrain

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all clean run
