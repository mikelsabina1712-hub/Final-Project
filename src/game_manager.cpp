#include "game_manager.h"
#include "zombie.h"
#include "human.h"
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>  
#include <godot_cpp/classes/object.hpp>

using namespace godot;

// Bind all methods to Godot
void GameManager::_bind_methods() {
    UtilityFunctions::print("GameManager::_bind_methods was called (FULL)");

    ClassDB::bind_method(D_METHOD("set_difficulty", "difficulty"),
                         &GameManager::set_difficulty);
    ClassDB::bind_method(D_METHOD("get_difficulty"),
                         &GameManager::get_difficulty);

    ClassDB::bind_method(D_METHOD("get_tokens"),
                         &GameManager::get_tokens);
    ClassDB::bind_method(D_METHOD("get_day"),
                         &GameManager::get_day);
    ClassDB::bind_method(D_METHOD("is_player_alive"),
                         &GameManager::is_player_alive);
    ClassDB::bind_method(D_METHOD("spend_tokens", "amount"), &GameManager::spend_tokens);

    ClassDB::bind_method(D_METHOD("debug_print_residents"),
                     &GameManager::debug_print_residents);
    ClassDB::bind_method(D_METHOD("get_notifications"),
                     &GameManager::get_notifications);
    ClassDB::bind_method(D_METHOD("clear_notifications"),
                     &GameManager::clear_notifications);

    ClassDB::bind_method(D_METHOD("has_player_won"),
                     &GameManager::has_player_won);

    ClassDB::bind_method(D_METHOD("start_new_game"),
                         &GameManager::start_new_game);
    ClassDB::bind_method(D_METHOD("start_night"),
                         &GameManager::start_night);
    ClassDB::bind_method(D_METHOD("finish_night"),
                         &GameManager::finish_night);
    ClassDB::bind_method(D_METHOD("start_day"),
                         &GameManager::start_day);

    ClassDB::bind_method(D_METHOD("let_visitor_in", "visitor"),
                         &GameManager::let_visitor_in);
    ClassDB::bind_method(D_METHOD("reject_visitor", "visitor"),
                         &GameManager::reject_visitor);
    ClassDB::bind_method(D_METHOD("get_resident_count"),
                         &GameManager::get_resident_count);

    ClassDB::bind_method(D_METHOD("start_quiz"),
                         &GameManager::start_quiz);
    ClassDB::bind_method(D_METHOD("answer_quiz_question", "choice_index"),
                         &GameManager::answer_quiz_question);
    ClassDB::bind_method(D_METHOD("quiz_has_more_questions"),
                         &GameManager::quiz_has_more_questions);

    ClassDB::bind_method(D_METHOD("inspect_resident", "idx", "token_cost"),
                         &GameManager::inspect_resident);
    ClassDB::bind_method(D_METHOD("decide_resident_fate", "idx", "kill"),
                         &GameManager::decide_resident_fate);

    ClassDB::bind_method(D_METHOD("resolve_night_danger"),
                         &GameManager::resolve_night_danger);

    ClassDB::bind_method(D_METHOD("next_day"), &GameManager::next_day);

    ClassDB::bind_method(D_METHOD("get_resident_character", "idx"), &GameManager::get_resident_character);
}

// Constructor - initialize everything
GameManager::GameManager() {
    day = 0;
    tokens = 0;
    player_alive = false;
    player_won = false;

    rng.instantiate();
    rng->randomize();

    notifications.clear();
}

// Called when the node is ready in the scene
void GameManager::_ready() {
    UtilityFunctions::print("GameManager READY, listing children:");
    UtilityFunctions::print("GameManager::_ready: my path = ", get_path());

    // Debug - list all children
    Array children = get_children();
    for (int i = 0; i < children.size(); i++) {
        Node *child = Object::cast_to<Node>(children[i]);
        if (child) {
            UtilityFunctions::print(" - child[", i, "]: name=", child->get_name(), " class=", child->get_class());
        }
    }

    // Try to find DayManager
    Node *dm_raw = get_node_or_null("DayManager");
    if (dm_raw) {
        UtilityFunctions::print("Raw DayManager node found, class: ", dm_raw->get_class());
    } else {
        UtilityFunctions::print("Raw DayManager node NOT found at path 'DayManager'.");
    }

    // Get references to child nodes
    day_manager  = Object::cast_to<DayManager>(dm_raw);
    quiz_manager = get_node<QuizManager>("QuizManager");
    player       = get_node<Player>("Player");

    // Error checking
    if (!day_manager) {
        UtilityFunctions::push_error("GameManager: DayManager node not found or wrong type.");
    }
    if (!quiz_manager) {
        UtilityFunctions::push_error("GameManager: QuizManager node not found.");
    }
    if (!player) {
        UtilityFunctions::push_error("GameManager: Player node not found.");
    }

    UtilityFunctions::print("GameManager READY");
}

