#include "zombie.h"

using namespace godot;

// Bind methods to Godot - zombies inherit everything from Character
void ZombieCharacter::_bind_methods() {
    UtilityFunctions::print("ZOMBIE::_bind_methods called");
    // Nothing to add, Character class handles it all
}

// Constructor - mark this character as a zombie
ZombieCharacter::ZombieCharacter() {
    is_zombie = true;
}

// Destructor
ZombieCharacter::~ZombieCharacter() {
}

// Give this zombie random features
void ZombieCharacter::assign_traits_randomly() {
    randomize_features();
}