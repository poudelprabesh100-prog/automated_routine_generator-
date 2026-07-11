#ifndef INSTRUCTOR_HPP
#define INSTRUCTOR_HPP

#include <string>
#include <vector>
#include "Course.hpp" // Links Course to Instructor

class Instructor {
private:
    std::string id;
    std::string name;
    int maxLimitHours;
    std::vector<Course> assignedCourses; // Tracking assigned courses

public:
    // Full constructor: explicit id + name + maxHours
    Instructor(std::string instructorId, std::string instructorName, int maxHours);
    // Backward-compat constructor: id is set to name
    Instructor(std::string instructorName, int maxHours);

    std::string getId() const;
    std::string getName() const;
    int getMaxLimitHours() const;
    const std::vector<Course>& getAssignedCourses() const;

    int calculateTotalAssignedHours() const;
    bool assignNewCourse(const Course& newCourse);
};

#endif
