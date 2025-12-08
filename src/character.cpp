#include "character.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/random_number_generator.hpp> 
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/dir_access.hpp>

using namespace godot;

// Bind all methods and properties so Godot can use them
void Character::_bind_methods() {
    UtilityFunctions::print("Character::_bind_methods called");

    // is_zombie flag
    ClassDB::bind_method(D_METHOD("set_is_zombie", "value"), &Character::set_is_zombie);
    ClassDB::bind_method(D_METHOD("get_is_zombie"), &Character::get_is_zombie);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_zombie"), "set_is_zombie", "get_is_zombie");

    // Bind setters/getters for zombie texture arrays
    ClassDB::bind_method(D_METHOD("set_zombie_eyes", "textures"),  &Character::set_zombie_eyes);
    ClassDB::bind_method(D_METHOD("get_zombie_eyes"),              &Character::get_zombie_eyes);

    ClassDB::bind_method(D_METHOD("set_zombie_hair", "textures"),  &Character::set_zombie_hair);
    ClassDB::bind_method(D_METHOD("get_zombie_hair"),              &Character::get_zombie_hair);

    ClassDB::bind_method(D_METHOD("set_zombie_teeth", "textures"), &Character::set_zombie_teeth);
    ClassDB::bind_method(D_METHOD("get_zombie_teeth"),             &Character::get_zombie_teeth);

    ClassDB::bind_method(D_METHOD("set_zombie_hands", "textures"), &Character::set_zombie_hands);
    ClassDB::bind_method(D_METHOD("get_zombie_hands"),             &Character::get_zombie_hands);

    ClassDB::bind_method(D_METHOD("set_zombie_feet", "textures"),  &Character::set_zombie_feet);
    ClassDB::bind_method(D_METHOD("get_zombie_feet"),              &Character::get_zombie_feet);

    // Bind setters/getters for human texture arrays
    ClassDB::bind_method(D_METHOD("set_human_eyes", "textures"),   &Character::set_human_eyes);
    ClassDB::bind_method(D_METHOD("get_human_eyes"),               &Character::get_human_eyes);

    ClassDB::bind_method(D_METHOD("set_human_hair", "textures"),   &Character::set_human_hair);
    ClassDB::bind_method(D_METHOD("get_human_hair"),               &Character::get_human_hair);

    ClassDB::bind_method(D_METHOD("set_human_teeth", "textures"),  &Character::set_human_teeth);
    ClassDB::bind_method(D_METHOD("get_human_teeth"),              &Character::get_human_teeth);

    ClassDB::bind_method(D_METHOD("set_human_hands", "textures"),  &Character::set_human_hands);
    ClassDB::bind_method(D_METHOD("get_human_hands"),              &Character::get_human_hands);

    ClassDB::bind_method(D_METHOD("set_human_feet", "textures"),   &Character::set_human_feet);
    ClassDB::bind_method(D_METHOD("get_human_feet"),               &Character::get_human_feet);

    // Add properties so they show up in the Godot editor
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_eyes"),  "set_zombie_eyes",  "get_zombie_eyes");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_hair"),  "set_zombie_hair",  "get_zombie_hair");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_teeth"), "set_zombie_teeth", "get_zombie_teeth");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_hands"), "set_zombie_hands", "get_zombie_hands");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "zombie_feet"),  "set_zombie_feet",  "get_zombie_feet");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_eyes"),   "set_human_eyes",   "get_human_eyes");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_hair"),   "set_human_hair",   "get_human_hair");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_teeth"),  "set_human_teeth",  "get_human_teeth");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_hands"),  "set_human_hands",  "get_human_hands");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "human_feet"),   "set_human_feet",   "get_human_feet");
}

// Constructor
Character::Character() {
}

// Called when added to the scene tree
void Character::_enter_tree() {
    // Nothing special to do here yet
}

