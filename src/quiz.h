#ifndef QUIZMANAGER_HPP
#define QUIZMANAGER_HPP

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

class QuizManager : public Node {
    GDCLASS(QuizManager, Node);

public:
    struct Question {
        String text;
        PackedStringArray choices;
        int correct_index = 0;
    };

protected:
    static void _bind_methods();

    // Internal storage
    Array questions;        // Array of Question (wrapped as Dictionary for Godot side)
    int current_index = -1;

public:
    QuizManager();

    // Load questions for the given difficulty + day.
    // difficulty: 0 = EASY, 1 = MID, 2 = HARD
    void load_questions_for_day(int difficulty, int day);

    // Return next question as:
    // { "text": String, "choices": PackedStringArray, "index": int }
    Dictionary get_next_question();

    // Check answer of current question.
    // Returns true if correct, false otherwise.
    bool answer_current_question(int choice_index);

    // Whether there are more questions left.
    bool has_more_questions() const;

    // Clear all questions and reset index.
    void reset();

private:
    void add_question(const String &text, const PackedStringArray &choices, int correct_index);
    void fill_questions_easy(int day);
    void fill_questions_mid(int day);
    void fill_questions_hard(int day);
};

#endif // QUIZMANAGER_HPP
