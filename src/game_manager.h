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

class GameManager : public Node {
    GDCLASS(GameManager, Node);

public:
    static const int MAX_RESIDENTS = 5;
    static const int MAX_DAYS = 7;

    enum Difficulty {
        EASY = 0,
        MID  = 1,
        HARD = 2
    };

    struct Resident {
        Character *character = nullptr;
        int days_in_house = 0;   // full days survived
        bool alive = true;
    };

    GameManager();

    void _ready() override;
    void debug_print_residents() const;


    // Difficulty
    void set_difficulty(int d);
    int get_difficulty() const;

    // Tokens
    int get_tokens() const;
    void earn_tokens(int amount);
    bool spend_tokens(int amount);

    // State access
    int get_day() const { return day; }
    bool is_player_alive() const { return player_alive; }
    bool has_player_won() const { return player_won; }


    // Game flow
    void start_new_game();
    void start_night();
    void finish_night();
    void start_day();
    void next_day();
    Array get_notifications() const;
    void clear_notifications();



    // Visitors
    void let_visitor_in(Character *visitor);
    void reject_visitor(Character *visitor);
    int get_resident_count() const;
    void increment_resident_days_and_evict();


    // Quiz
    void start_quiz();
    bool answer_quiz_question(int choice_index);
    bool quiz_has_more_questions() const;

    // Inspection
    bool inspect_resident(int idx, int token_cost);
    void decide_resident_fate(int idx, bool kill);

    // Night danger
    void resolve_night_danger();

    Character* get_resident_character(int idx);


protected:
    static void _bind_methods();

private:
    // State
    int day = 1;
    Difficulty difficulty = EASY;
    bool player_alive = true;
    bool player_won = false;
    int tokens = 1;
    Array notifications;

    // References
    DayManager *day_manager = nullptr;
    QuizManager *quiz_manager = nullptr;
    Player *player = nullptr;

    std::vector<Resident> residents;
    Ref<RandomNumberGenerator> rng; 

    void increment_resident_days_and_handle_departures();
    void check_win_condition();
    void push_notification(const String &msg);

  

};

#endif // GAMEMANAGER_HPP
