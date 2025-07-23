#include "Goal.h"
#include <sstream>
#include <iomanip>

void GoalManager::addGoal(const std::string& description, const std::string& date, GoalStatus status) {
    goals.push_back({ description, date, status });
}

void GoalManager::markGoalCompleted(size_t idx) {
    if (idx < goals.size())
        goals[idx].status = GoalStatus::Completed;
}

void GoalManager::markGoalInProgress(size_t idx) {
    if (idx < goals.size())
        goals[idx].status = GoalStatus::InProgress;
}

void GoalManager::removeGoal(size_t idx) {
    if (idx < goals.size())
        goals.erase(goals.begin() + idx);
}

void GoalManager::editGoal(size_t idx, const std::string& newDescription) {
    if (idx < goals.size())
        goals[idx].description = newDescription;
}


size_t GoalManager::getGoalCount() const {
    return goals.size();
}

const Goal* GoalManager::getGoal(size_t idx) const {
    if (idx < goals.size())
        return &goals[idx];
    return nullptr;
}

void GoalManager::loadFromUser(const std::string& rawData) {
    goals.clear();
    std::istringstream iss(rawData);
    std::string line;
    while (std::getline(iss, line)) {
        std::istringstream ls(line);
        std::string date, desc, statusStr;
        getline(ls, date, '|');
        getline(ls, desc, '|');
        getline(ls, statusStr, '|');
        GoalStatus status = GoalStatus::NotStarted;
        if (statusStr == "completed") status = GoalStatus::Completed;
        else if (statusStr == "inprogress") status = GoalStatus::InProgress;
        goals.push_back({ desc, date, status });
    }
}

std::string GoalManager::serializeForUser() const {
    std::ostringstream oss;
    for (const auto& g : goals) {
        std::string stat = "notstarted";
        if (g.status == GoalStatus::Completed) stat = "completed";
        else if (g.status == GoalStatus::InProgress) stat = "inprogress";
        oss << g.date << "|" << g.description << "|" << stat << "\n";
    }
    return oss.str();
}