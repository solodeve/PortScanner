CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra
BIN = port_scanner
SRC = main.cpp PortScanner.cpp Cli.cpp

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: run clean
