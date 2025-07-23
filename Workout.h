#ifndef WORKOUT_H
#define WORKOUT_H

#include <string>
#include <vector>

struct WorkoutEntry {
    std::string name;
    int sets;
    int reps;
    double weight;
    std::string date;
};

class Workout {
private:
    std::vector<WorkoutEntry> entries;

public:
    void addWorkout(const std::string& name, int sets, int reps, double weight, const std::string& date);
    void removeWorkout(size_t idx);
    void editWorkout(size_t idx, const WorkoutEntry& newEntry);
    size_t getWorkoutCount() const;
    const WorkoutEntry* getWorkout(size_t idx) const;
    void loadFromUser(const std::string& rawData);
    std::string serializeForUser() const;
};

#endif