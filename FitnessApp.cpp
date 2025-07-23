#include "FitnessApp.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include <limits>
#include <filesystem>

// COLOR AND STYLES
#define COLOR_TITLE      "\033[1;38;5;200m"
#define COLOR_MENU       "\033[3;38;5;208m"
#define COLOR_INPUT      "\033[3;38;5;120m"
#define COLOR_SUCCESS    "\033[1;92m"
#define COLOR_FAILURE    "\033[1;91m"
#define COLOR_WARN       "\033[1;93m"
#define COLOR_RESET      "\033[0m"
#define COLOR_ID         "\033[1;38;5;199m"
#define COLOR_GOAL_RED   "\033[1;91m"
#define COLOR_GOAL_YELLOW "\033[1;93m"
#define COLOR_GOAL_GREEN "\033[1;92m"
#define COLOR_WORKOUT_HEADER "\033[1;38;5;220m"
#define COLOR_WORKOUT_ROW    "\033[3;38;5;207m"

#define DIVIDER "\n════════════════════════════════════════════════════════════════════════\n\n"

// BOX WIDTH (about 60 characters wide)
const char* SUPER_TOP      = "╔════════════════════════════════════════════════════════════════════╗";
const char* SUPER_BOTTOM   = "╚════════════════════════════════════════════════════════════════════╝";
const char* WORKOUT_TITLE  = "║                    Weight Workouts (ID on left)                    ║";
const char* WORKOUT_HEAD   = "╠══╦═════════════╦═══════════════╦══════╦══════╦══════════╦══════════╣";
const char* WORKOUT_COL    = "║# ║ Date        ║ Name          ║ Sets ║ Reps ║ Weight   ║ Volume   ║";
const char* WORKOUT_DIV    = "╠══╬═════════════╬═══════════════╬══════╬══════╬══════════╬══════════╣";

const char* GOAL_TITLE     = "║                        Goals (ID on left)                          ║";
const char* GOAL_HEAD      = "╠══╦═════════════╦══════════════════════════════╦════════════════════╣";
const char* GOAL_COL       = "║# ║ Date        ║ Description                  ║ Status             ║";
const char* GOAL_DIV       = "╠══╬═════════════╬══════════════════════════════╬════════════════════╣";
                            
const char* SIMPLE_TITLE   = "║ %-54s                                                              ║";
const char* SIMPLE_DIV     = "╠════════════════════════════════════════════════════════════════════╣";

std::string styleGoalStatus(GoalStatus status) {
    switch (status) {
    case GoalStatus::Completed: return COLOR_GOAL_GREEN "Completed" COLOR_RESET;
    case GoalStatus::InProgress: return COLOR_GOAL_YELLOW "In Progress" COLOR_RESET;
    default: return COLOR_GOAL_RED "Not Started" COLOR_RESET;
    }
}

FitnessApp::FitnessApp() : currentUser(nullptr) {}
FitnessApp::~FitnessApp() { delete currentUser; }

bool FitnessApp::checkExit(const std::string& input) {
    return input == "exit" || input == "EXIT" || input == "Exit";
}

std::string FitnessApp::getTodayDateString() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
#if defined(_MSC_VER)
    localtime_s(&tm_now, &t_now);
#else
    tm_now = *std::localtime(&t_now);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%d");
    return oss.str();
}

std::string FitnessApp::askDateString(const std::string& prompt) {
    std::string date;
    std::cout << COLOR_INPUT << prompt << " (YYYY-MM-DD, blank for today, 'exit' to quit): " << COLOR_RESET;
    std::getline(std::cin, date);
    if (checkExit(date)) {
        std::cout << COLOR_WARN << "\nExiting...\n" << COLOR_RESET;
        exit(0);
    }
    if (date.empty()) return getTodayDateString();
    return date;
}

void FitnessApp::run() {
    while (true) {
        std::cout << DIVIDER;
        std::cout << COLOR_TITLE "====================[ Main Menu ]====================" COLOR_RESET "\n\n";
        std::cout << COLOR_MENU
            "\t[1] Register\n"
            "\t[2] Login\n"
            "\t[3] Delete User\n"
            "\t[4] Rename User\n"
            "\t[0] Exit"
            COLOR_RESET "\n> ";
        std::string choice;
        std::getline(std::cin, choice);
        if (checkExit(choice) || choice == "0") return;
        std::cout << DIVIDER;
        if (choice == "1") registerUser();
        else if (choice == "2") loginUser();
        else if (choice == "3") deleteUser();
        else if (choice == "4") renameUser();
        else std::cout << COLOR_FAILURE "Invalid option.\n" COLOR_RESET;
    }
}

