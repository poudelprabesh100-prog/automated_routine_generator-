#include "timeslot.hpp"
#include "classSession.hpp"
ClassSession::ClassSession(TimeSlot timeslot, std::string teacherid,std::string subjectid,std::string roomid,std::string batchid ):m_timeslot(timeslot),m_teacherid(teacherid),m_subjectid(subjectid),m_batchid(batchid),m_roomid(roomid){}
TimeSlot ClassSession::getTimeSlot() const{
    return m_timeslot;
}
string ClassSession::getTeacherId() const{
    return m_teacherid;
}
string ClassSession::getSubjectId() const{
    return m_subjectid;
}
string ClassSession::getRoomId() const{
    return m_roomid;
}
string ClassSession::getBatchId() const{
    return m_batchid;

}