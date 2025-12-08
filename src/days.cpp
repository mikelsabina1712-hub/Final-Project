#include "days.h"
#include <godot_cpp/classes/random_number_generator.hpp>  

using namespace godot;

// Hook up methods so GDScript can call them
void DayManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start_new_day", "parent"), &DayManager::start_new_day);
    ClassDB::bind_method(D_METHOD("spawn_random_character", "parent"), &DayManager::spawn_random_character);
    ClassDB::bind_method(D_METHOD("show_character_features", "character", "ui_panel"), &DayManager::show_character_features);
    ClassDB::bind_method(D_METHOD("end_day"), &DayManager::end_day);
    ClassDB::bind_method(D_METHOD("get_daily_characters"), &DayManager::get_daily_characters);
    ClassDB::bind_method(D_METHOD("show_character_feature", "character", "ui_panel", "feature_name"), &DayManager::show_character_feature);
}

// Constructor
DayManager::DayManager() {
}

// Factory function - creates either a human or zombie character
Character* DayManager::spawn_random_character(Node2D* parent) {
    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();
    
    // 33% chance to be a zombie (0, 1, or 2 - only 1 is zombie)
    bool is_zombie = rng->randi_range(0, 2) == 1;

    Character* character = nullptr;

    // Create the right type
    if (is_zombie) {
        character = memnew(ZombieCharacter());
    } else {
        character = memnew(HumanCharacter());
    }

    // Create all the body part sprites BEFORE adding to the scene tree
    auto make_sprite = [&](const String& name) {
        Sprite2D* sprite = memnew(Sprite2D);
        sprite->set_name(name);
        character->add_child(sprite);
    };

    make_sprite("Eyes");
    make_sprite("Hair");
    make_sprite("Teeth");
    make_sprite("Hands");
    make_sprite("Feet");
    make_sprite("Portrait");

    // Add the character to the world
    parent->add_child(character);

    // Hide them - we only use this as a data container for textures
    character->set_visible(false);

    // Give them random features
    character->assign_traits_randomly();

    return character;
}

// Start a new day by spawning fresh visitors
void DayManager::start_new_day(Node2D *parent) {
    // Clean up yesterday's visitors first
    end_day();

    const int SPAWN_COUNT = 3;
    daily_characters.clear();

    // Spawn the new batch
    for (int i = 0; i < SPAWN_COUNT; ++i) {
        Character *c = spawn_random_character(parent);
        if (c) {
            daily_characters.append(c);
        }
    }

    UtilityFunctions::print("DayManager: spawned ", SPAWN_COUNT, " visitors.");
}

// Show just one specific body part in the UI panel
void DayManager::show_character_feature(Character *c, Node *ui_panel, const String &feature_name) {
    if (!c || !ui_panel) {
        return;
    }

    // Clear out any old sprites from the panel
    for (int i = ui_panel->get_child_count() - 1; i >= 0; i--) {
        Node *child = ui_panel->get_child(i);
        if (Object::cast_to<Sprite2D>(child)) {
            ui_panel->remove_child(child);
            child->queue_free();
        }
    }

    // Get the sprite we want to show
    Sprite2D *sprite = c->get_node<Sprite2D>(feature_name);
    if (!sprite) {
        UtilityFunctions::push_warning("Feature sprite not found: " + feature_name);
        return;
    }

    // Duplicate it and add to the panel
    Node *dup_base = sprite->duplicate();
    Sprite2D *dup = Object::cast_to<Sprite2D>(dup_base);
    if (dup) {
        ui_panel->add_child(dup);
    }
}

// Show all body parts in the UI panel (probably not used but kept for reference)
void DayManager::show_character_features(Character *c, Node *ui_panel) {
    if (!c || !ui_panel) {
        return;
    }

    // Clear the panel
    for (int i = ui_panel->get_child_count() - 1; i >= 0; i--) {
        Node *child = ui_panel->get_child(i);
        if (Object::cast_to<Sprite2D>(child)) {
            ui_panel->remove_child(child);
            child->queue_free();
        }
    }

    // Get all the body part sprites
    Sprite2D *eyes  = c->get_node<Sprite2D>("Eyes");
    Sprite2D *hair  = c->get_node<Sprite2D>("Hair");
    Sprite2D *teeth = c->get_node<Sprite2D>("Teeth");
    Sprite2D *hands = c->get_node<Sprite2D>("Hands");
    Sprite2D *feet  = c->get_node<Sprite2D>("Feet");

    if (!eyes || !hair || !teeth || !hands || !feet) {
        UtilityFunctions::push_warning("Character sprites not set up correctly.");
        return;
    }

    // Helper to duplicate and add a sprite
    auto add_copy = [&](Sprite2D *src) {
        if (!src) return;
        Node *dup_base = src->duplicate();
        Sprite2D *dup = Object::cast_to<Sprite2D>(dup_base);
        if (dup) {
            ui_panel->add_child(dup);
        }
    };

    add_copy(eyes);
    add_copy(hair);
    add_copy(teeth);
    add_copy(hands);
    add_copy(feet);
}

// Clean up all today's visitors
void DayManager::end_day() {
    for (int i = 0; i < daily_characters.size(); i++) {
        Character* c = Object::cast_to<Character>(daily_characters[i]);
        if (c) {
            c->queue_free();
        }
    }
    daily_characters.clear();
}

// Get the list of today's visitors
Array DayManager::get_daily_characters() const {
    return daily_characters;
}

// Remove a character from the list (when they're accepted or rejected)
void DayManager::remove_daily_character(Character* character) {
    for (int i = 0; i < daily_characters.size(); ++i) {
        Character *c = Object::cast_to<Character>(daily_characters[i]);
        if (c == character) {
            daily_characters.remove_at(i);
            break;
        }
    }
}