// Difficulty management
void GameManager::set_difficulty(int d) {
    if (d < 0) d = 0;
    if (d > 2) d = 2;
    difficulty = (Difficulty)d;
}

int GameManager::get_difficulty() const {
    return (int)difficulty;
}

// Token system
int GameManager::get_tokens() const {
    return tokens;
}

void GameManager::earn_tokens(int amount) {
    if (amount < 0) return;
    tokens += amount;
    UtilityFunctions::print("You earned ", amount, " tokens. Total tokens: ", tokens);
}

bool GameManager::spend_tokens(int amount) {
    if (amount <= 0) return true;
    if (tokens < amount) {
        UtilityFunctions::print("Not enough tokens. You have: ", tokens);
        return false;
    }
    tokens -= amount;
    UtilityFunctions::print("Spent ", amount, " tokens. Remaining: ", tokens);
    return true;
}

// Start a new game - reset everything
void GameManager::start_new_game() {
    day = 0;
    tokens = 0;
    player_alive = true;
    player_won = false;
    residents.clear();

    UtilityFunctions::print("New game started. Day 0 begins.");
}

// Night phase - spawn visitors
void GameManager::start_night() {
    if (!player_alive) {
        return;
    }

    UtilityFunctions::print("Night ", day, " begins.");

    // Lazy initialization of day_manager if needed
    if (!day_manager) {
        UtilityFunctions::print("GameManager::start_night: day_manager is null, trying to find it now…");

        Node *dm_raw = get_node_or_null("DayManager");
        if (!dm_raw) {
            UtilityFunctions::push_error("GameManager: Node 'DayManager' not found under GameManager in start_night.");
            return;
        }

        day_manager = Object::cast_to<DayManager>(dm_raw);
        if (!day_manager) {
            UtilityFunctions::push_error("GameManager: Node 'DayManager' exists but is not a DayManager in start_night.");
            return;
        }

        UtilityFunctions::print("GameManager::start_night: successfully grabbed DayManager at runtime.");
    }

    // Use our parent node as the spawn location
    Node2D *world = Object::cast_to<Node2D>(get_parent());
    if (!world) {
        UtilityFunctions::push_error("GameManager: Parent is not a Node2D. Cannot spawn visitors.");
        return;
    }

    day_manager->start_new_day(world);
}