void FitnessApp::registerUser() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Register New User ]====================" COLOR_RESET "\n\n";
    std::string username, password;
    std::cout << COLOR_INPUT "\tEnter new username ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, username);
    if (checkExit(username)) return;
    std::cout << COLOR_INPUT "\tEnter password: " COLOR_RESET;
    std::getline(std::cin, password);
    if (checkExit(password)) return;
    if (User::isUsernameTaken(username)) {
        std::cout << COLOR_FAILURE "Username already exists.\n" COLOR_RESET;
        return;
    }
    if (User::saveUserCredentials(username, password)) {
        std::filesystem::create_directories("data/users/" + username);
        std::cout << COLOR_SUCCESS "Registration successful!\n" COLOR_RESET;
    }
}

void FitnessApp::loginUser() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ User Login ]====================" COLOR_RESET "\n\n";
    std::string username, password;
    std::cout << COLOR_INPUT "\tUsername ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, username);
    if (checkExit(username)) return;
    std::cout << COLOR_INPUT "\tPassword: " COLOR_RESET;
    std::getline(std::cin, password);
    if (checkExit(password)) return;
    auto users = User::loadAllUsers();
    if (users.count(username) && users[username] == password) {
        delete currentUser;
        currentUser = new User(username, password);
        std::cout << COLOR_SUCCESS "\n\tLogin successful!\n" COLOR_RESET;
        userMenu();
    }
    else {
        std::cout << COLOR_FAILURE "\n\tInvalid credentials.\n" COLOR_RESET;
    }
}

void FitnessApp::deleteUser() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Delete User ]====================" COLOR_RESET "\n\n";
    std::string username;
    std::cout << COLOR_INPUT "\tEnter username to delete ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, username);
    if (checkExit(username)) return;
    if (User::deleteUserFromFile(username) && User::deleteUserFolder(username)) {
        std::cout << COLOR_SUCCESS "User deleted.\n" COLOR_RESET;
    }
    else {
        std::cout << COLOR_FAILURE "Failed to delete user.\n" COLOR_RESET;
    }
}

void FitnessApp::renameUser() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Rename User ]====================" COLOR_RESET "\n\n";
    std::string oldUsername, newUsername;
    std::cout << COLOR_INPUT "\tCurrent username ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, oldUsername);
    if (checkExit(oldUsername)) return;
    std::cout << COLOR_INPUT "\tNew username: " COLOR_RESET;
    std::getline(std::cin, newUsername);
    if (checkExit(newUsername)) return;
    if (User::renameUser(oldUsername, newUsername)) {
        std::cout << COLOR_SUCCESS "User renamed successfully.\n" COLOR_RESET;
    }
    else {
        std::cout << COLOR_FAILURE "Failed to rename user.\n" COLOR_RESET;
    }
}

void FitnessApp::userMenu() {
    workoutManager.loadFromUser(currentUser->loadUserData("workout.txt"));
    goalManager.loadFromUser(currentUser->loadUserData("goal.txt"));
    while (true) {
        std::cout << DIVIDER;
        std::cout << COLOR_TITLE "====================[ User Menu ]====================" COLOR_RESET "\n\n";
        std::cout << COLOR_MENU
            "\t[1] Log Weight Workout\n"
            "\t[2] Log Cardio\n"
            "\t[3] Log Nutrition\n"
            "\t[4] View Progress\n"
            "\t[5] Manage Goals\n"
            "\t[6] Calendar View\n"
            "\t[7] Edit/Remove Workouts\n"
            "\t[8] Edit/Remove Goals\n"
            "\t[0] Logout"
            COLOR_RESET "\n> ";
        std::string choice;
        std::getline(std::cin, choice);
        if (checkExit(choice) || choice == "0") {
            currentUser->saveUserData("workout.txt", workoutManager.serializeForUser());
            currentUser->saveUserData("goal.txt", goalManager.serializeForUser());
            return;
        }
        std::cout << DIVIDER;
        if (choice == "1") logWorkout();
        else if (choice == "2") logCardio();
        else if (choice == "3") logNutrition();
        else if (choice == "4") viewProgress();
        else if (choice == "5") manageGoals();
        else if (choice == "6") viewCalendar();
        else if (choice == "7") editRemoveWorkouts();
        else if (choice == "8") editRemoveGoals();
        else std::cout << COLOR_FAILURE "Invalid option.\n" COLOR_RESET;
    }
}

