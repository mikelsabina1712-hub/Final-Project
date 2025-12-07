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

class Character : public Node2D {
    GDCLASS(Character, Node2D);

protected:
    static void _bind_methods();

    bool is_zombie = false;

    // Arrays of textures (set from the editor)
    Array zombie_eyes;
    Array zombie_hair;
    Array zombie_teeth;
    Array zombie_hands;
    Array zombie_feet;
    Array zombie_portrait;


    Array human_eyes;
    Array human_hair;
    Array human_teeth;
    Array human_hands;
    Array human_feet;
    Array human_portrait;


    // Sprites in the scene
    Sprite2D *eyes_sprite = nullptr;
    Sprite2D *hair_sprite = nullptr;
    Sprite2D *teeth_sprite = nullptr;
    Sprite2D *hands_sprite = nullptr;
    Sprite2D *feet_sprite = nullptr;
    Sprite2D *portrait_sprite = nullptr;



    Array load_textures_from_dir(const godot::String &path);
    void ensure_feature_textures_loaded();


public:
    Character();

    void _enter_tree() override;
    void _ready() override;

    // Zombie flag
    void set_is_zombie(bool value);
    bool get_is_zombie() const;

    // Setters
    void set_zombie_eyes(const Array &arr);
    void set_zombie_hair(const Array &arr);
    void set_zombie_teeth(const Array &arr);
    void set_zombie_hands(const Array &arr);
    void set_zombie_feet(const Array &arr);

    void set_human_eyes(const Array &arr);
    void set_human_hair(const Array &arr);
    void set_human_teeth(const Array &arr);
    void set_human_hands(const Array &arr);
    void set_human_feet(const Array &arr);

    // Getters (needed by Godot for properties)
    Array get_zombie_eyes() const;
    Array get_zombie_hair() const;
    Array get_zombie_teeth() const;
    Array get_zombie_hands() const;
    Array get_zombie_feet() const;

    Array get_human_eyes() const;
    Array get_human_hair() const;
    Array get_human_teeth() const;
    Array get_human_hands() const;
    Array get_human_feet() const;

    // Texture helper
    Ref<Texture2D> get_random_texture(const Array &arr);


    // Randomizer
    void randomize_features();

    // Must be implemented by HumanCharacter / ZombieCharacter
    virtual void assign_traits_randomly() = 0;
};

#endif