// End of night - check what happened based on who's in the house
void GameManager::finish_night() {
    if (!player_alive) {
        return;
    }

    // Find the House node where residents live
    Node *house = get_node_or_null("House");
    if (!house) {
        UtilityFunctions::push_error("GameManager::finish_night: 'House' node not found.");
        return;
    }

    Array humans;
    Array zombies;

    // Count how many humans and zombies are in the house
    for (int i = 0; i < house->get_child_count(); i++) {
        Node *child = house->get_child(i);
        Character *c = Object::cast_to<Character>(child);
        if (!c) {
            continue;
        }

        if (Object::cast_to<ZombieCharacter>(c)) {
            zombies.append(c);
        } else if (Object::cast_to<HumanCharacter>(c)) {
            humans.append(c);
        }
    }

    Ref<RandomNumberGenerator> rng;
    rng.instantiate();
    rng->randomize();

    // Case 1: Nobody home - you die
    if (humans.size() == 0 && zombies.size() == 0) {
        player_alive = false;
        UtilityFunctions::print("No one was in the house to protect you. An intruder kills you in the night.");
        push_notification("No one was in the house to protect you. An intruder kills you in the night.");
        return;
    }

    // Case 2: Both humans and zombies - 50% chance zombies kill a human
    if (humans.size() > 0 && zombies.size() > 0) {
        const double HUMAN_KILL_CHANCE = 0.5;

        if (rng->randf() < HUMAN_KILL_CHANCE) {
            int victim_index = rng->randi_range(0, humans.size() - 1);
            Character *victim = Object::cast_to<Character>(humans[victim_index]);
            if (victim) {
                UtilityFunctions::print("Zombies killed one of your residents during the night.");
                push_notification("Zombies killed one of your residents during the night.");
                victim->queue_free();

                // Remove from our residents list too
                for (int i = (int)residents.size() - 1; i >= 0; --i) {
                    Resident &r = residents[i];
                    if (r.character == victim) {
                        residents.erase(residents.begin() + i);
                        break;
                    }
                }
            }
        } else {
            UtilityFunctions::print("Zombies failed to kill anyone tonight.");
            push_notification("Zombies failed to kill anyone tonight.");
        }

        // You survive, move to next day
        next_day();
        if (day_manager) {
            day_manager->end_day();
        } else {
            UtilityFunctions::push_error("GameManager::finish_night: day_manager is null when ending day.");
        }
        return;
    }

    // Case 3: Only zombies - 50% chance they kill you
    if (zombies.size() > 0 && humans.size() == 0) {
        const double PLAYER_KILL_CHANCE = 0.5;

        if (rng->randf() < PLAYER_KILL_CHANCE) {
            player_alive = false;
            UtilityFunctions::print("There were only zombies in the house. They killed you during the night.");
            push_notification("There were only zombies in the house. They killed you during the night.");
            return;
        } else {
            UtilityFunctions::print("There were only zombies, but you got lucky and survived the night.");
            push_notification("There were only zombies in the house, but you got lucky and survived the night.");

            next_day();
            if (day_manager) {
                day_manager->end_day();
            } else {
                UtilityFunctions::push_error("GameManager::finish_night: day_manager is null when ending day.");
                push_notification("GameManager::finish_night: day_manager is null when ending day.");
            }
            return;
        }
    }

    // Case 4: Only humans - totally safe
    if (zombies.size() == 0 && humans.size() > 0) {
        UtilityFunctions::print("Your human residents kept you safe tonight.");
        push_notification("Your human residents kept you safe tonight.");
        next_day();
        if (day_manager) {
            day_manager->end_day();
        } else {
            UtilityFunctions::push_error("GameManager::finish_night: day_manager is null when ending day.");
        }
        return;
    }

    // Shouldn't reach here but just in case
    day++;
}

// Move to the next day
void GameManager::next_day() {
    day++;         
    start_day();
}

// Day starts - do the morning routine
void GameManager::start_day() {
    if (!player_alive) return;

    // Check if we've won
    check_win_condition();
    if (!player_alive) {
        return;
    }

    UtilityFunctions::print("Morning of day ", day, ".");

    // Handle residents who've been here too long
    increment_resident_days_and_handle_departures();

    UtilityFunctions::print("News: new zombie trait revealed for day ", day, ".");

    // Start the daily quiz
    start_quiz();
}

// Accept a visitor into the house
void GameManager::let_visitor_in(Character *visitor) {
    if (!visitor) return;

    // Check if house is full
    int current_residents = get_resident_count();
    if (get_resident_count()  >= MAX_RESIDENTS) {
        UtilityFunctions::print("House is full. You cannot let more people in.");
        return;
    }

    // Add to our residents list
    Resident r;
    r.character = visitor;
    r.days_in_house = 0;
    r.alive = true;

    residents.push_back(r);

    // Move them into the House node
    Node *house_node = get_node<Node>("House");
    if (house_node) {
        visitor->reparent(house_node);
    }

    // Remove from daily visitors list
    if (day_manager) {
        day_manager->remove_daily_character(visitor);
    }

    UtilityFunctions::print("You let someone into the house. Total residents: ", (int)residents.size());
}

// Reject a visitor at the door
void GameManager::reject_visitor(Character *visitor) {
    if (!visitor) return;

    UtilityFunctions::print("You rejected a visitor at the door.");

    // Remove from daily visitors list
    if (day_manager) {
        day_manager->remove_daily_character(visitor);
    }

    // Delete the character
    visitor->queue_free();

    debug_print_residents();
}

