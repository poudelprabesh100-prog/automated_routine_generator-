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
    std::vector<Course> assignedCourses; // Runtime hour-tracking (cleared on timetable reset)

    // Permanent locked subjects — set once at creation time, never cleared.
    std::vector<std::string> m_lockedSubjects;

public:
    // Full constructor: id + name + maxHours
    Instructor(std::string instructorId, std::string instructorName, int maxHours);
    // Backward-compat constructor: id is set to name
    Instructor(std::string instructorName, int maxHours);

    std::string getId() const;
    std::string getName() const;
    int getMaxLimitHours() const;
    const std::vector<Course>& getAssignedCourses() const;

    // Locked subjects — permanent list decided at instructor-creation time
    void setLockedSubjects(const std::vector<std::string>& subjects);
    const std::vector<std::string>& getLockedSubjects() const;

    // Returns true iff courseCode is in the permanent locked-subjects list.
    // An empty locked list is treated as "unset" (no restriction enforced by this method —
    // the UI prevents creation without at least one subject, but legacy JSON may have none).
    bool isQualifiedFor(const std::string& courseCode) const;

    int calculateTotalAssignedHours() const;
    bool assignNewCourse(const Course& newCourse);
    bool unassignCourse(const std::string& courseCode);
};

#endif