void FitnessApp::logWorkout() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Log Weight Workout ]====================" COLOR_RESET "\n\n";
    std::string date = askDateString("Enter workout date");
    std::string name;
    std::cout << COLOR_INPUT "\tWorkout name ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, name);
    if (checkExit(name)) return;
    int sets = 0, reps = 0;
    double weight = 0.0;
    std::cout << COLOR_INPUT "\tSets: " COLOR_RESET;
    std::string setsStr; std::getline(std::cin, setsStr);
    if (checkExit(setsStr)) return;
    std::istringstream(setsStr) >> sets;
    std::cout << COLOR_INPUT "\tReps per set: " COLOR_RESET;
    std::string repsStr; std::getline(std::cin, repsStr);
    if (checkExit(repsStr)) return;
    std::istringstream(repsStr) >> reps;
    std::cout << COLOR_INPUT "\tWeight lifted (lbs): " COLOR_RESET;
    std::string weightStr; std::getline(std::cin, weightStr);
    if (checkExit(weightStr)) return;
    std::istringstream(weightStr) >> weight;
    workoutManager.addWorkout(name, sets, reps, weight, date);
    currentUser->saveUserData("workout.txt", workoutManager.serializeForUser());
    std::cout << COLOR_SUCCESS "> Workout saved!\n" COLOR_RESET;
}

void FitnessApp::logCardio() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Log Cardio ]====================" COLOR_RESET "\n\n";
    std::string date = askDateString("Enter cardio date");
    std::string name;
    std::cout << COLOR_INPUT "\tCardio type ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, name);
    if (checkExit(name)) return;
    int duration = 0;
    double calories = 0.0;
    std::cout << COLOR_INPUT "\tDuration (minutes): " COLOR_RESET;
    std::string durationStr; std::getline(std::cin, durationStr);
    if (checkExit(durationStr)) return;
    std::istringstream(durationStr) >> duration;
    std::cout << COLOR_INPUT "\tCalories burned: " COLOR_RESET;
    std::string caloriesStr; std::getline(std::cin, caloriesStr);
    if (checkExit(caloriesStr)) return;
    std::istringstream(caloriesStr) >> calories;
    std::stringstream log;
    log << date << " | " << name << " | Duration: " << duration << " min | Calories: " << calories;
    currentUser->saveUserData("cardio.txt", log.str());
    std::cout << COLOR_SUCCESS "> Cardio log saved!\n" COLOR_RESET;
}

void FitnessApp::logNutrition() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Log Nutrition ]====================" COLOR_RESET "\n\n";
    std::string date = askDateString("Enter nutrition date");
    std::string food;
    std::cout << COLOR_INPUT "\tFood eaten ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, food);
    if (checkExit(food)) return;
    double protein = 0.0, carbs = 0.0, fat = 0.0, cal = 0.0;
    std::cout << COLOR_INPUT "\tProtein (g): " COLOR_RESET;
    std::string proteinStr; std::getline(std::cin, proteinStr);
    if (checkExit(proteinStr)) return;
    std::istringstream(proteinStr) >> protein;
    std::cout << COLOR_INPUT "\tCarbs (g): " COLOR_RESET;
    std::string carbsStr; std::getline(std::cin, carbsStr);
    if (checkExit(carbsStr)) return;
    std::istringstream(carbsStr) >> carbs;
    std::cout << COLOR_INPUT "\tFat (g): " COLOR_RESET;
    std::string fatStr; std::getline(std::cin, fatStr);
    if (checkExit(fatStr)) return;
    std::istringstream(fatStr) >> fat;
    std::cout << COLOR_INPUT "\tCalories: " COLOR_RESET;
    std::string calStr; std::getline(std::cin, calStr);
    if (checkExit(calStr)) return;
    std::istringstream(calStr) >> cal;
    std::stringstream log;
    log << date << " | " << food << " | Protein: " << protein << "g | Carbs: " << carbs << "g | Fat: " << fat << "g | Calories: " << cal;
    currentUser->saveUserData("nutrition.txt", log.str());
    std::cout << COLOR_SUCCESS "> Nutrition log saved!\n" COLOR_RESET;
}

