# We had AI help to create the file to make sure it worked properly for the diffrent OS.

# Try to detect OS
UNAME_S := $(shell uname -s 2>/dev/null)

# Default Godot executable (user can override)
GODOT ?= godot

# Automatic detection
ifeq ($(UNAME_S),Darwin)
    PLATFORM := macos
    ARCH := arm64
    # If godot is not in PATH, try the default macOS app location
    ifeq ($(shell command -v $(GODOT) >/dev/null 2>&1; echo $$?),1)
        GODOT := /Applications/Godot.app/Contents/MacOS/Godot
    endif
else ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
    ARCH := x86_64
    # If "godot" not found, try godot4, godot-latest, etc
    ifeq ($(shell command -v $(GODOT) >/dev/null 2>&1; echo $$?),1)
        GODOT := $(shell command -v godot4 2>/dev/null || command -v godot.linuxbsd.editor 2>/dev/null || echo godot)
    endif
else ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    ARCH := x86_64
    # Escape spaces in paths
    GODOT := C:/Program\ Files/Godot/Godot.exe
endif

GODOT_CPP := godot-cpp
GEN := gen

.PHONY: all scons_libs run clean

all: scons_libs run

# Build Godot-cpp
scons_libs:
	cd $(GODOT_CPP) && scons platform=$(PLATFORM) arch=$(ARCH) target=template_release -j8

# Run Godot project
run:
	"$(GODOT)" --path demo --run

# Remove generated folders
clean:
	rm -rf gen bin
