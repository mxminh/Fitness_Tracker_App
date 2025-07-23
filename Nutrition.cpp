#include "Nutrition.h"
#include <sstream>

void Nutrition::addEntry(const NutritionEntry& entry) {
    entries.push_back(entry);
}

void Nutrition::loadFromUser(const std::string& data) {
    entries.clear();
    std::istringstream iss(data);
    std::string line;
    while (std::getline(iss, line)) {
        NutritionEntry entry;
        std::istringstream ls(line);
        getline(ls, entry.date, '|');
        getline(ls, entry.food, '|');
        ls >> entry.protein >> entry.carbs >> entry.fat >> entry.calories;
        entries.push_back(entry);
    }
}

std::string Nutrition::serializeForUser() const {
    std::ostringstream oss;
    for (const auto& e : entries) {
        oss << e.date << " | " << e.food << " | Protein: " << e.protein << "g | Carbs: " << e.carbs << "g | Fat: " << e.fat << "g | Calories: " << e.calories << "\n";
    }
    return oss.str();
}