# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -I.
LDFLAGS := 
DEBUG_FLAGS := -g -O0
RELEASE_FLAGS := -O3

# Directories
SRC_DIR := lib
INCLUDE_DIR := include
BUILD_DIR := build
BIN_DIR := bin

# Files
MAIN_SRC := main.cpp
MAIN_OBJ := $(BUILD_DIR)/$(MAIN_SRC:.cpp=.o)
MAIN_TARGET := $(BIN_DIR)/main

# Find all .cpp files in lib directory
LIB_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
LIB_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SRCS))

# Dependencies (header files)
DEPS := $(wildcard $(INCLUDE_DIR)/*.h)

# Determine build type (debug or release)
ifeq ($(BUILD),release)
    CXXFLAGS += $(RELEASE_FLAGS)
else
    CXXFLAGS += $(DEBUG_FLAGS)
    BUILD := debug
endif

.PHONY: all clean release

all: $(MAIN_TARGET)

release:
	@$(MAKE) BUILD=release

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(DEPS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_TARGET): $(LIB_OBJS) $(MAIN_OBJ)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)