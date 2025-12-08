#include "human.h"

using namespace godot;

// Bind methods to Godot - humans inherit everything from Character
void HumanCharacter::_bind_methods() {
    UtilityFunctions::print("HUMAN::_bind_methods called");
    // Nothing to add, Character class handles it all
}

// Constructor - mark this character as NOT a zombie
HumanCharacter::HumanCharacter() {
    is_zombie = false;
}

// Destructor
HumanCharacter::~HumanCharacter() {
}

// Give this human random features
void HumanCharacter::assign_traits_randomly() {
    randomize_features();
}