void FitnessApp::viewProgress() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Your Progress ]====================" COLOR_RESET "\n\n";
    // Workout Table
    std::cout << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_TITLE << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_HEAD << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_COL << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_DIV << COLOR_RESET << "\n";
    for (size_t i = 0; i < workoutManager.getWorkoutCount(); ++i) {
        const WorkoutEntry* e = workoutManager.getWorkout(i);
        double totalVol = e->weight * e->reps * e->sets;
        std::cout << COLOR_GOAL_YELLOW << "║"
            << std::setw(2) << (i + 1) << COLOR_GOAL_YELLOW << "║"
            << std::setw(13) << e->date << "║"
            << std::setw(15) << e->name.substr(0, 15) << "║"
            << std::setw(6) << e->sets << "║"
            << std::setw(6) << e->reps << "║"
            << std::setw(10) << std::fixed << std::setprecision(2) << e->weight << "║"
            << std::setw(10) << std::fixed << std::setprecision(2) << totalVol << "║"
            << COLOR_RESET << "\n";
    }
	std::cout << COLOR_GOAL_YELLOW << "╚══╩═════════════╩═══════════════╩══════╩══════╩══════════╩══════════╝" << COLOR_RESET << "\n\n";

    // ----------- CARDIO TABLE ------------
    std::cout << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << "║                       Cardio Sessions                              ║\n";
    std::cout << COLOR_GOAL_YELLOW << "╠══╦═════════════╦══════════════════╦═══════════╦════════════════════╣\n";
    std::cout << COLOR_GOAL_YELLOW << "║# ║    Date     ║      Name        ║ Duration  ║ Calories Burned    ║\n";
    std::cout << COLOR_GOAL_YELLOW << "╠══╬═════════════╬══════════════════╬═══════════╬════════════════════╣\n";

    std::istringstream cardioStream(currentUser->loadUserData("cardio.txt"));
    std::string cardioLine;
    int cardioIndex = 1;
    while (std::getline(cardioStream, cardioLine)) {
        std::istringstream lineStream(cardioLine);
        std::string date, name, durationStr, caloriesStr;

        std::getline(lineStream, date, '|');
        std::getline(lineStream, name, '|');
        std::getline(lineStream, durationStr, '|');
        std::getline(lineStream, caloriesStr, '|');

        // Cleanup whitespace
        auto trim = [](std::string& s) {
            size_t start = s.find_first_not_of(" ");
            size_t end = s.find_last_not_of(" ");
            s = (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
            };

        trim(date); trim(name); trim(durationStr); trim(caloriesStr);

        std::cout << COLOR_GOAL_YELLOW << "║"
            << std::setw(2) << cardioIndex++ << "║"
            << std::setw(13) << date << "║"
            << std::setw(17) << name.substr(0, 17) << " ║"
            << std::setw(11) << durationStr.substr(durationStr.find(":") + 1) << "║"
            << std::setw(20) << caloriesStr.substr(caloriesStr.find(":") + 1) << "║"
            << COLOR_RESET << "\n";
    }
    std::cout << COLOR_GOAL_YELLOW << "╚══╩═════════════╩══════════════════╩═══════════╩════════════════════╝" << COLOR_RESET << "\n\n";

    auto trim = [](std::string& s) {
        size_t start = s.find_first_not_of(" ");
        size_t end = s.find_last_not_of(" ");
        if (start == std::string::npos) {
            s = "";
        } else {
            s = s.substr(start, end - start + 1);
        }
    };
    
    // ----------- NUTRITION TABLE ------------
    std::cout << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << "║                       Nutrition Intake                             ║\n";
    std::cout << COLOR_GOAL_YELLOW << "╠══╦═════════════╦════════════════╦═════════╦═══════╦═════╦══════════╣\n";
    std::cout << COLOR_GOAL_YELLOW << "║# ║    Date     ║      Name      ║ Protein ║ Carbs ║ Fat ║ Calories ║\n";
    std::cout << COLOR_GOAL_YELLOW << "╠══╬═════════════╬════════════════╬═════════╬═══════╬═════╬══════════╣\n";

    std::istringstream nutritionStream(currentUser->loadUserData("nutrition.txt"));
    std::string nutritionLine;
    int nutritionIndex = 1;
    while (std::getline(nutritionStream, nutritionLine)) {
        std::istringstream lineStream(nutritionLine);
        std::string date, name, proteinStr, carbsStr, fatStr, calStr;

        std::getline(lineStream, date, '|');
        std::getline(lineStream, name, '|');
        std::getline(lineStream, proteinStr, '|');
        std::getline(lineStream, carbsStr, '|');
        std::getline(lineStream, fatStr, '|');
        std::getline(lineStream, calStr, '|');

        trim(date); trim(name); trim(proteinStr);
        trim(carbsStr); trim(fatStr); trim(calStr);

        std::cout << COLOR_GOAL_YELLOW << "║"
            << std::setw(2) << nutritionIndex++ << "║"
            << std::setw(13) << date << "║"
            << std::setw(16) << name.substr(0, 15) << "║"
            << std::setw(9) << proteinStr.substr(proteinStr.find(":") + 1) << "║"
            << std::setw(7) << carbsStr.substr(carbsStr.find(":") + 1) << "║"
            << std::setw(5) << fatStr.substr(fatStr.find(":") + 1) << "║"
            << std::setw(10) << calStr.substr(calStr.find(":") + 1) << "║"
            << COLOR_RESET << "\n";
    }
    std::cout << COLOR_GOAL_YELLOW << "╚══╩═════════════╩════════════════╩═════════╩═══════╩═════╩══════════╝" << COLOR_RESET << "\n\n";
    // Goals Table
    std::cout << COLOR_WORKOUT_HEADER << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_TITLE << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_HEAD << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_COL << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_DIV << COLOR_RESET << "\n";
    for (size_t i = 0; i < goalManager.getGoalCount(); ++i) {
        const Goal* g = goalManager.getGoal(i);

        std::string message = styleGoalStatus(g->status);
        short int spaces = 11;

        if (message == COLOR_GOAL_GREEN "Completed" COLOR_RESET) {
            spaces = 13;
        }
        std::cout << COLOR_WORKOUT_HEADER << "║"
            << std::setw(2) << (i + 1) << COLOR_WORKOUT_HEADER << "║"
            << std::setw(12) << g->date << " ║"
            << std::setw(30) << g->description.substr(0, 29) << "║ "
            << std::setw(1) << COLOR_RESET << styleGoalStatus(g->status) << std::setw(5) << COLOR_WORKOUT_HEADER << std::setw(spaces) << std::setfill(' ') << "║"
            << COLOR_RESET << "\n";
    }
    std::cout << COLOR_WORKOUT_HEADER << "╚══╩═════════════╩══════════════════════════════╩════════════════════╝" << COLOR_RESET << "\n\n";
}

