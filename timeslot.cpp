#include "timeslot.hpp"
TimeSlot::TimeSlot(Day day,ClockTime startTime,ClockTime endTime){
    m_day=day;
    m_start_time=startTime;
    m_end_time=endTime;

}
Day TimeSlot::getDay() const{
    return m_day;
}
ClockTime TimeSlot::getStartTime() const{
    return m_start_time;
}
ClockTime TimeSlot::getEndTime() const{
    return m_end_time;
}
int TimeSlot::getDurationmin() const{
    int duration=(m_end_time.hours*60+m_end_time.minutes)-(m_start_time.hours*60+m_start_time.minutes);
    return duration;
}

bool TimeSlot::overlapsWith(const TimeSlot& other) const {
    if (m_day != other.getDay()) return false;
    
    int thisStart = m_start_time.hours * 60 + m_start_time.minutes;
    int thisEnd = m_end_time.hours * 60 + m_end_time.minutes;
    
    int otherStart = other.getStartTime().hours * 60 + other.getStartTime().minutes;
    int otherEnd = other.getEndTime().hours * 60 + other.getEndTime().minutes;
    
    return (thisStart < otherEnd) && (otherStart < thisEnd);
}

int TimeSlot::lunchBreakOverlapMinutes() const {
    int thisStart = m_start_time.hours * 60 + m_start_time.minutes;
    int thisEnd = m_end_time.hours * 60 + m_end_time.minutes;
    
    int lunchStart = 11 * 60; // 660
    int lunchEnd = 12 * 60;   // 720
    
    int overlapStart = (thisStart > lunchStart) ? thisStart : lunchStart;
    int overlapEnd = (thisEnd < lunchEnd) ? thisEnd : lunchEnd;
    
    if (overlapStart < overlapEnd) {
        return overlapEnd - overlapStart;
    }
    return 0;
}
