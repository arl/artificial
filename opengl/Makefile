CC=gcc
CXX=g++
BASE_CFLAGS=-Iinclude -I$(GLFW_ROOT)/include

ifndef DEBUG
	CFLAGS=-Wall -ggdb -g3 -DDEBUG $(BASE_CFLAGS)
else
	CFLAGS=-Wall -ggdb -O3 $(BASE_CFLAGS)
endif
LDFLAGS=-L$(GLFW_ROOT)/lib -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lXinerama -lXxf86vm -lXcursor
GLFW_ROOT=/usr/local#  glfw install directory

# libartificial

obj/csrc_%.o: src/%.c
	$(call color,INFO,- compiling $@)
	@$(CC) $(CFLAGS) -o $@ -c $^
obj/cppsrc_%.o: src/%.cpp
	$(call color,INFO,- compiling $@)
	@$(CXX) $(CFLAGS) -o $@ -c $^

cpp_srcs := $(wildcard src/*.cpp)
c_srcs   := $(wildcard src/*.c)
cpp_objs := $(patsubst src/%.cpp, obj/cppsrc_%.o, $(cpp_srcs))
c_objs   := $(patsubst src/%.c, obj/csrc_%.o, $(c_srcs))

pre_build:
	@mkdir -p obj bin
.PHONY: pre_build

lib: pre_build bin/libartificial.a ## build libartificial.a static library
.PHONY: libartificial.a

bin/libartificial.a: $(c_objs) $(cpp_objs)
	$(call color,INFO,+ linking $@)
	@$(AR) rcs $@ $(c_objs) $(cpp_objs)

# tests

test_srcs := $(wildcard test/*.cpp)
test_objs := $(patsubst test/%.cpp, obj/test_%.o, $(test_srcs))

obj/test_%.o: test/%.cpp
	$(call color,INFO,- compiling $@)
	@$(CXX) $(BASE_CFLAGS) -L bin -o $@ -c $^

pre_test:
	@mkdir -p tmp
.PHONY: pre_test

test: pre_test lib bin/test ## build and run tests
	$(call color,INFO,'*' running tests...)
	@./bin/test --reporter compact
.PHONY: test

bin/test: $(test_objs)
	$(call color,INFO,+ linking $@)
	@$(CXX) -lm -o $@ $^ -Lbin -lartificial $(LDFLAGS)

# benchmarks

bench_srcs := $(wildcard benchmark/*.cpp)
bench_objs := $(patsubst benchmark/%.cpp, obj/bench_%.o, $(bench_srcs))

obj/bench_%.o: benchmark/%.cpp
	$(call color,INFO,- compiling $@)
	@$(CXX) $(BASE_CFLAGS) -o $@ -c $^

pre_bench: pre_test
.PHONY: pre_bench

bench: pre_bench lib bin/bench ## build and run benchmarks
	$(call color,INFO,'*' running benchmarks...)
	@./bin/bench
.PHONY: bench

bin/bench: $(bench_objs)
	$(call color,INFO,+ linking $@)
	@$(CXX) -lm -o $@ $^ -Lbin -lartificial $(LDFLAGS)

# misc

help:
	@grep -E '^[a-zA-Z_-.]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
.DEFAULT_GOAL := help

all: lib test bench ## build all, run tests and benchmarks

clean: ## clean temporary objects
	@rm -rf obj bin tmp

.PHONY: all clean

RED=9
GREEN=10
YELLOW=11
BLUE=12

ERROR=$(RED)
WARNING=$(YELLOW)
INFO=$(BLUE)
SUCCESS=$(GREEN)

define color
@tput setaf $($1)
@echo $2
@tput sgr0
endef
