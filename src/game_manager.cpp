#include "game_manager.h"
#include "zombie.h"
#include "human.h"
#include <godot_cpp/classes/random_number_generator.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/utility_functions.hpp>  
#include <godot_cpp/classes/object.hpp>



using namespace godot;

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

GameManager::GameManager() {
    day = 0;
    tokens = 0;
    player_alive = false;
    player_won = false;

    rng.instantiate();
    rng->randomize();

    notifications.clear();
}


void GameManager::_ready() {
    UtilityFunctions::print("GameManager READY, listing children:");
    UtilityFunctions::print("GameManager::_ready: my path = ", get_path());


    Array children = get_children();
    for (int i = 0; i < children.size(); i++) {
        Node *child = Object::cast_to<Node>(children[i]);
        if (child) {
            UtilityFunctions::print(" - child[", i, "]: name=", child->get_name(), " class=", child->get_class());
        }
    }

    Node *dm_raw = get_node_or_null("DayManager");
    if (dm_raw) {
        UtilityFunctions::print("Raw DayManager node found, class: ", dm_raw->get_class());
    } else {
        UtilityFunctions::print("Raw DayManager node NOT found at path 'DayManager'.");
    }

    day_manager  = Object::cast_to<DayManager>(dm_raw);
    quiz_manager = get_node<QuizManager>("QuizManager");
    player       = get_node<Player>("Player");

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

// Difficulty
void GameManager::set_difficulty(int d) {
    if (d < 0) d = 0;
    if (d > 2) d = 2;
    difficulty = (Difficulty)d;
}

int GameManager::get_difficulty() const {
    return (int)difficulty;
}

// Tokens
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

// Main flow
void GameManager::start_new_game() {
    day = 0;
    tokens = 0;
    player_alive = true;
    player_won = false;
    residents.clear();

    UtilityFunctions::print("New game started. Day 0 begins.");
    

    //start_night();
}

void GameManager::start_night() {
    if (!player_alive) {
        return;
    }

    UtilityFunctions::print("Night ", day, " begins.");

    
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

    // Use GameManager's parent (your main Node2D scene) as spawn parent
    Node2D *world = Object::cast_to<Node2D>(get_parent());
    if (!world) {
        UtilityFunctions::push_error("GameManager: Parent is not a Node2D. Cannot spawn visitors.");
        return;
    }

    day_manager->start_new_day(world);


}

void GameManager::finish_night() {
    if (!player_alive) {
        return;
    }

    // Safely get the House node
    Node *house = get_node_or_null("House"); // adjust path if needed
    if (!house) {
        UtilityFunctions::push_error("GameManager::finish_night: 'House' node not found.");
        return;
    }

    Array humans;
    Array zombies;

    // Collect residents from the House node
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

    // 3) Nobody in the house → you always die
    if (humans.size() == 0 && zombies.size() == 0) {
        player_alive = false;
        UtilityFunctions::print("No one was in the house to protect you. An intruder kills you in the night.");
        push_notification("No one was in the house to protect you. An intruder kills you in the night.");

        return;
    }

    // 1) Both humans AND zombies in the house
    if (humans.size() > 0 && zombies.size() > 0) {
        const double HUMAN_KILL_CHANCE = 0.5; // 50% chance

        if (rng->randf() < HUMAN_KILL_CHANCE) {
            int victim_index = rng->randi_range(0, humans.size() - 1);
            Character *victim = Object::cast_to<Character>(humans[victim_index]);
            if (victim) {
                UtilityFunctions::print("Zombies killed one of your residents during the night.");
                push_notification("Zombies killed one of your residents during the night.");
                victim->queue_free(); // schedule removal from the scene tree

                //  Sync with residents so we don't keep a stale pointer
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

        // Player survives this night if we reach here
        next_day();
        if (day_manager) {
            day_manager->end_day(); // clear nightly visitors
        } else {
            UtilityFunctions::push_error("GameManager::finish_night: day_manager is null when ending day.");
        }
        return;
    }

    // 2) Only zombies, no humans
    if (zombies.size() > 0 && humans.size() == 0) {
        const double PLAYER_KILL_CHANCE = 0.5; // tweak probability

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

    // 0 zombies, at least 1 human → totally safe
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

    // Fallback, shouldn't really hit this
    day++;
}

void GameManager::next_day() {
    day++;         
    start_day();
}


void GameManager::start_day() {
    if (!player_alive) return;

    check_win_condition();
    if (!player_alive) {
        return;
    }



    UtilityFunctions::print("Morning of day ", day, ".");

    increment_resident_days_and_handle_departures();

    UtilityFunctions::print("News: new zombie trait revealed for day ", day, ".");

    start_quiz();
}

// Visitors
void GameManager::let_visitor_in(Character *visitor) {
    if (!visitor) return;

    // Use *alive* resident count instead of raw vector size
    int current_residents = get_resident_count();
    if (get_resident_count()  >= MAX_RESIDENTS) {
        UtilityFunctions::print("House is full. You cannot let more people in.");
        return;
    }

    Resident r;
    r.character = visitor;
    r.days_in_house = 0;
    r.alive = true;

    residents.push_back(r);

    Node *house_node = get_node<Node>("House");
    if (house_node) {
        visitor->reparent(house_node);
    }

    if (day_manager) {
        day_manager->remove_daily_character(visitor);
    }

    UtilityFunctions::print("You let someone into the house. Total residents: ", (int)residents.size());
}


void GameManager::reject_visitor(Character *visitor) {
    if (!visitor) return;

    UtilityFunctions::print("You rejected a visitor at the door.");

    // Tell DayManager this visitor is gone, just like in let_visitor_in
    if (day_manager) {
        day_manager->remove_daily_character(visitor);
    }

    visitor->queue_free();

    // Just in case, show that residents list didn't change
    debug_print_residents();
}



// Quiz
void GameManager::start_quiz() {
    // Try to resolve lazily if it's null
    if (!quiz_manager) {
        quiz_manager = get_node<QuizManager>("QuizManager");
        if (!quiz_manager) {
            UtilityFunctions::push_error("GameManager: No QuizManager for start_quiz.");
            return;
        }
    }

    quiz_manager->reset();
    quiz_manager->load_questions_for_day((int)difficulty, day);

    if (!quiz_manager->has_more_questions()) {
        UtilityFunctions::print("No quiz questions loaded for today. Skipping straight to inspection.");
        return;
    }

    UtilityFunctions::print("Quiz started for day ", day, ".");
}

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

bool GameManager::quiz_has_more_questions() const {
    if (!quiz_manager) return false;
    return quiz_manager->has_more_questions();
}

// Inspection
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

void GameManager::decide_resident_fate(int idx, bool kill) {
    if (idx < 0) {
        UtilityFunctions::print("Invalid resident index for decision.");
        return;
    }

    // Interpret idx as "alive index", same as get_resident_character
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

        // Free the node
        r.character->queue_free();

        // Erase this resident *immediately* so UI indices stay in sync
        residents.erase(residents.begin() + real_index);
    } else {
        UtilityFunctions::print("You let resident alive-index ", idx,
                                " (internal index ", real_index, ") live.");
    }

    // Optional: debug after any decision
    debug_print_residents();
}

  
   

// Night danger
void GameManager::resolve_night_danger() {
    if (residents.empty()) {
        UtilityFunctions::print("Your house is empty. Someone breaks in and kills you while you sleep.");
        push_notification("Your house is empty. Someone breaks in and kills you while you sleep.");
        player_alive = false;
        return;
    }

    std::vector<int> zombie_indices;
    std::vector<int> human_indices;

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

    float kill_prob = 0.2f + 0.1f * (day - 1); // day 1: 20%, day 7: 80%
    if (kill_prob > 0.95f) kill_prob = 0.95f;

    float roll = rng->randf();
    UtilityFunctions::print("Night danger roll: ", roll, ", threshold: ", kill_prob);

    if (roll > kill_prob) {
        UtilityFunctions::print("You got lucky. Zombies didn't attack tonight.");
        return;
    }

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
    UtilityFunctions::print("No humans left… the zombies attack YOU in your sleep.");
    player_alive = false;
    }

}

void GameManager::increment_resident_days_and_handle_departures() {
    // 0) Remove residents that are already dead or have no character
    for (int i = (int)residents.size() - 1; i >= 0; --i) {
        Resident &r = residents[i];
        if (!r.alive || r.character == nullptr) {
            residents.erase(residents.begin() + i);
        }
    }

    // 1) Increment days for alive HUMAN residents
    for (Resident &r : residents) {
        if (!r.character->get_is_zombie()) {
            r.days_in_house += 1;
        }
    }

    // 2) Remove humans who have stayed more than 2 days
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




void GameManager::check_win_condition() {
    if (day > MAX_DAYS) {
        UtilityFunctions::print("You survived ", MAX_DAYS, " days. YOU WIN!");
        player_won = true;
        player_alive = false;
        return;
    }

}

int GameManager::get_resident_count() const {
    int count = 0;
    for (const Resident &r : residents) {
        // Count only residents that are marked alive and still have a character pointer
        if (r.alive && r.character != nullptr) {
            ++count;
        }
    }
    return count;
}


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

Array GameManager::get_notifications() const {
    return notifications;
}

void GameManager::clear_notifications() {
    notifications.clear();
}

void GameManager::push_notification(const String &msg) {
    notifications.append(msg);
}

