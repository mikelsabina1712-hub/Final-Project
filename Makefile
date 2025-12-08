.PHONY: all clean

# Build the extension and open Godot
all:
	cd godot-cpp && scons
	godot --path demo --run


# Remove generated folders
clean:
	rm -rf gen bin