void FitnessApp::manageGoals() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Manage Goals ]====================" COLOR_RESET "\n\n";
    std::cout << COLOR_WORKOUT_HEADER << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_TITLE << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_HEAD << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_COL << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_DIV << COLOR_RESET << "\n";
    
	// Display Goals
    for (size_t i = 0; i < goalManager.getGoalCount(); ++i) {
        const Goal* g = goalManager.getGoal(i);

		std::string message = styleGoalStatus(g->status);
        short int spaces = 11;

        if (message == COLOR_GOAL_GREEN "Completed" COLOR_RESET) {
            spaces = 13;
        }
        std::cout << COLOR_WORKOUT_HEADER << "║"
            << std::setw(2) << (i + 1) << COLOR_WORKOUT_HEADER << "║"
            << std::setw(12) << g->date << " ║"
            << std::setw(30) << g->description.substr(0, 29) << "║ "
            << std::setw(1) << COLOR_RESET << styleGoalStatus(g->status) << std::setw(5) << COLOR_WORKOUT_HEADER << std::setw(spaces) << std::setfill(' ') << "║"
            << COLOR_RESET << "\n";
    }
	std::cout << COLOR_WORKOUT_HEADER << "╚══╩═════════════╩══════════════════════════════╩════════════════════╝" << COLOR_RESET << "\n\n";

    std::cout << COLOR_MENU
        "\t[1] Add Goal\n"
        "\t[2] Mark Goal as Done\n"
        "\t[3] Edit Goal Name\n"
        "\t[4] Mark Goal as In Progress\n"
        "\t[5] Remove Goal\n"
        "\t[0] Return"
        COLOR_RESET "\n> ";
    std::string choice;
    std::getline(std::cin, choice);
    if (checkExit(choice) || choice == "0") return;
    if (choice == "1") {
        std::string goal;
        std::cout << COLOR_INPUT "\tEnter your goal ('exit' to quit): " COLOR_RESET;
        std::getline(std::cin, goal);
        if (checkExit(goal)) return;
        goalManager.addGoal(goal, getTodayDateString(), GoalStatus::NotStarted);
        currentUser->saveUserData("goal.txt", goalManager.serializeForUser());
        std::cout << COLOR_SUCCESS "Goal added!\n" COLOR_RESET;
    }
    else if (choice == "2" || choice == "3" || choice == "4" || choice == "5") {
        if (goalManager.getGoalCount() == 0) {
            std::cout << COLOR_WARN "No goals to edit.\n" COLOR_RESET;
            return;
        }
        std::cout << COLOR_INPUT "\tEnter goal number: " COLOR_RESET;
        std::string idxStr;
        std::getline(std::cin, idxStr);
        if (checkExit(idxStr)) return;
        size_t idx = std::stoi(idxStr);
        if (idx < 1 || idx > goalManager.getGoalCount()) {
            std::cout << COLOR_FAILURE "Invalid goal number.\n" COLOR_RESET;
            return;
        }
        --idx;
        if (choice == "2") {
            goalManager.markGoalCompleted(idx);
            std::cout << COLOR_SUCCESS "Goal marked as completed.\n" COLOR_RESET;
        }
        else if (choice == "3") {
            std::cout << COLOR_INPUT "\tEnter new goal name: " COLOR_RESET;
            std::string newName;
            std::getline(std::cin, newName);
            if (checkExit(newName)) return;
            goalManager.editGoal(idx, newName);
            std::cout << COLOR_SUCCESS "Goal updated.\n" COLOR_RESET;
        }
        else if (choice == "4") {
            goalManager.markGoalInProgress(idx);
            std::cout << COLOR_SUCCESS "Goal marked as in progress.\n" COLOR_RESET;
        }
        else if (choice == "5") {
            goalManager.removeGoal(idx);
            std::cout << COLOR_SUCCESS "Goal removed.\n" COLOR_RESET;
        }
        currentUser->saveUserData("goal.txt", goalManager.serializeForUser());
    }
    else {
        std::cout << COLOR_FAILURE "Invalid option.\n" COLOR_RESET;
    }
}

