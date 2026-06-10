#include "Course.hpp"

// Constructor implementation
Course::Course(std::string code, int hours) {
    courseCode = code;
    allocatedHours = hours;
}

// Getter implementations
std::string Course::getCourseCode() const {
    return courseCode;
}

int Course::getAllocatedHours() const {
    return allocatedHours;
}
