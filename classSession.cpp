#include "timeslot.hpp"
#include "classSession.hpp" 


#include <random>
#include <sstream>

static std::string generateSessionId() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    std::stringstream ss;
    ss << std::hex << dis(gen) << dis(gen);
    return ss.str();
}

// The reordered initializer list is perfect!
ClassSession::ClassSession(TimeSlot timeslot, Instructor* teacherid, Course* subjectid, Room* roomid, StudentBatch* batchid, std::string sessionId)
    : m_timeslot(timeslot), m_teacherid(teacherid), m_subjectid(subjectid), m_roomid(roomid), m_batchid(batchid)
{
    if (sessionId.empty()) {
        m_sessionId = generateSessionId();
    } else {
        m_sessionId = sessionId;
    }
}

std::string ClassSession::getSessionId() const {
    return m_sessionId;
}

TimeSlot ClassSession::getTimeSlot() const {
    return m_timeslot;
}

Instructor* ClassSession::getTeacherId() const {
    return m_teacherid;
}

Course* ClassSession::getSubjectId() const {
    return m_subjectid;
}

Room* ClassSession::getRoomId() const {
    return m_roomid;
}

StudentBatch* ClassSession::getBatchId() const {
    return m_batchid;
}