#include "Course.hpp"

// Backward-compat constructor: name defaults to code
Course::Course(std::string code, int hours) {
    this->code = code;
    this->name = code;
    this->creditHours = hours;
    this->courseCode = code;
    this->allocatedHours = hours;
}

// Full constructor: explicit code + name + creditHours
Course::Course(std::string code, std::string name, int creditHours) {
    this->code = code;
    this->name = name;
    this->creditHours = creditHours;
    this->courseCode = code;
    this->allocatedHours = creditHours;
}

std::string Course::getCourseCode() const { return courseCode; }
int Course::getAllocatedHours() const { return allocatedHours; }
std::string Course::getCode() const { return code; }
std::string Course::getName() const { return name; }
int Course::getCreditHours() const { return creditHours; }
