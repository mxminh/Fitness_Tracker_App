#ifndef FITNESSAPP_H
#define FITNESSAPP_H

#include "User.h"
#include "Workout.h"
#include "Nutrition.h"
#include "Goal.h"
#include <string>

class FitnessApp {
private:
    User* currentUser;
    Workout workoutManager;
    Nutrition nutritionManager;
    GoalManager goalManager;

    void registerUser();
    void loginUser();
    void deleteUser();
    void renameUser();

    void userMenu();

    void logWorkout();
    void logCardio();
    void logNutrition();

    void viewProgress();
    void viewCalendar();

    void manageGoals();
    void editRemoveGoals();
    void editRemoveWorkouts();

    std::string getTodayDateString() const;
    std::string askDateString(const std::string& prompt);

    bool checkExit(const std::string& input);

public:
    FitnessApp();
    ~FitnessApp();

    void run();
};

#endif