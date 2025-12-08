#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// Base class for both humans and zombies - handles appearance and traits
class Character : public Node2D {
    GDCLASS(Character, Node2D);

protected:
    static void _bind_methods();
    
    // Whether this character is a zombie or not
    bool is_zombie = false;
    
    // Texture arrays for zombie body parts
    // These get loaded from res://art/zombies/
    Array zombie_eyes;
    Array zombie_hair;
    Array zombie_teeth;
    Array zombie_hands;
    Array zombie_feet;
    Array zombie_portrait;
    
    // Texture arrays for human body parts
    // These get loaded from res://art/humans/
    Array human_eyes;
    Array human_hair;
    Array human_teeth;
    Array human_hands;
    Array human_feet;
    Array human_portrait;
    
    // The actual sprite nodes that display the textures in the scene
    Sprite2D *eyes_sprite = nullptr;
    Sprite2D *hair_sprite = nullptr;
    Sprite2D *teeth_sprite = nullptr;
    Sprite2D *hands_sprite = nullptr;
    Sprite2D *feet_sprite = nullptr;
    Sprite2D *portrait_sprite = nullptr;
    
    // Helper to load all textures from a folder
    Array load_textures_from_dir(const godot::String &path);
    
    // Make sure all texture arrays are filled from art folders
    void ensure_feature_textures_loaded();

public:
    Character();
    void _enter_tree() override;
    void _ready() override;
    
    // Check if this is a zombie
    void set_is_zombie(bool value);
    bool get_is_zombie() const;
    
    // Setters for zombie texture arrays (used by Godot editor)
    void set_zombie_eyes(const Array &arr);
    void set_zombie_hair(const Array &arr);
    void set_zombie_teeth(const Array &arr);
    void set_zombie_hands(const Array &arr);
    void set_zombie_feet(const Array &arr);
    
    // Setters for human texture arrays
    void set_human_eyes(const Array &arr);
    void set_human_hair(const Array &arr);
    void set_human_teeth(const Array &arr);
    void set_human_hands(const Array &arr);
    void set_human_feet(const Array &arr);
    
    // Getters for zombie texture arrays
    Array get_zombie_eyes() const;
    Array get_zombie_hair() const;
    Array get_zombie_teeth() const;
    Array get_zombie_hands() const;
    Array get_zombie_feet() const;
    
    // Getters for human texture arrays
    Array get_human_eyes() const;
    Array get_human_hair() const;
    Array get_human_teeth() const;
    Array get_human_hands() const;
    Array get_human_feet() const;
    
    // Pick a random texture from an array
    Ref<Texture2D> get_random_texture(const Array &arr);
    
    // Give this character random features based on their type
    void randomize_features();
    
    // Subclasses (Human/Zombie) must implement this
    virtual void assign_traits_randomly() = 0;
};

#endif