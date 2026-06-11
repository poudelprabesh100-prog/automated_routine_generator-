#include <iostream>
#include <string>
#include "timeslot.hpp"
#include "classSession.hpp" 
#include "Instructor.hpp"
#include "Room.hpp"
#include "Student_batch.hpp"
#include "Course.hpp"

int main() {
    std::cout << "=== University Routine Generator: Week 1 Integration ===\n" << std::endl;

    // 1. Setup TimeSlot (Monday, 09:30 to 11:30)
    ClockTime startTime{9, 30};
    ClockTime endTime{11, 30};
    TimeSlot slot1(Day::Monday, startTime, endTime);

    // 2. Setup Course (Matches Nicolson's exact layout: code, hours)
    Course subject("COMP-102", 3);

    // 3. Setup Instructor (Matches Nicolson's exact layout: name, maxHours)
    // Notice: NO "T-001" ID string here anymore! Only 2 arguments.
    Instructor teacher("Dr. Niraj Sharma", 12);

    // 4. Test Nicolson's workload tracker logic
    std::cout << "--- Testing Instructor Tracking ---" << std::endl;
    if (teacher.assignNewCourse(subject)) {
        std::cout << "Success: Assigned " << subject.getCourseCode() << " to " << teacher.getName() << "\n" << std::endl;
    }

    // 5. Setup Room (Assuming Room constructor matches: id, capacity, type)
    Room classroom("Block-C-102", 60, RoomType::Theory);

    // 6. Setup StudentBatch (Matches Samip's layout: id, strength, program)
    // Notice: Added ProgramType::BCE as the 3rd argument!
    StudentBatch batch("BCT-2025-A", 48, ProgramType::BCE);

    // 7. Bundle into Prabesh's ClassSession Matrix
    ClassSession session1(slot1, teacher, subject, classroom, batch);

    // 8. Print Results using your team's EXACT function names
    std::cout << "--- Final Routine Entry Verification ---" << std::endl;
    std::cout << "Course Code: " << session1.getSubjectId().getCourseCode() << std::endl;
    std::cout << "Instructor:  " << session1.getTeacherId().getName() << std::endl;
    std::cout << "Room:        " << session1.getRoomId().getRoomId() << " (Capacity: " << session1.getRoomId().getCapacity() << ")" << std::endl;
    std::cout << "Batch:       " << session1.getBatchId().getBatchId() 
              << " (Program: " << session1.getBatchId().getProgramAsString() 
              << ", Strength: " << session1.getBatchId().getStrength() << " students)" << std::endl;
    std::cout << "Duration:    " << session1.getTimeSlot().getDurationmin() << " minutes" << std::endl;
    
    std::cout << "\n=======================================================" << std::endl;
    return 0;
}