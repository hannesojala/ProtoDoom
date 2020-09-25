CXX       := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb -O3

BIN     := bin
SRC     := src
INCLUDE := include
LIB     := lib
LIBRARIES   := -lSDL2
EXECUTABLE  := Caster


all: $(BIN)/$(EXECUTABLE)

run: clean all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*

