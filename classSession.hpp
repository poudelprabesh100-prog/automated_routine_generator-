#ifndef CLASSSESSION_HPP
#define CLASSSESSION_HPP

#include "timeslot.hpp"
#include "Instructor.hpp"
#include "Room.hpp"
#include "Student_batch.hpp"
#include "Course.hpp"
#include <string>

class ClassSession {
private:
    TimeSlot m_timeslot;
    Instructor* m_teacherid;
    Course* m_subjectid;
    Room* m_roomid;
    StudentBatch* m_batchid;
    std::string m_sessionId;

public:
    ClassSession(TimeSlot timeslot, Instructor* teacherid, Course* subjectid, Room* roomid, StudentBatch* batchid, std::string sessionId = "");
    
    std::string getSessionId() const;
    TimeSlot getTimeSlot() const;
    Instructor* getTeacherId() const;
    Course* getSubjectId() const;
    Room* getRoomId() const;
    StudentBatch* getBatchId() const;
};

#endif