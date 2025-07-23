#include "Workout.h"
#include <sstream>
#include <iomanip>

void Workout::addWorkout(const std::string& name, int sets, int reps, double weight, const std::string& date) {
    entries.push_back({ name, sets, reps, weight, date });
}

void Workout::removeWorkout(size_t idx) {
    if (idx < entries.size())
        entries.erase(entries.begin() + idx);
}

void Workout::editWorkout(size_t idx, const WorkoutEntry& newEntry) {
    if (idx < entries.size())
        entries[idx] = newEntry;
}

size_t Workout::getWorkoutCount() const {
    return entries.size();
}

const WorkoutEntry* Workout::getWorkout(size_t idx) const {
    if (idx < entries.size())
        return &entries[idx];
    return nullptr;
}

void Workout::loadFromUser(const std::string& rawData) {
    entries.clear();
    std::istringstream iss(rawData);
    std::string line;
    while (std::getline(iss, line)) {
        std::istringstream ls(line);
        std::string date, name, tmp;
        int sets = 0, reps = 0;
        double weight = 0.0;
        getline(ls, date, '|');
        getline(ls, name, '|');
        ls >> tmp >> sets;
        ls >> tmp >> reps;
        ls >> tmp >> weight;
        entries.push_back({ name, sets, reps, weight, date });
    }
}

std::string Workout::serializeForUser() const {
    std::ostringstream oss;
    for (const auto& e : entries) {
        oss << e.date << " | " << e.name << " | Sets: " << e.sets << " | Reps: " << e.reps << " | Weight: " << e.weight << "\n";
    }
    return oss.str();
}