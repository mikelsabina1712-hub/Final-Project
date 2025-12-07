#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class Player : public Node {
    GDCLASS(Player, Node);

protected:
    static void _bind_methods();

public:
    Player();
};

#endif // PLAYER_HPP
