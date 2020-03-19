CXX=g++
CXXFLAGS=-std=c++11
LDFLAGS=-pthread
SOURCES=main.cc lexical_analyzer.cc parser.cc
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=executable

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CXX) $(CXXFLAGS) $< -o $@