// Called when the node is fully ready in the scene
void Character::_ready() {
    // Grab references to all the sprite children that should already exist
    if (has_node("Eyes")) {
        eyes_sprite = get_node<Sprite2D>("Eyes");
    }
    if (has_node("Hair")) {
        hair_sprite = get_node<Sprite2D>("Hair");
    }
    if (has_node("Teeth")) {
        teeth_sprite = get_node<Sprite2D>("Teeth");
    }
    if (has_node("Hands")) {
        hands_sprite = get_node<Sprite2D>("Hands");
    }
    if (has_node("Feet")) {
        feet_sprite = get_node<Sprite2D>("Feet");
    }
    if (has_node("Portrait")) {
        portrait_sprite = get_node<Sprite2D>("Portrait");
    }

    // Load all textures from the art folders if not already loaded
    ensure_feature_textures_loaded();
}

// Setters and getters for is_zombie flag
void Character::set_is_zombie(bool value) {
    is_zombie = value;
}

bool Character::get_is_zombie() const {
    return is_zombie;
}

// Setters for zombie texture arrays
void Character::set_zombie_eyes(const Array &arr)  { zombie_eyes  = arr; }
void Character::set_zombie_hair(const Array &arr)  { zombie_hair  = arr; }
void Character::set_zombie_teeth(const Array &arr) { zombie_teeth = arr; }
void Character::set_zombie_hands(const Array &arr) { zombie_hands = arr; }
void Character::set_zombie_feet(const Array &arr)  { zombie_feet  = arr; }

// Setters for human texture arrays
void Character::set_human_eyes(const Array &arr)   { human_eyes   = arr; }
void Character::set_human_hair(const Array &arr)   { human_hair   = arr; }
void Character::set_human_teeth(const Array &arr)  { human_teeth  = arr; }
void Character::set_human_hands(const Array &arr)  { human_hands  = arr; }
void Character::set_human_feet(const Array &arr)   { human_feet   = arr; }

// Getters for zombie texture arrays
Array Character::get_zombie_eyes() const  { return zombie_eyes; }
Array Character::get_zombie_hair() const  { return zombie_hair; }
Array Character::get_zombie_teeth() const { return zombie_teeth; }
Array Character::get_zombie_hands() const { return zombie_hands; }
Array Character::get_zombie_feet() const  { return zombie_feet; }

// Getters for human texture arrays
Array Character::get_human_eyes() const   { return human_eyes; }
Array Character::get_human_hair() const   { return human_hair; }
Array Character::get_human_teeth() const  { return human_teeth; }
Array Character::get_human_hands() const  { return human_hands; }
Array Character::get_human_feet() const   { return human_feet; }

// Pick a random texture from an array
Ref<Texture2D> Character::get_random_texture(const Array &arr) {
    if (arr.is_empty()) {
        return Ref<Texture2D>();
    }

    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();
    int index = rng->randi_range(0, arr.size() - 1);

    Ref<Texture2D> tex = arr[index];
    return tex;
}

// Give this character a random look based on whether they're human or zombie
void Character::randomize_features() {
    // Make sure all sprites exist
    if (!eyes_sprite || !hair_sprite || !teeth_sprite || !hands_sprite || !feet_sprite) {
        UtilityFunctions::push_warning("Character sprites not set up correctly.");
        return;
    }

    // Pick the right texture arrays based on type
    const Array &eyes_arr   = is_zombie ? zombie_eyes   : human_eyes;
    const Array &hair_arr   = is_zombie ? zombie_hair   : human_hair;
    const Array &teeth_arr  = is_zombie ? zombie_teeth  : human_teeth;
    const Array &hands_arr  = is_zombie ? zombie_hands  : human_hands;
    const Array &feet_arr   = is_zombie ? zombie_feet   : human_feet;

    // Portrait array is mutable because we remove used portraits
    Array &portrait_arr     = is_zombie ? zombie_portrait : human_portrait;

    // Find the smallest array size so we don't go out of bounds
    int count = eyes_arr.size();
    count = MIN(count, hair_arr.size());
    count = MIN(count, teeth_arr.size());
    count = MIN(count, hands_arr.size());
    count = MIN(count, feet_arr.size());

    if (count == 0) {
        UtilityFunctions::push_warning("Character: no textures configured for this type.");
        return;
    }

    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();

    // Pick one index - all body parts come from the same "set"
    int idx = rng->randi_range(0, count - 1);

    // Helper to safely get a texture from an array
    auto get_tex = [](const Array &arr, int i) -> Ref<Texture2D> {
        if (i < 0 || i >= arr.size()) {
            return Ref<Texture2D>();
        }
        return arr[i];
    };

    // Assign all the body part textures
    eyes_sprite->set_texture(get_tex(eyes_arr, idx));
    hair_sprite->set_texture(get_tex(hair_arr, idx));
    teeth_sprite->set_texture(get_tex(teeth_arr, idx));
    hands_sprite->set_texture(get_tex(hands_arr, idx));
    feet_sprite->set_texture(get_tex(feet_arr, idx));

    // Portrait is special - each character gets a unique one
    if (portrait_sprite) {
        Ref<Texture2D> portrait_tex;

        if (!portrait_arr.is_empty()) {
            int pidx = rng->randi_range(0, portrait_arr.size() - 1);
            portrait_tex = portrait_arr[pidx];

            // Remove this portrait so no other character can use it
            portrait_arr.remove_at(pidx);
        }

        portrait_sprite->set_texture(portrait_tex);
    }
}

