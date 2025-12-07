#include "quiz.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void QuizManager::_bind_methods() {
    UtilityFunctions::print("QUIZ::_bind_methods called");

    ClassDB::bind_method(D_METHOD("load_questions_for_day", "difficulty", "day"),
                         &QuizManager::load_questions_for_day);
    ClassDB::bind_method(D_METHOD("get_next_question"),
                         &QuizManager::get_next_question);
    ClassDB::bind_method(D_METHOD("answer_current_question", "choice_index"),
                         &QuizManager::answer_current_question);
    ClassDB::bind_method(D_METHOD("has_more_questions"),
                         &QuizManager::has_more_questions);
    ClassDB::bind_method(D_METHOD("reset"),
                         &QuizManager::reset);
}

QuizManager::QuizManager() {
    reset();
}

void QuizManager::reset() {
    questions.clear();
    current_index = -1;
}

void QuizManager::add_question(const String &text,
    const PackedStringArray &choices,
    int correct_index) {
    Dictionary q;
    q["text"] = text;
    q["choices"] = choices;
    q["correct_index"] = correct_index;
    questions.push_back(q);
}

void QuizManager::fill_questions_easy(int day) {
    // Example: 2 easy questions per day

    if (day == 1) {
        add_question("The correct way to compile with all warnings turned on is:",
                     PackedStringArray({"g++ file.cpp", "g++ -Wall -Wextra -Werror -pedantic file.cpp", "clang file.cpp", "make file.cpp"}), 1);
        add_question("True or False: using namespace std; is safe to put in a .cpp file.",
                     PackedStringArray({"False", "True (but discouraged in headers)"}), 1);
        add_question("What is printed? cout << boolalpha << (5 == 5.0);",
                     PackedStringArray({"1", "true", "True", "0"}), 1);
        add_question("argc always has at least the value:",
                     PackedStringArray({"0", "1", "2", "-1"}), 1);
        add_question("The command to see hidden files in Unix is:",
                     PackedStringArray({"ls", "ls -a", "ls -h", "ls --all"}), 1);
    }
    else if (day == 2) {
        add_question("True or False: std::vector automatically shrinks capacity when you erase elements.",
                     PackedStringArray({"True", "False"}), 1);
        add_question("What does v.at(5) do that v[5] does not?",
                     PackedStringArray({"Returns a pointer", "Bounds checking", "Moves the element", "Resizes"}), 1);
        add_question("True or False: A reference must always be initialized when declared.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("What is wrong with int arr[10]; arr[10] = 42;?",
                     PackedStringArray({"Syntax error", "Out-of-bounds", "Needs std::", "Nothing"}), 1);
        add_question("In a Makefile, $@ refers to:",
                     PackedStringArray({"All prerequisites", "The target", "The compiler", "The first dependency"}), 1);
    }
    else if (day == 3) {
        add_question("The correct include guard for utils.h is:",
                     PackedStringArray({"#ifndef UTILS_H", "#pragma once", "Both a and b are acceptable", "#define UTILS_H only"}), 2);
        add_question("True or False: std::cout << 10 << 5.5; prints 105.5 with no space.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("delete is used for:",
                     PackedStringArray({"Arrays", "Single objects", "Stack variables", "References"}), 1);
        add_question("The command to run Valgrind memory leak check is:",
                     PackedStringArray({"valgrind ./a.out", "valgrind --leak-check=full ./a.out", "gdb --leak ./a.out", "make leak"}), 1);
        add_question("True or False: int* p = new int[10]; delete p; is correct.",
                     PackedStringArray({"True", "False (must use delete[])"}), 1);
    }
    else if (day == 4) {
        add_question("What does git add . do?",
                     PackedStringArray({"Commits changes", "Stages all files", "Deletes files", "Pulls from remote"}), 1);
        add_question("True or False: #include \"header.h\" searches current directory first.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("The output of printf(\"%d\\n\", 42.0); is:",
                     PackedStringArray({"42.0", "42", "Compilation error", "0"}), 1);
        add_question("std::string s = \"hi\"; s += ' '; appends:",
                     PackedStringArray({"Nothing", "\"hi \"", "\"hi h\"", "Crash"}), 1);
        add_question("True or False: Loops can be nested infinitely without stack overflow.",
                     PackedStringArray({"True", "False"}), 1);
    }
    else if (day == 5) {
        add_question("What is the ternary operator?",
                     PackedStringArray({"if-else", "condition ? true : false", "&&", "=="}), 1);
        add_question("int x = 5; ++x; cout << x; prints:",
                     PackedStringArray({"5", "6", "4", "Undefined"}), 1);
        add_question("True or False: std::array size is fixed at compile time.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("The purpose of gdb is:",
                     PackedStringArray({"Compile", "Debug", "Build", "Version control"}), 1);
        add_question("break main in gdb sets a breakpoint at:",
                     PackedStringArray({"Line 1", "Function main", "End of program", "Variable"}), 1);
    }
    else if (day == 6) {
        add_question("True or False: const int& can bind to temporaries.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("What does v.reserve(10) do?",
                     PackedStringArray({"Adds 10 elements", "Allocates space for 10", "Shrinks", "Sorts"}), 1);
        add_question("chmod 755 file makes it:",
                     PackedStringArray({"Read-only", "Executable by owner", "Hidden", "Deleted"}), 1);
        add_question("True or False: main must return int.",
                    PackedStringArray({"True", "False"}), 0);
        add_question("What is UB?",
                     PackedStringArray({"Defined behavior", "Undefined behavior", "User behavior", "Unix binary"}), 1);
    }
    else { // day 7 and fallback
        add_question("True or False: new int(5) returns a pointer.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("ls -la shows:",
                     PackedStringArray({"Only files", "Hidden + details", "Directories", "Nothing"}), 1);
        add_question("In regex, . matches:",
                     PackedStringArray({"Any char", "Digit", "Word", "Line end"}), 0);
        add_question("True or False: [[nodiscard]] warns if ignored.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("True or False: git diff shows unstaged changes.",
                     PackedStringArray({"True", "False"}), 0);
    }
}

void QuizManager::fill_questions_mid(int day) {
    if (day == 1) {
        add_question("When deleting through a base pointer, the base destructor must be:",
                     PackedStringArray({"private", "protected", "virtual", "constexpr"}), 2);
        add_question("True or False: The default copy constructor performs a deep copy of all members.",
                     PackedStringArray({"True", "False (shallow for pointers)"}), 1);
        add_question("What does std::move actually do?",
                     PackedStringArray({"Immediately moves data", "Casts to rvalue reference", "Calls move constructor", "Deletes original"}), 1);
        add_question("The Rule of Zero is possible when your class:",
                     PackedStringArray({"Has no pointers", "Only uses RAII types", "Has virtual functions", "Is final"}), 1);
        add_question("Object slicing happens when:",
                     PackedStringArray({"You delete a derived object", "You assign derived to base by value", "You use references", "You use unique_ptr"}), 1);
    }
    else if (day == 2) {
        add_question("True or False: override keyword is required to override a virtual function.",
                     PackedStringArray({"True", "False (but highly recommended)"}), 1);
        add_question("Which smart pointer allows shared ownership?",
                     PackedStringArray({"unique_ptr", "weak_ptr", "shared_ptr", "raw_ptr"}), 2);
        add_question("std::string usually uses SSO for strings shorter than about:",
                     PackedStringArray({"8 chars", "15–23 chars", "64 chars", "256 chars"}), 1);
        add_question("True or False: A lambda that captures by reference is safe to return from a function.",
                     PackedStringArray({"True", "False (usually dangling)"}), 1);
        add_question("The correct way to transfer ownership of a unique_ptr is:",
                     PackedStringArray({"p2 = p1;", "p2 = std::move(p1);", "p2 = &p1;", "std::swap(p1, p2);"}), 1);
    }
    else if (day == 3) {
        add_question("What is the Rule of Three?",
                     PackedStringArray({"Dtor, copy ctor, copy assign", "Move only", "All defaults", "Zero"}), 0);
        add_question("True or False: Move semantics require noexcept.",
                     PackedStringArray({"True", "False (but recommended for strong exception guarantee)"}), 1);
        add_question("std::map vs std::unordered_map:",
                     PackedStringArray({"Unordered is slower", "Map is ordered", "Same speed", "No difference"}), 1);
        add_question("Virtual destructor ensures:",
                     PackedStringArray({"No polymorphism", "Derived dtor runs", "Copy safe", "Move only"}), 1);
        add_question("True or False: final prevents overriding.",
                     PackedStringArray({"True", "False"}), 0);
    }
    else if (day == 4) {
        add_question("The Big Five includes:",
                     PackedStringArray({"Only copy", "Dtor + copy/move ctor/assign", "Lambdas", "Templates"}), 1);
        add_question("What is RVO?",
                     PackedStringArray({"Return Value Optimization", "Rare Virtual Override", "Run-time Value Object", "None"}), 0);
        add_question("True or False: std::unique_ptr can be copied.",
                     PackedStringArray({"True", "False (moved only)"}), 1);
        add_question("Heap allocation uses:",
                     PackedStringArray({"Stack", "new/delete", "Auto", "Static"}), 1);
        add_question("In gdb, next vs step:",
                     PackedStringArray({"Same", "Next skips functions", "Step enters", "None"}), 2);
    }
    else if (day == 5) {
        add_question("True or False: File I/O uses RAII with ifstream.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("argc/argv for command args:",
                     PackedStringArray({"Strings only", "Count + array of char*", "Ints", "Vectors"}), 1);
        add_question("What is a macro danger?",
                     PackedStringArray({"Type safe", "No checking", "Slow", "Fast"}), 1);
        add_question("True or False: std::vector::erase invalidates iterators after erased.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("Pointer vs reference:",
                     PackedStringArray({"Ref can be null", "Pointer can", "Same", "Ref slower"}), 1);
    }
    else if (day == 6) {
        add_question("What is this?",
                     PackedStringArray({"Global", "Pointer to current object", "Static", "Void"}), 1);
        add_question("True or False: Default ctor is provided if none declared.",
                     PackedStringArray({"True (for POD)", "False"}), 0);
        add_question("protected inheritance makes base public become:",
                     PackedStringArray({"Public", "Protected", "Private", "Virtual"}), 1);
        add_question("Polymorphism requires:",
                     PackedStringArray({"No virtual", "Virtual functions", "Templates", "Macros"}), 1);
        add_question("True or False: std::function uses type erasure.",
                     PackedStringArray({"True", "False"}), 0);
    }
    else { // day 7
        add_question("Move ctor signature:",
                     PackedStringArray({"T(const T&)", "T(T&&)", "T& (T&)", "Void"}), 1);
        add_question("What is SSO?",
                     PackedStringArray({"Small String Optimization", "Stack Overflow", "Smart Shared Object", "None"}), 0);
        add_question("True or False: Valgrind detects double free.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("Makefile phony target:",
                     PackedStringArray({".PHONY", "Real file", "Ignore", "Delete"}), 0);
        add_question("References can't be:",
                     PackedStringArray({"Rebound", "Null", "Both", "None"}), 2);
    }
}

void QuizManager::fill_questions_hard(int day) {
    if (day == 1) {
        add_question("This code has undefined behavior: int x = 5; int& r = x; int* p = &r; delete p;",
                     PackedStringArray({"True", "False"}), 0);
        add_question("Virtual inheritance is primarily used to solve:",
                     PackedStringArray({"Multiple inheritance performance", "The diamond problem", "Memory leaks", "Name hiding"}), 1);
        add_question("True or False: std::shared_ptr is thread-safe for the control block.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("The most vexing parse interprets string s(); as:",
                     PackedStringArray({"Default-constructed string", "Function declaration", "Compile error", "Empty string"}), 1);
        add_question("Which is guaranteed by C++17?",
                     PackedStringArray({"Copy elision in all cases", "Guaranteed RVO for prvalues", "No move semantics", "No exceptions"}), 1);
    }
    else if (day == 2) {
        add_question("True or False: consteval functions can be called at runtime.",
                     PackedStringArray({"True", "False"}), 1);
        add_question("In the game loop (Lecture 16), the correct order is:",
                     PackedStringArray({"Render → Update → Input", "Input → Update → Render", "Update → Input → Render", "Render → Input → Update"}), 1);
        add_question("Conway’s Game of Life: a live cell with exactly 3 neighbors:",
                     PackedStringArray({"Dies", "Stays alive", "Becomes dead next gen", "Birth only"}), 1);
        add_question("True or False: On most embedded systems you can freely use new and delete.",
                     PackedStringArray({"True", "False"}), 1);
        add_question("The three boid steering behaviors are:",
                     PackedStringArray({"Seek, Flee, Wander", "Separation, Alignment, Cohesion", "Pursuit, Evade, Obstacle Avoidance", "Path Following, Leader Following, Queue"}), 1);
    }
    else if (day == 3) {
        add_question("In Qt, the signal/slot mechanism is processed by:",
                     PackedStringArray({"Templates", "moc (Meta-Object Compiler)", "CMake", "qmake only"}), 1);
        add_question("True or False: std::launder is needed after placement new.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("The diamond problem is solved using:",
                     PackedStringArray({"final", "override", "virtual inheritance", "protected"}), 2);
        add_question("True or False: std::weak_ptr can be dereferenced directly.",
                     PackedStringArray({"True", "False (must lock first)"}), 1);
        add_question("CRTP is for:",
                     PackedStringArray({"Dynamic poly", "Static polymorphism", "Memory", "Threads"}), 1);
    }
    else if (day == 4) {
        add_question("True or False: SFINAE enables concepts pre-C++20.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("EBO saves space in:",
                     PackedStringArray({"Empty bases", "Full classes", "Pointers", "None"}), 0);
        add_question("typeid is part of:",
                     PackedStringArray({"RTTI", "Static", "Compile", "Run only"}), 0);
        add_question("True or False: Multiple inheritance always bad.",
                     PackedStringArray({"True", "False (useful but risky)"}), 1);
        add_question("std::forward for:",
                     PackedStringArray({"Move only", "Perfect forwarding", "Copy", "None"}), 1);
    }
    else if (day == 5) {
        add_question("Universal reference is:",
                     PackedStringArray({"T&", "T&& (templated)", "int&", "Void"}), 1);
        add_question("Vtable is:",
                     PackedStringArray({"Static", "Per-class function table", "Heap", "Stack"}), 1);
        add_question("True or False: Strict aliasing violation = UB.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("CMake vs Makefile:",
                     PackedStringArray({"CMake generates Makefiles", "Both same", "Makefile only", "None"}), 0);
        add_question("True or False: Bare-metal has no OS.",
                     PackedStringArray({"True", "False"}), 0);
    }
    else if (day == 6) {
        add_question("True or False: Heap fragmentation kills embedded.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("consteval vs constexpr:",
                     PackedStringArray({"Same", "Consteval only compile-time", "Runtime", "None"}), 1);
        add_question("True or False: std::any erases type.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("True or False: Weak ptr breaks cycles.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("True or False: final on class prevents inheritance.",
                     PackedStringArray({"True", "False"}), 0);
    }
    else { // day 7
        add_question("Boid cohesion:",
                     PackedStringArray({"Avoid", "Steer toward average", "Match velocity", "Separate"}), 1);
        add_question("What is \"as-if\" rule?",
                     PackedStringArray({"Optimization freedom", "Strict", "No UB", "None"}), 0);
        add_question("True or False: shared_ptr control block on heap.",
                     PackedStringArray({"True", "False"}), 0);
        add_question("Multiple inheritance diamond fixed by:",
                     PackedStringArray({"Virtual base", "Private", "Protected", "None"}), 0);
        add_question("True or False: Valgrind \"possibly lost\" means reachable but suspicious.",
                     PackedStringArray({"True", "False"}), 0);
    }
}

void QuizManager::load_questions_for_day(int difficulty, int day) {
    reset();

    switch (difficulty) {
        case 0:
            fill_questions_easy(day);
            break;
        case 1:
            fill_questions_mid(day);
            break;
        case 2:
        default:
            fill_questions_hard(day);
            break;
    }

    if (questions.size() > 0) {
        current_index = 0;
    } else {
        current_index = -1;
    }
}

Dictionary QuizManager::get_next_question() {
    Dictionary result;

    if (current_index < 0 || current_index >= questions.size()) {
        return result; // empty = no more questions
    }

    Dictionary q = questions[current_index];

    // Match what UI.gd expects
    result["question"] = q["text"];          
    result["choices"]  = q["choices"];
    // Optional: forward correct index if you ever want it on the GDScript side
    result["correct_index"] = q["correct_index"];

    return result;
}

bool QuizManager::answer_current_question(int choice_index) {
    if (current_index < 0 || current_index >= questions.size()) {
        return false;
    }

    Dictionary q = questions[current_index];
    int correct_index = (int)q["correct_index"];

    bool correct = (choice_index == correct_index);

    // Move to next question
    current_index++;
    if (current_index >= questions.size()) {
        current_index = questions.size(); // locked at end
    }

    return correct;
}

bool QuizManager::has_more_questions() const {
    return (current_index >= 0) && (current_index < questions.size());
}
