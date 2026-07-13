#ifndef RULES_HPP
#define RULES_HPP

#include "classSession.hpp"
#include <vector>
#include <string>

class Rules {
public:
    // Rule: Checks if the same instructor is booked for two different courses at overlapping times
    static bool hasTeacherClash(const std::vector<ClassSession>& allSessions, std::string& outErrorMessage);
};

#endif // RULES_HPP
