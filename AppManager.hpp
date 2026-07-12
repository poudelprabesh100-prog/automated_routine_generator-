#ifndef APP_MANAGER_HPP
#define APP_MANAGER_HPP
#include <vector>
#include <string>
#include "Instructor.hpp"
#include "Course.hpp"
#include "Room.hpp"
#include "Student_Batch.hpp"
#include "classSession.hpp"
class AppManager{
    private:
    std::vector<Instructor> m_masterInstructors;
    std::vector<Course> m_masterCourses;
    std::vector<Room> m_masterRooms;
    std::vector<StudentBatch> m_masterBatches;
    
    std::vector<ClassSession> m_timetable; //Final generated Container
    public:
    AppManager()= default;
    void addInstructor(const Instructor& instructor);
    void addRoom(const Room& room);
    void addCourse(const Course& course);
    void addBatch(const StudentBatch& batch);
    std::string validateAndAddClassSession(const ClassSession& session);
    bool removeClassSession(int index); // Remove a timetable entry by index


    Instructor* findInstructorByName(const std::string& name);
    Instructor* findInstructorById(const std::string& id);
    Course* findCourseByCode(const std::string& code);
    Room* findRoomById(const std::string& id);
    StudentBatch* findBatchById(const std::string& id);
    // Deletion helpers – return true if removed
    bool removeInstructor(const std::string& id);
    bool removeCourse(const std::string& code);
    bool removeRoom(const std::string& id);
    bool removeBatch(const std::string& id);

    // Update helpers
    bool updateInstructor(const Instructor& instructor);
    bool updateCourse(const Course& course);
    bool updateRoom(const Room& room);
    bool updateBatch(const StudentBatch& batch);

    // Usage checks – block deletion if referenced
    bool isInstructorUsed(const std::string& id) const;
    bool isCourseUsed(const std::string& code) const;
    bool isRoomUsed(const std::string& id) const;
    bool isBatchUsed(const std::string& id) const;

    const std::vector<ClassSession>& getTimetable() const;
    const std::vector<Instructor>& getInstructors() const;
    const std::vector<Course>& getCourses() const;
    const std::vector<Room>& getRooms() const;
    const std::vector<StudentBatch>& getBatches() const;

    // Auto-generation
    void clearTimetable();
    void autoGenerateTimetable();

    // Busy-check helpers for auto-generation
    bool isInstructorBusyAt(const std::string& id, const TimeSlot& slot) const;
    bool isRoomBusyAt(const std::string& id, const TimeSlot& slot) const;
    bool isBatchBusyAt(const std::string& id, const TimeSlot& slot) const;

};

// Implementation notes: definitions are in AppManager.cpp ;
#endif
