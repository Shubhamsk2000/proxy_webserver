CXX = g++
CXXFLAGS = -std=c++20 -Wall -pthread -Iinclude

SRC = src/main.cpp src/server.cpp src/client_handler.cpp src/http_parser.cpp src/proxy.cpp src/lru_cache.cpp
OBJ = $(SRC:src/%.cpp=build/%.o)
BIN = build/proxy

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJ)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build


# runn main.cpp file
run:
	./build/proxy

clean:
	rm -rf build