void FitnessApp::viewCalendar() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Calendar View ]====================" COLOR_RESET "\n\n";
    std::string period;
    std::cout << COLOR_INPUT "\tView progress by [week/month/custom] ('exit' to quit): " COLOR_RESET;
    std::getline(std::cin, period);
    if (checkExit(period)) return;
    std::string from, to;
    if (period == "week" || period == "month") {
        std::string today = getTodayDateString();
        std::tm tm = {};
        std::istringstream ss(today);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        if (period == "week") {
            time_t t = mktime(&tm) - 6 * 24 * 3600;
#if defined(_MSC_VER)
            std::tm tm_from;
            localtime_s(&tm_from, &t);
#else
            std::tm tm_from = *std::localtime(&t);
#endif
            std::stringstream sfrom;
            sfrom << std::put_time(&tm_from, "%Y-%m-%d");
            from = sfrom.str();
            to = today;
        }
        else {
            tm.tm_mday = 1;
            std::stringstream sfrom;
            sfrom << std::put_time(&tm, "%Y-%m-%d");
            from = sfrom.str();
            to = today;
        }
    }
    else if (period == "custom") {
        std::cout << COLOR_INPUT "\tEnter start date (YYYY-MM-DD): " COLOR_RESET;
        std::getline(std::cin, from);
        if (checkExit(from)) return;
        std::cout << COLOR_INPUT "\tEnter end date (YYYY-MM-DD): " COLOR_RESET;
        std::getline(std::cin, to);
        if (checkExit(to)) return;
    }
    else {
        std::cout << COLOR_FAILURE "Invalid period.\n" COLOR_RESET;
        return;
    }
    // Calendar View: Workouts
    std::cout << std::endl << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << "║                        Calendar View: Workouts                     ║";
    std::cout << COLOR_GOAL_YELLOW << "\n" << SIMPLE_DIV << COLOR_RESET << "\n";
    std::istringstream ws(currentUser->loadUserData("workout.txt"));
    std::string line;
    size_t count = 0;
    while (getline(ws, line)) {
        if (line.substr(0, from.size()) >= from && line.substr(0, to.size()) <= to)
            std::cout << COLOR_GOAL_YELLOW << "║" << std::setw(2) << ++count << ". " << line << "\n";
    }
    std::cout << COLOR_GOAL_YELLOW << SUPER_BOTTOM << COLOR_RESET << "\n\n";
    // Calendar View: Cardio
    std::cout << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << "║                        Calendar View: Cardio                       ║";
    std::cout << COLOR_GOAL_YELLOW << "\n" << SIMPLE_DIV << COLOR_RESET << "\n";
    std::istringstream cs(currentUser->loadUserData("cardio.txt"));
    count = 0;
    while (getline(cs, line)) {
        if (line.substr(0, from.size()) >= from && line.substr(0, to.size()) <= to)
            std::cout << COLOR_GOAL_YELLOW << "║" << std::setw(2) << ++count << ". " << line << "\n";
    }
    std::cout << COLOR_GOAL_YELLOW << SUPER_BOTTOM << COLOR_RESET << "\n\n";
    // Calendar View: Nutrition
    std::cout << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << "║                        Calendar View: Nutrition                    ║";
    std::cout << COLOR_GOAL_YELLOW << "\n" << SIMPLE_DIV << COLOR_RESET << "\n";
    std::istringstream ns(currentUser->loadUserData("nutrition.txt"));
    count = 0;
    while (getline(ns, line)) {
        if (line.substr(0, from.size()) >= from && line.substr(0, to.size()) <= to)
            std::cout << COLOR_GOAL_YELLOW << "║" << std::setw(2) << ++count << ". " << line << "\n";
    }
    std::cout << COLOR_GOAL_YELLOW << SUPER_BOTTOM << COLOR_RESET << "\n\n";
    // Calendar View: Goals
    std::cout << COLOR_WORKOUT_HEADER << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << "║                        Calendar View: Goals                        ║";
    std::cout << COLOR_WORKOUT_HEADER << "\n" << SIMPLE_DIV << COLOR_RESET << "\n";
    std::istringstream gs(currentUser->loadUserData("goal.txt"));
    count = 0;
    while (getline(gs, line)) {
        if (line.substr(0, from.size()) >= from && line.substr(0, to.size()) <= to)
            std::cout << COLOR_WORKOUT_HEADER << "║" << std::setw(2) << ++count << ". " << line << "\n";
    }
    std::cout << COLOR_WORKOUT_HEADER << SUPER_BOTTOM << COLOR_RESET << "\n\n";
}

