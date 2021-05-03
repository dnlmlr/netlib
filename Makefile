NAME = netlib.a
TARGET = $(addprefix $(BUILD_DIR)/, $(NAME))


SRC_DIR = src
BUILD_DIR = build


SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(SRC:.cpp=.o)))


TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(addprefix $(BUILD_DIR)/, $(notdir $(TEST_SRC:.cpp=.o)))


LD_FLAGS = -g -Iinc
COMPILE_FLAGS = -g -c -O3 -Wall -Iinc


$(TARGET): $(OBJ)
	ar rcs $@ $(OBJ)


# Build rule for normal source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(COMPILE_FLAGS) -o $@ $<


$(TEST_OBJ): $(TEST_SRC)
	g++ $(COMPILE_FLAGS) -c -I$(SRC_DIR) -o $@ $<


test: $(TEST_OBJ) $(TARGET)
	g++ $(LD_FLAGS) -o $(BUILD_DIR)/test.run $(TEST_OBJ) $(TARGET)
	$(BUILD_DIR)/test.run
#   lcov -d $(BUILD_DIR) -c -o lcov.info

example: $(TARGET)
	g++ $(LD_FLAGS) example/example.cpp $(TARGET) -o build/example.run
	./build/example.run

.PHONY: clean
clean:
	rm -f $(OBJ) $(TARGET) $(TEST_OBJ) $(BUILD_DIR)/test.run $(BUILD_DIR)/example.run
