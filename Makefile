CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -MMD -MP
BUILD = build
BIN = $(BUILD)/port_scanner
SRC = $(wildcard *.cpp) $(wildcard src/*.cpp)
OBJ = $(SRC:%.cpp=$(BUILD)/%.o)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJ:.o=.d)

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(BUILD)

.PHONY: run clean
