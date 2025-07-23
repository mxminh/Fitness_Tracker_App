#ifndef GOAL_H
#define GOAL_H

#include <string>
#include <vector>

enum class GoalStatus { NotStarted, InProgress, Completed };

struct Goal {
    std::string description;
    std::string date;
    GoalStatus status;
};

class GoalManager {
private:
    std::vector<Goal> goals;

public:
    void addGoal(const std::string& description, const std::string& date, GoalStatus status = GoalStatus::NotStarted);
    void markGoalCompleted(size_t idx);
    void markGoalInProgress(size_t idx);
    void removeGoal(size_t idx);
    void editGoal(size_t idx, const std::string& newDescription);
    size_t getGoalCount() const;
    const Goal* getGoal(size_t idx) const;

    void loadFromUser(const std::string& rawData);
    std::string serializeForUser() const;
};

#endif