#ifndef ZOMBIECHARACTER_HPP
#define ZOMBIECHARACTER_HPP

#include "character.h"

using namespace godot;

class ZombieCharacter : public Character {
    GDCLASS(ZombieCharacter, Character);

protected:
    static void _bind_methods();

public:
    ZombieCharacter();
    ~ZombieCharacter();

    void assign_traits_randomly() override;
};

#endif
