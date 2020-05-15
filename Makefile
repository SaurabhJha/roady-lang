CXX=g++
CXXFLAGS=-std=c++11
LDFLAGS=-pthread
SOURCES=main.cc lexical_analyzer.cc parser_common.cc parser_top_down.cc parser_bottom_up.cc
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=executable

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CXX) $(CXXFLAGS) $< -o $@