# -------------------------------------------------------------
# src/Makefile -- regex project build.
# -------------------------------------------------------------

# ------------------ Pretty output ----------------------------
V ?= 0
ifeq ($(V), 0)
	Q := @
else
	Q :=
endif

# Print helper
define PRINT
	@printf "%-5s %s\n" "$(1)" "$(2)"
endef

# ------------------ Prerequisite configuration ---------------
TARGET_EXEC := re
TARGET_TEST := re_test
LIB_NAME 		:= libre.a

# Directories
SRC_DIR := src
INC_DIR := include
TST_DIR := tests
BUI_DIR := build
BIN_DIR := bin

CORE_DIR := $(SRC_DIR)/re
APP_DIR  := $(SRC_DIR)/app

# Toolchain
CXX := g++
AR  := ar

UNAME_S := $(shell uname -s)
AR := ar

ifeq ($(UNAME_S),Linux)
    # Linux
    CC  := gcc
    CXX := g++
else ifeq ($(UNAME_S),Darwin)
    # macOS
    CC  := gcc-15
    CXX := g++-15
else
    # Other
    CC  := gcc
    CXX := g++
endif

# Include paths
#  -Iinclude     : core public headers
#  -Isrc/app     : app-private headers (e.g., src/app/cli.hpp)
CPPFLAGS := -I$(INC_DIR) -I$(SRC_DIR)/app

# Comlile flags
CXXFLAGS := -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion \
						-MMD -MP \
						-O0 -g \
						-std=c++20
LDFLAGS :=
ARFLAGS := rcs

# -------------------- Sources ------------------------
CORE_SRCS := $(wildcard $(CORE_DIR)/*.cpp)
APP_SRCS  := $(wildcard $(APP_DIR)/*.cpp)
TEST_SRCS := $(wildcard $(TST_DIR)/*.cpp)

# -------------------- Objects-------------------------
CORE_OBJS := $(CORE_SRCS:$(CORE_DIR)/%.cpp=$(BUI_DIR)/core/%.o)
APP_OBJS  := $(APP_SRCS:$(APP_DIR)/%.cpp=$(BUI_DIR)/app/%.o)
TEST_OBJS := $(TEST_SRCS:$(TST_DIR)/%.cpp=$(BUI_DIR)/tests/%.o)

# -------------------- Deps ---------------------------
DEPS := $(CORE_OBJS:.o=.d) $(APP_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# -------------------- Library ------------------------
LIB := $(BUI_DIR)/$(LIB_NAME)

# -------------------- Top-level targets ---------------
all: $(BIN_DIR)/$(TARGET_EXEC)

test: $(BIN_DIR)/$(TARGET_TEST)
	@./$(BIN_DIR)/$(TARGET_TEST)

clean:
	$(call PRINT,RM,$(BUI_DIR) $(BIN_DIR))
	$(Q)rm -rf $(BUI_DIR) $(BIN_DIR)

# -------------------- Directories ---------------------
$(BIN_DIR):
	@mkdir -p $@

$(BUI_DIR):
	@mkdir -p $@

$(BUI_DIR)/core:
	@mkdir -p $@

$(BUI_DIR)/app:
	@mkdir -p $@

$(BUI_DIR)/tests:
	@mkdir -p $@

# -------------------- Build core static library ---------
$(LIB): $(CORE_OBJS) | $(BUI_DIR)
	$(call PRINT,AR,$@)
	$(Q)$(AR) $(ARFLAGS) $@ $(CORE_OBJS)

# -------------------- Link executables ------------------
# App executable: app objs + core static library
$(BIN_DIR)/$(TARGET_EXEC): $(LIB) $(APP_OBJS) | $(BIN_DIR)
	$(call PRINT,LINK,$@)
	$(Q)$(CXX) $(APP_OBJS) $(LIB) -o $@ $(LDFLAGS)

# Test executable: tests objs (includes test_main.cpp main) + core static library
$(BIN_DIR)/$(TARGET_TEST): $(LIB) $(TEST_OBJS) | $(BIN_DIR)
	$(call PRINT,LINK,$@)
	$(Q)$(CXX) $(TEST_OBJS) $(LIB) -o $@ $(LDFLAGS)

# -------------------- Compile rules ---------------------
# core: src/re/*.cpp -> build/core/*.o
$(BUI_DIR)/core/%.o: $(CORE_DIR)/%.cpp | $(BUI_DIR)/core
	$(call PRINT,CXX,[core] $<)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# app: src/app/*.cpp -> build/app/*.o
$(BUI_DIR)/app/%.o: $(APP_DIR)/%.cpp | $(BUI_DIR)/app
	$(call PRINT,CXX,[app ] $<)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# tests: tests/*.cpp -> build/tests/*.o
$(BUI_DIR)/tests/%.o: $(TST_DIR)/%.cpp | $(BUI_DIR)/tests
	$(call PRINT,CXX,[test] $<)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: all test clean