void FitnessApp::editRemoveWorkouts() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Edit/Remove Workouts ]====================" COLOR_RESET "\n\n";
    // Workout Table
    std::cout << COLOR_GOAL_YELLOW << SUPER_TOP << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << "║                        Edit/Remove Workout                         ║\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_HEAD << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_COL << COLOR_RESET << "\n";
    std::cout << COLOR_GOAL_YELLOW << WORKOUT_DIV << COLOR_RESET << "\n";
    for (size_t i = 0; i < workoutManager.getWorkoutCount(); ++i) {
        const WorkoutEntry* e = workoutManager.getWorkout(i);
        double totalVol = e->weight * e->reps * e->sets;
        std::cout << COLOR_GOAL_YELLOW << "║"
            << std::setw(2) << (i + 1) << COLOR_GOAL_YELLOW << "║"
            << std::setw(13) << e->date << "║"
            << std::setw(15) << e->name.substr(0, 15) << "║"
            << std::setw(6) << e->sets << "║"
            << std::setw(6) << e->reps << "║"
            << std::setw(10) << std::fixed << std::setprecision(2) << e->weight << "║"
            << std::setw(10) << std::fixed << std::setprecision(2) << totalVol << "║"
            << COLOR_RESET << "\n";
    }
    std::cout << COLOR_GOAL_YELLOW << "╚══╩═════════════╩═══════════════╩══════╩══════╩══════════╩══════════╝" << COLOR_RESET << "\n";
    if (workoutManager.getWorkoutCount() == 0) {
        std::cout << COLOR_WARN "\tNo workouts to edit/remove.\n" COLOR_RESET;
        return;
    }
    std::cout << COLOR_MENU
        "\n\t[1] Edit Workout\n"
        "\t[2] Remove Workout\n"
        "\t[0] Return"
        COLOR_RESET "\n> ";
    std::string choice;
    std::getline(std::cin, choice);
    if (checkExit(choice) || choice == "0") return;
    std::cout << COLOR_INPUT "\tEnter workout number: " COLOR_RESET;
    std::string idxStr;
    std::getline(std::cin, idxStr);
    if (checkExit(idxStr)) return;
    size_t idx = std::stoi(idxStr);
    if (idx < 1 || idx > workoutManager.getWorkoutCount()) {
        std::cout << COLOR_FAILURE "\tInvalid workout number.\n" COLOR_RESET;
        return;
    }
    --idx;
    if (choice == "1") {
        const WorkoutEntry* entry = workoutManager.getWorkout(idx);
        if (!entry) return;
        std::string name;
        int sets, reps;
        double weight;
        std::string date;
        std::cout << COLOR_INPUT "\tNew name (" << entry->name << "): " COLOR_RESET;
        std::getline(std::cin, name);
        if (checkExit(name)) return;
        if (name.empty()) name = entry->name;
        std::cout << COLOR_INPUT "\tSets (" << entry->sets << "): " COLOR_RESET;
        std::string setsStr; std::getline(std::cin, setsStr);
        if (checkExit(setsStr)) return;
        sets = setsStr.empty() ? entry->sets : std::stoi(setsStr);
        std::cout << COLOR_INPUT "\tReps (" << entry->reps << "): " COLOR_RESET;
        std::string repsStr; std::getline(std::cin, repsStr);
        if (checkExit(repsStr)) return;
        reps = repsStr.empty() ? entry->reps : std::stoi(repsStr);
        std::cout << COLOR_INPUT "\tWeight (" << entry->weight << "): " COLOR_RESET;
        std::string weightStr; std::getline(std::cin, weightStr);
        if (checkExit(weightStr)) return;
        weight = weightStr.empty() ? entry->weight : std::stod(weightStr);
        std::cout << COLOR_INPUT "\tDate (" << entry->date << "): " COLOR_RESET;
        std::getline(std::cin, date);
        if (checkExit(date)) return;
        if (date.empty()) date = entry->date;
        workoutManager.editWorkout(idx, { name, sets, reps, weight, date });
        currentUser->saveUserData("workout.txt", workoutManager.serializeForUser());
        std::cout << COLOR_SUCCESS "Workout updated!\n" COLOR_RESET;
    }
    else if (choice == "2") {
        workoutManager.removeWorkout(idx);
        currentUser->saveUserData("workout.txt", workoutManager.serializeForUser());
        std::cout << COLOR_SUCCESS "Workout removed!\n" COLOR_RESET;
    }
    else {
        std::cout << COLOR_FAILURE "\tInvalid option.\n" COLOR_RESET;
    }
}

