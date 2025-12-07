#ifndef DAYMANAGER_HPP
#define DAYMANAGER_HPP

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include "character.h"
#include "zombie.h"
#include "human.h"

using namespace godot;

class DayManager : public Node {
    GDCLASS(DayManager, Node);

protected:
    static void _bind_methods();


public:
    Array daily_characters; // stores the 3 characters

    DayManager();

    void show_character_feature(Character *c, Node *ui_panel, const String &feature_name);
    void start_new_day(Node2D* parent);
    Character* spawn_random_character(Node2D* parent);
    void show_character_features(Character *c, Node *ui_panel);
    void end_day();
    Array get_daily_characters() const;
    void remove_daily_character(Character *c);


};

#endif
