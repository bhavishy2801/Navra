CXX=g++
CXXFLAGS=-std=c++17 -Wall -Iinclude
TARGET=optimizer.exe
SRCDIR=src
OBJDIR=obj
SOURCES=$(wildcard $(SRCDIR)/*.cpp) main.cpp
OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))

all: directories $(TARGET)

directories:
	mkdir -p $(OBJDIR)/src

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run directories