void FitnessApp::editRemoveGoals() {
    std::cout << DIVIDER;
    std::cout << COLOR_TITLE "====================[ Edit/Remove Goals ]====================" COLOR_RESET "\n\n";

    // Goals Table
    std::cout << COLOR_WORKOUT_HEADER << SUPER_TOP << COLOR_RESET << "\n";
	std::cout << COLOR_WORKOUT_HEADER << "║                        Edit/Remove Goal                            ║\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_HEAD << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_COL << COLOR_RESET << "\n";
    std::cout << COLOR_WORKOUT_HEADER << GOAL_DIV << COLOR_RESET << "\n";
    for (size_t i = 0; i < goalManager.getGoalCount(); ++i) {
        const Goal* g = goalManager.getGoal(i);

        std::string message = styleGoalStatus(g->status);
        short int spaces = 11;

        if (message == COLOR_GOAL_GREEN "Completed" COLOR_RESET) {
            spaces = 13;
        }
        std::cout << COLOR_WORKOUT_HEADER << "║"
            << std::setw(2) << (i + 1) << COLOR_WORKOUT_HEADER << "║"
            << std::setw(12) << g->date << " ║"
            << std::setw(30) << g->description.substr(0, 29) << "║ "
            << std::setw(1) << COLOR_RESET << styleGoalStatus(g->status) << std::setw(5) << COLOR_WORKOUT_HEADER << std::setw(spaces) << std::setfill(' ') << "║"
            << COLOR_RESET << "\n";
    }
    std::cout << COLOR_WORKOUT_HEADER << "╚══╩═════════════╩══════════════════════════════╩════════════════════╝" << COLOR_RESET << "\n\n";

    if (goalManager.getGoalCount() == 0) {
        std::cout << COLOR_WARN "\tNo goals to edit/remove.\n" COLOR_RESET;
        return;
    }
    std::cout << COLOR_MENU
        "\t[1] Edit Goal Name\n"
        "\t[2] Remove Goal\n"
        "\t[0] Return"
        COLOR_RESET "\n> ";
    std::string choice;
    std::getline(std::cin, choice);
    if (checkExit(choice) || choice == "0") return;
    std::cout << COLOR_INPUT "\tEnter goal number: " COLOR_RESET;
    std::string idxStr;
    std::getline(std::cin, idxStr);
    if (checkExit(idxStr)) return;
    size_t idx = std::stoi(idxStr);
    if (idx < 1 || idx > goalManager.getGoalCount()) {
        std::cout << COLOR_FAILURE "\tInvalid goal number.\n" COLOR_RESET;
        return;
    }   
    --idx;
    if (choice == "1") {
        std::cout << COLOR_INPUT "\tEnter new goal name: " COLOR_RESET;
        std::string newName;
        std::getline(std::cin, newName);
        if (checkExit(newName)) return;
        goalManager.editGoal(idx, newName);
        currentUser->saveUserData("goal.txt", goalManager.serializeForUser());
        std::cout << COLOR_SUCCESS "Goal updated.\n" COLOR_RESET;
    }
    else if (choice == "2") {
        goalManager.removeGoal(idx);
        currentUser->saveUserData("goal.txt", goalManager.serializeForUser());
        std::cout << COLOR_SUCCESS "Goal removed.\n" COLOR_RESET;
    }
    else {
        std::cout << COLOR_FAILURE "\tInvalid option.\n" COLOR_RESET;
    }
}