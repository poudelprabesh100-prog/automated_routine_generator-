#include "rules.hpp"

bool Rules::hasTeacherClash(const std::vector<ClassSession>& allSessions, std::string& outErrorMessage) {
    for (size_t i = 0; i < allSessions.size(); ++i) {
        for (size_t j = i + 1; j < allSessions.size(); ++j) {
            
            // Extract the instructor pointers from the ClassSession objects
            const Instructor* teacher1 = allSessions[i].getTeacherId();
            const Instructor* teacher2 = allSessions[j].getTeacherId();
            
            // Safety guard: skip if either session doesn't have an instructor assigned yet
            if (teacher1 == nullptr || teacher2 == nullptr) {
                continue;
            }
            
            // Compare the memory addresses (pointers) directly
            if (teacher1 == teacher2) {
                // Check if they occur on the same day
                if (allSessions[i].getTimeSlot().getDay() == allSessions[j].getTimeSlot().getDay()) {
                    // Check if their execution intervals overlap
                    if (allSessions[i].getTimeSlot().overlapsWith(allSessions[j].getTimeSlot())) {
                        
                        const Course* subject1 = allSessions[i].getSubjectId();
                        const Course* subject2 = allSessions[j].getSubjectId();
                        
                        std::string course1 = (subject1 != nullptr) ? subject1->getCourseCode() : "Unknown Course";
                        std::string course2 = (subject2 != nullptr) ? subject2->getCourseCode() : "Unknown Course";

                        // Formulate the error message
                        outErrorMessage = "Teacher Clash! " + teacher1->getName() +
                                          " is double-booked for " + course1 + " and " + course2 + ".";
                        return true; 
                    }
                }
            }
        }
    }
    return false; 
}
