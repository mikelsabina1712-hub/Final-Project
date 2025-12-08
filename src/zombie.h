#ifndef ZOMBIECHARACTER_HPP
#define ZOMBIECHARACTER_HPP

#include "character.h"

using namespace godot;

// A zombie character - looks like a human but is dangerous
class ZombieCharacter : public Character {
    GDCLASS(ZombieCharacter, Character);

protected:
    static void _bind_methods();

public:
    ZombieCharacter();
    ~ZombieCharacter();
    
    // Give this zombie a random appearance
    void assign_traits_randomly() override;
};

#endif