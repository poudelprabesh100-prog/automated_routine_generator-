#ifndef CLASSSSESSION_HPP
#define CLASSSSESSION_HPP
#include "timeslot.hpp"
#include <string>

class ClassSession{
private:
TimeSlot m_timeslot;
string m_teacherid;
string m_subjectid;
string m_roomid;
string m_batchid;
public:
ClassSession(TimeSlot timeslot,std::string teacherid,std::string subjectid,std::string roomid,std::string batchid );
TimeSlot getTimeSlot() const;
string getTeacherId() const;
string getSubjectId() const;
string getRoomId() const;
string getBatchId() const;



};
#endif