// Load all image files from a directory
Array Character::load_textures_from_dir(const String &path) {
    Array result;

    Ref<DirAccess> dir = DirAccess::open(path);
    if (dir.is_null()) {
        UtilityFunctions::push_warning("Character: Could not open directory: " + path);
        return result;
    }

    dir->list_dir_begin();
    while (true) {
        String file_name = dir->get_next();
        if (file_name.is_empty()) {
            break; // no more files
        }

        // Skip directories and special entries
        if (file_name == "." || file_name == ".." || dir->current_is_dir()) {
            continue;
        }

        // Only load image files
        if (!file_name.ends_with(".png") &&
            !file_name.ends_with(".jpg") &&
            !file_name.ends_with(".jpeg") &&
            !file_name.ends_with(".webp")) {
            continue;
        }

        String full_path = path.path_join(file_name);
        Ref<Texture2D> tex = ResourceLoader::get_singleton()->load(full_path);
        if (tex.is_valid()) {
            result.append(tex);
        } else {
            UtilityFunctions::push_warning("Character: Failed to load texture: " + full_path);
        }
    }

    dir->list_dir_end();
    return result;
}

// Make sure all texture arrays are populated from the art folders
void Character::ensure_feature_textures_loaded() {
    // Load zombie textures
    if (zombie_eyes.is_empty()) {
        zombie_eyes = load_textures_from_dir("res://art/zombies/eyes");
    }
    if (zombie_hair.is_empty()) {
        zombie_hair = load_textures_from_dir("res://art/zombies/hair");
    }
    if (zombie_teeth.is_empty()) {
        zombie_teeth = load_textures_from_dir("res://art/zombies/teeth");
    }
    if (zombie_hands.is_empty()) {
        zombie_hands = load_textures_from_dir("res://art/zombies/hands");
    }
    if (zombie_feet.is_empty()) {
        zombie_feet = load_textures_from_dir("res://art/zombies/feet");
    }
    if (zombie_portrait.is_empty()) {
        zombie_portrait = load_textures_from_dir("res://art/zombies/portrait");
    }

    // Load human textures
    if (human_eyes.is_empty()) {
        human_eyes = load_textures_from_dir("res://art/humans/eyes");
    }
    if (human_hair.is_empty()) {
        human_hair = load_textures_from_dir("res://art/humans/hair");
    }
    if (human_teeth.is_empty()) {
        human_teeth = load_textures_from_dir("res://art/humans/teeth");
    }
    if (human_hands.is_empty()) {
        human_hands = load_textures_from_dir("res://art/humans/hands");
    }
    if (human_feet.is_empty()) {
        human_feet = load_textures_from_dir("res://art/humans/feet");
    }
    if (human_portrait.is_empty()) {
        human_portrait = load_textures_from_dir("res://art/humans/portrait");
    }
}