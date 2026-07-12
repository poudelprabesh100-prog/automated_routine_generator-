#ifndef COURSE_HPP
#define COURSE_HPP

#include <string>

class Course {
private:
    std::string code;
    std::string name;
    int creditHours;
    std::string courseCode;
    int allocatedHours;

public:
    // Full constructor: code + name + hours
    Course(std::string code, std::string name, int creditHours);
    // Backward-compat constructor: name defaults to code
    Course(std::string code, int hours);

    std::string getCode() const;
    std::string getName() const;
    int getCreditHours() const;

    // Legacy getters used by Instructor / AppManager
    std::string getCourseCode() const;
    int getAllocatedHours() const;
};

#endif
