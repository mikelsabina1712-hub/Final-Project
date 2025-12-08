# =========================================================
# Minimal cross-platform Makefile for Godot 4.x GDExtension
# =========================================================

# Detect operating system
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    PLATFORM := macos
    ARCH := arm64      # change to x86_64 if using Intel
    GODOT := /Applications/Godot.app/Contents/MacOS/Godot
else ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
    ARCH := x86_64
    GODOT := /usr/bin/godot
else ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    ARCH := x86_64
    GODOT := "C:/Program Files/Godot/Godot.exe"
endif

GODOT_CPP := godot-cpp
GEN := gen

.PHONY: all scons_libs run clean

all: scons_libs run

# compile godot-cpp and generate bindings
scons_libs:
	cd $(GODOT_CPP) && scons platform=$(PLATFORM) arch=$(ARCH) target=template_release -j8
	cd $(GODOT_CPP) && python3 binding_generator.py --output ../$(GEN)

# run Godot editor with demo project
run:
	$(GODOT) --path demo --editor

# clean generated files
clean:
	rm -rf gen bin
