#ifndef HUMANCHARACTER_HPP
#define HUMANCHARACTER_HPP

#include "character.h"

using namespace godot;

class HumanCharacter : public Character {
    GDCLASS(HumanCharacter, Character);

protected:
    static void _bind_methods();

public:
    HumanCharacter();
    ~HumanCharacter();

    void assign_traits_randomly() override;
};

#endif
