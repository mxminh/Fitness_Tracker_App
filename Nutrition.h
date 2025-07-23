#ifndef NUTRITION_H
#define NUTRITION_H

#include <string>
#include <vector>

struct NutritionEntry {
    std::string date;
    std::string food;
    double protein;
    double carbs;
    double fat;
    double calories;
};

class Nutrition {
private:
    std::vector<NutritionEntry> entries;
public:
    void addEntry(const NutritionEntry& entry);
    void loadFromUser(const std::string& data);
    std::string serializeForUser() const;
};

#endif