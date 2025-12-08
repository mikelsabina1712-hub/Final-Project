#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>
#include "days.h"
#include "player.h"
#include "quiz.h"
#include "character.h"

using namespace godot;

// Main game logic controller - handles the entire game flow
class GameManager : public Node {
    GDCLASS(GameManager, Node);

public:
    // Game constants
    static const int MAX_RESIDENTS = 5;
    static const int MAX_DAYS = 7;
    
    // Difficulty levels for the game
    enum Difficulty {
        EASY = 0,
        MID  = 1,
        HARD = 2
    };
    
    // Info about each person living in the house
    struct Resident {
        Character *character = nullptr;
        int days_in_house = 0;   // how many full days they've been here
        bool alive = true;
    };
    
    GameManager();
    void _ready() override;
    void debug_print_residents() const;
    
    // Difficulty settings
    void set_difficulty(int d);
    int get_difficulty() const;
    
    // Token economy - earn tokens from quiz, spend on inspections
    int get_tokens() const;
    void earn_tokens(int amount);
    bool spend_tokens(int amount);
    
    // Game state queries
    int get_day() const { return day; }
    bool is_player_alive() const { return player_alive; }
    bool has_player_won() const { return player_won; }
    
    // Main game flow control
    void start_new_game();
    void start_night();      // Spawn visitors
    void finish_night();     // Resolve what happens overnight
    void start_day();        // Morning routine
    void next_day();         // Move to next day
    Array get_notifications() const;
    void clear_notifications();
    
    // Visitor management
    void let_visitor_in(Character *visitor);
    void reject_visitor(Character *visitor);
    int get_resident_count() const;
    void increment_resident_days_and_evict();
    
    // Quiz phase
    void start_quiz();
    bool answer_quiz_question(int choice_index);
    bool quiz_has_more_questions() const;
    
    // Inspection phase - spend tokens to check if someone is a zombie
    bool inspect_resident(int idx, int token_cost);
    void decide_resident_fate(int idx, bool kill);
    
    // Night danger system (alternative implementation)
    void resolve_night_danger();
    Character* get_resident_character(int idx);

protected:
    static void _bind_methods();

private:
    // Game state variables
    int day = 1;
    Difficulty difficulty = EASY;
    bool player_alive = true;
    bool player_won = false;
    int tokens = 1;
    Array notifications;  // Messages to show the player
    
    // References to other manager nodes
    DayManager *day_manager = nullptr;
    QuizManager *quiz_manager = nullptr;
    Player *player = nullptr;
    
    // List of people currently living in the house
    std::vector<Resident> residents;
    
    Ref<RandomNumberGenerator> rng; 
    
    // Internal helper functions
    void increment_resident_days_and_handle_departures();
    void check_win_condition();
    void push_notification(const String &msg);
};

#endif // GAMEMANAGER_HPP