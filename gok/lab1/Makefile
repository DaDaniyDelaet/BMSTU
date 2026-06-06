PLUGIN_NAME := gimple_lab1_dump
PLUGIN_SO   := $(PLUGIN_NAME).so
SRC         := gimple_lab1_plugin.cpp

GCC := gcc-15
CXX := g++-15

PLUGIN_INCLUDE := $(shell $(GCC) -print-file-name=plugin/include)
BREW_PREFIX := /opt/homebrew

CXXFLAGS := -O2 -fPIC -shared -Wall -Wextra -std=gnu++17 -fno-rtti -fno-exceptions
INCLUDES := -I"$(PLUGIN_INCLUDE)" -I"$(BREW_PREFIX)/include"
LDFLAGS := -L"$(BREW_PREFIX)/lib" -Wl,-undefined,dynamic_lookup

all: $(PLUGIN_SO)

$(PLUGIN_SO): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o "$@" "$<"

test: $(PLUGIN_SO)
	$(GCC) -O0 -fplugin=./$(PLUGIN_SO) -c test.c -o test.o

clean:
	rm -f "$(PLUGIN_SO)" test test.o

.PHONY: all test clean
