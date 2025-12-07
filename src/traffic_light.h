#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/texture_rect.hpp>
#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

enum TrafficLightType{
    TRAFFIC_LIGHT_GO,
    TRAFFIC_LIGHT_STOP,
    TRAFFIC_LIGHT_CAUTION,
};

class TrafficLight: public Control{
    GDCLASS(TrafficLight, Control); 

    TextureRect *texture_rect;

    Ref<Texture2D> go_texture;
    Ref<Texture2D> stop_texture;
    Ref<Texture2D> caution_texture;

    TrafficLightType light_type;


protected:
    static void _bind_methods();
    void _notification(int p_what);

public: 

    void set_go_texture(const Ref<Texture2D> &p_texture);
    Ref<Texture2D> get_go_texture() const;

    
    void set_stop_texture(const Ref<Texture2D> &p_texture);
    Ref<Texture2D> get_stop_texture() const;

    void set_caution_texture(const Ref<Texture2D> &p_texture);
    Ref<Texture2D> get_caution_texture() const;

    void set_light_type(TrafficLightType p_type);
    TrafficLightType get_light_type() const;
    


    TrafficLight();


};

VARIANT_ENUM_CAST(TrafficLightType);