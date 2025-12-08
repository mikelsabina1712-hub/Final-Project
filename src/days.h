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

// Manages daily visitors - spawning them and cleaning them up
class DayManager : public Node {
    GDCLASS(DayManager, Node);

protected:
    static void _bind_methods();

public:
    // Array of the 3 visitors who showed up today
    Array daily_characters;
    
    DayManager();
    
    // Show one specific body part (eyes, hands, etc.) in the UI panel
    void show_character_feature(Character *c, Node *ui_panel, const String &feature_name);
    
    // Spawn a fresh batch of visitors for the new day
    void start_new_day(Node2D* parent);
    
    // Create a single random character (either human or zombie)
    Character* spawn_random_character(Node2D* parent);
    
    // Show all body parts of a character in the UI panel
    void show_character_features(Character *c, Node *ui_panel);
    
    // Clean up all of today's visitors
    void end_day();
    
    // Get the list of today's visitors
    Array get_daily_characters() const;
    
    // Remove a character from today's list (when they're accepted/rejected)
    void remove_daily_character(Character *c);
};

#endif