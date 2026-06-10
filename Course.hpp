#ifndef COURSE_HPP
#define COURSE_HPP

#include <string>

class Course {
private:
    std::string courseCode;
    int allocatedHours;

public:
    // Constructor declaration
    Course(std::string code, int hours);

    // Getter declarations
    std::string getCourseCode() const;
    int getAllocatedHours() const;
};

#endif