// Start the daily quiz
void GameManager::start_quiz() {
    // Lazy initialization if needed
    if (!quiz_manager) {
        quiz_manager = get_node<QuizManager>("QuizManager");
        if (!quiz_manager) {
            UtilityFunctions::push_error("GameManager: No QuizManager for start_quiz.");
            return;
        }
    }

    // Reset and load questions for today
    quiz_manager->reset();
    quiz_manager->load_questions_for_day((int)difficulty, day);

    if (!quiz_manager->has_more_questions()) {
        UtilityFunctions::print("No quiz questions loaded for today. Skipping straight to inspection.");
        return;
    }

    UtilityFunctions::print("Quiz started for day ", day, ".");
}

// Answer a quiz question
bool GameManager::answer_quiz_question(int choice_index) {
    if (!quiz_manager) return false;

    bool correct = quiz_manager->answer_current_question(choice_index);
    if (correct) {
        int reward = 1;
        earn_tokens(reward);
    } else {
        UtilityFunctions::print("Wrong answer.");
    }

    if (!quiz_manager->has_more_questions()) {
        UtilityFunctions::print("Quiz finished. You have ", tokens, " tokens.");
    }

    return correct;
}

// Check if there are more quiz questions
bool GameManager::quiz_has_more_questions() const {
    if (!quiz_manager) return false;
    return quiz_manager->has_more_questions();
}

// Inspect a resident - costs tokens
bool GameManager::inspect_resident(int idx, int token_cost) {
    if (idx < 0 || idx >= (int)residents.size()) {
        UtilityFunctions::print("Invalid resident index for inspection.");
        return false;
    }

    Resident &r = residents[idx];
    if (!r.alive || !r.character) {
        UtilityFunctions::print("This resident is already dead or missing.");
        return false;
    }

    if (!spend_tokens(token_cost)) {
        return false;
    }

    bool is_zombie = r.character->get_is_zombie();
    UtilityFunctions::print("Inspection result: resident ", idx,
                            is_zombie ? " IS a zombie." : " seems HUMAN.");

    return is_zombie;
}

// Decide what to do with a resident (kill or spare)
void GameManager::decide_resident_fate(int idx, bool kill) {
    if (idx < 0) {
        UtilityFunctions::print("Invalid resident index for decision.");
        return;
    }

    // Interpret idx as "alive index" - skip dead residents when counting
    int alive_index = 0;
    int real_index  = -1;

    for (int i = 0; i < (int)residents.size(); ++i) {
        Resident &r = residents[i];
        if (!r.alive || r.character == nullptr) {
            continue;
        }

        if (alive_index == idx) {
            real_index = i;
            break;
        }

        alive_index++;
    }

    if (real_index == -1) {
        UtilityFunctions::print("No alive resident found for index ", idx);
        return;
    }

    Resident &r = residents[real_index];
    if (!r.alive || !r.character) {
        UtilityFunctions::print("Cannot change fate of this resident.");
        return;
    }

    if (kill) {
        UtilityFunctions::print("You killed resident alive-index ", idx,
                                " (internal index ", real_index, ").");

        // Delete the character node
        r.character->queue_free();

        // Remove from list immediately to keep UI in sync
        residents.erase(residents.begin() + real_index);
    } else {
        UtilityFunctions::print("You let resident alive-index ", idx,
                                " (internal index ", real_index, ") live.");
    }

    debug_print_residents();
}

