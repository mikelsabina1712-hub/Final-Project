#ifndef HUMANCHARACTER_HPP
#define HUMANCHARACTER_HPP

#include "character.h"

using namespace godot;

// A regular human character - not a zombie
class HumanCharacter : public Character {
    GDCLASS(HumanCharacter, Character);

protected:
    static void _bind_methods();

public:
    HumanCharacter();
    ~HumanCharacter();
    
    // Give this human a random appearance
    void assign_traits_randomly() override;
};

#endif