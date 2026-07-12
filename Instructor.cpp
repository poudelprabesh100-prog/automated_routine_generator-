#include "Instructor.hpp"
#include <iostream>

// Full constructor: id + name + maxHours
Instructor::Instructor(std::string instructorId, std::string instructorName, int maxHours)
{
    id = std::move(instructorId);
    name = std::move(instructorName);
    maxLimitHours = maxHours;
}

// Backward-compat constructor: id defaults to name
Instructor::Instructor(std::string instructorName, int maxHours)
{
    id = instructorName;
    name = std::move(instructorName);
    maxLimitHours = maxHours;
}

std::string Instructor::getId() const { return id; }
std::string Instructor::getName() const { return name; }
int Instructor::getMaxLimitHours() const { return maxLimitHours; }

const std::vector<Course>& Instructor::getAssignedCourses() const {
    return assignedCourses;
}

int Instructor::calculateTotalAssignedHours() const {
    int total = 0;
    for (const Course& c : assignedCourses) {
        total += c.getAllocatedHours();
    }
    return total;
}

bool Instructor::assignNewCourse(const Course& newCourse) {
    int currentHours = calculateTotalAssignedHours();
    if (currentHours + newCourse.getAllocatedHours() > maxLimitHours) {
        std::cout << "Error: Cannot assign " << newCourse.getCourseCode()
                  << ". It exceeds " << name << "'s weekly hour limit!\n";
        return false;
    }
    assignedCourses.push_back(newCourse);
    return true;
}

bool Instructor::unassignCourse(const std::string& courseCode) {
    for (auto it = assignedCourses.begin(); it != assignedCourses.end(); ++it) {
        if (it->getCourseCode() == courseCode) {
            assignedCourses.erase(it);
            return true;
        }
    }
    return false;
}