// Night danger resolution (alternative system, not currently used)
void GameManager::resolve_night_danger() {
    if (residents.empty()) {
        UtilityFunctions::print("Your house is empty. Someone breaks in and kills you while you sleep.");
        push_notification("Your house is empty. Someone breaks in and kills you while you sleep.");
        player_alive = false;
        return;
    }

    std::vector<int> zombie_indices;
    std::vector<int> human_indices;

    // Sort residents into zombies and humans
    for (int i = 0; i < (int)residents.size(); ++i) {
        Resident &r = residents[i];
        if (!r.alive || !r.character) continue;

        if (r.character->get_is_zombie()) {
            zombie_indices.push_back(i);
        } else {
            human_indices.push_back(i);
        }
    }

    if (zombie_indices.empty()) {
        UtilityFunctions::print("No zombies inside tonight. You sleep peacefully.");
        push_notification("No zombies inside tonight. You sleep peacefully.");
        return;
    }

    // Danger increases each day
    float kill_prob = 0.2f + 0.1f * (day - 1); // day 1: 20%, day 7: 80%
    if (kill_prob > 0.95f) kill_prob = 0.95f;

    float roll = rng->randf();
    UtilityFunctions::print("Night danger roll: ", roll, ", threshold: ", kill_prob);

    if (roll > kill_prob) {
        UtilityFunctions::print("You got lucky. Zombies didn't attack tonight.");
        return;
    }

    // Attack happens
    if (!human_indices.empty()) {
        int victim_idx = human_indices[rng->randi_range(0, (int)human_indices.size() - 1)];
        Resident &victim = residents[victim_idx];
        victim.alive = false;

        UtilityFunctions::print("During the night, a zombie killed one of your human guests (index ",victim_idx, ").");
        push_notification("During the night, a zombie killed one of your human guests.");
        if (victim.character) {
            victim.character->queue_free();
            victim.character = nullptr;  
        }
    } 
    else {
        // No humans left, zombies kill you
        UtilityFunctions::print("No humans left… the zombies attack YOU in your sleep.");
        player_alive = false;
    }
}

// Track how many days residents have been here, kick out humans after 2 days
void GameManager::increment_resident_days_and_handle_departures() {
    // First pass - remove dead residents or ones with no character
    for (int i = (int)residents.size() - 1; i >= 0; --i) {
        Resident &r = residents[i];
        if (!r.alive || r.character == nullptr) {
            residents.erase(residents.begin() + i);
        }
    }

    // Add a day to each living human
    for (Resident &r : residents) {
        if (!r.character->get_is_zombie()) {
            r.days_in_house += 1;
        }
    }

    // Kick out humans who've stayed more than 2 days
    for (int i = (int)residents.size() - 1; i >= 0; --i) {
        Resident &r = residents[i];

        if (!r.character->get_is_zombie() && r.days_in_house > 2) {
            String msg = "A human guest left your house after " +
                         String::num_int64(r.days_in_house) + " days.";
            UtilityFunctions::print(msg);
            push_notification(msg);

            if (r.character) {
                r.character->queue_free();
            }

            residents.erase(residents.begin() + i);
        }
    }

    debug_print_residents();
}

// Check if player has won or lost
void GameManager::check_win_condition() {
    if (day > MAX_DAYS) {
        UtilityFunctions::print("You survived ", MAX_DAYS, " days. YOU WIN!");
        player_won = true;
        player_alive = false;
        return;
    }
}

// Count how many residents are actually alive
int GameManager::get_resident_count() const {
    int count = 0;
    for (const Resident &r : residents) {
        if (r.alive && r.character != nullptr) {
            ++count;
        }
    }
    return count;
}

// Get a resident character by alive index
Character *GameManager::get_resident_character(int idx) {
    if (idx < 0) {
        return nullptr;
    }

    int alive_index = 0;
    for (Resident &r : residents) {
        // Skip dead or empty entries
        if (!r.alive || r.character == nullptr) {
            continue;
        }

        if (alive_index == idx) {
            return r.character;
        }

        alive_index++;
    }

    return nullptr;
}

// Debug - print out all residents
void GameManager::debug_print_residents() const {
    UtilityFunctions::print("--- GameManager::debug_print_residents ---");
    UtilityFunctions::print("  total vector size = ", (int)residents.size(),
                            ", alive count = ", get_resident_count());

    int alive_index = 0;
    for (int i = 0; i < (int)residents.size(); ++i) {
        const Resident &r = residents[i];
        String char_name = "<null>";
        bool is_zombie = false;

        if (r.character) {
            char_name = r.character->get_name();
            is_zombie = r.character->get_is_zombie();
        }

        UtilityFunctions::print("  [", i, "] alive=", r.alive,
                                " days=", r.days_in_house,
                                " char=", char_name,
                                " is_zombie=", is_zombie ? "Z" : "H/?");

        if (r.alive && r.character != nullptr) {
            UtilityFunctions::print("      alive_index=", alive_index);
            alive_index++;
        }
    }
}

// Get notifications for the UI
Array GameManager::get_notifications() const {
    return notifications;
}

// Clear notifications
void GameManager::clear_notifications() {
    notifications.clear();
}

// Add a notification
void GameManager::push_notification(const String &msg) {
    notifications.append(msg);
}