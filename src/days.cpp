#include "days.h"
#include <godot_cpp/classes/random_number_generator.hpp>  


using namespace godot;

void DayManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("start_new_day", "parent"), &DayManager::start_new_day);
    ClassDB::bind_method(D_METHOD("spawn_random_character", "parent"), &DayManager::spawn_random_character);
    ClassDB::bind_method(D_METHOD("show_character_features", "character", "ui_panel"), &DayManager::show_character_features);
    ClassDB::bind_method(D_METHOD("end_day"), &DayManager::end_day);
    ClassDB::bind_method(D_METHOD("get_daily_characters"), &DayManager::get_daily_characters);
    ClassDB::bind_method(D_METHOD("show_character_feature", "character", "ui_panel", "feature_name"),&DayManager::show_character_feature
);


}

DayManager::DayManager() {
}

Character* DayManager::spawn_random_character(Node2D* parent) {
    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();
    bool is_zombie = rng->randi_range(0, 2) == 1;

    Character* character = nullptr;

    if (is_zombie) {
        character = memnew(ZombieCharacter());
    } else {
        character = memnew(HumanCharacter());
    }

    // Add child nodes BEFORE adding to tree
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


    // Add to world ONCE
    parent->add_child(character);

    // Hide the character in the world; we only use it as a data source
    character->set_visible(false);


    // Assign random traits
    character->assign_traits_randomly();

    // Do NOT append to daily_characters here — callers should do it once
    return character;
}


void DayManager::start_new_day(Node2D *parent) {
    // 1) Clear previous characters FIRST
    end_day();                 // calls queue_free() and clears daily_characters

    const int SPAWN_COUNT = 3;
    daily_characters.clear();


    for (int i = 0; i < SPAWN_COUNT; ++i) {
        Character *c = spawn_random_character(parent);
        if (c) {
            daily_characters.append(c);
        }
    }

    UtilityFunctions::print("DayManager: spawned ", SPAWN_COUNT, " visitors.");
}

void DayManager::show_character_feature(Character *c, Node *ui_panel, const String &feature_name) {
    if (!c || !ui_panel) {
        return;
    }

    // Clear previous feature sprites from the panel
    for (int i = ui_panel->get_child_count() - 1; i >= 0; i--) {
        Node *child = ui_panel->get_child(i);
        if (Object::cast_to<Sprite2D>(child)) {
            ui_panel->remove_child(child);
            child->queue_free();
        }
    }

    // Get the specific Sprite2D for this feature
    Sprite2D *sprite = c->get_node<Sprite2D>(feature_name);
    if (!sprite) {
        UtilityFunctions::push_warning("Feature sprite not found: " + feature_name);
        return;
    }

    // Duplicate and add just this one sprite
    Node *dup_base = sprite->duplicate();
    Sprite2D *dup = Object::cast_to<Sprite2D>(dup_base);
    if (dup) {
        ui_panel->add_child(dup);
    }
}


void DayManager::show_character_features(Character *c, Node *ui_panel) {
    if (!c || !ui_panel) {
        return;
    }

    // Clear any previous feature sprites from the panel
    for (int i = ui_panel->get_child_count() - 1; i >= 0; i--) {
        Node *child = ui_panel->get_child(i);
        if (Object::cast_to<Sprite2D>(child)) {
            ui_panel->remove_child(child);
            child->queue_free();
        }
    }

    Sprite2D *eyes  = c->get_node<Sprite2D>("Eyes");
    Sprite2D *hair  = c->get_node<Sprite2D>("Hair");
    Sprite2D *teeth = c->get_node<Sprite2D>("Teeth");
    Sprite2D *hands = c->get_node<Sprite2D>("Hands");
    Sprite2D *feet  = c->get_node<Sprite2D>("Feet");

    if (!eyes || !hair || !teeth || !hands || !feet) {
        UtilityFunctions::push_warning("Character sprites not set up correctly.");
        return;
    }

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

void DayManager::end_day() {
    for (int i = 0; i < daily_characters.size(); i++) {
        Character* c = Object::cast_to<Character>(daily_characters[i]);
        if (c) {
            c->queue_free();
        }
    }
    daily_characters.clear();
}

Array DayManager::get_daily_characters() const {
    return daily_characters;
}


void DayManager::remove_daily_character(Character* character) {
for (int i = 0; i < daily_characters.size(); ++i) {
    Character *c = Object::cast_to<Character>(daily_characters[i]);
    if (c == character) {
        daily_characters.remove_at(i);
        break;
    }
}
}
