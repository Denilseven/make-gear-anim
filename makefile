COMPILER = g++
SOURCES = $(shell find src -name '*.cxx')
OBJECTS = $(patsubst %.cxx,build/%.o,$(SOURCES))
FLAGS = -Isrc/header -lraylib -lX11
OUTPUT = game.out

.PHONY: default build run clean

default: build

build: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(COMPILER) $(OBJECTS) -o $@ $(FLAGS)

build/%.o: %.cxx
	mkdir -p $(dir $@)
	$(COMPILER) $(FLAGS) -c $< -o $@

run: build
	./$(OUTPUT)

clean:
	rm -rf $(OUTPUT) build/
