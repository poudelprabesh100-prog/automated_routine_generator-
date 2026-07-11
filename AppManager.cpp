#include "AppManager.hpp"
void AppManager::addInstructor(const Instructor& instructor){
    m_masterInstructors.push_back(instructor);
}
void AppManager::addRoom(const Room& room){
    m_masterRooms.push_back(room);
}
void AppManager::addCourse(const Course& course){
    m_masterCourses.push_back(course);
}
void AppManager::addBatch(const StudentBatch& batch ){
    m_masterBatches.push_back(batch);

}
void AppManager::addClassSession(const ClassSession& session){
    m_timetable.push_back(session);
}

bool AppManager::removeClassSession(int index) {
    if (index < 0 || index >= static_cast<int>(m_timetable.size())) return false;
    m_timetable.erase(m_timetable.begin() + index);
    return true;
}

Instructor* AppManager::findInstructorByName(const std::string& name){
 for(size_t i=0;i<m_masterInstructors.size(); ++i){
    if(m_masterInstructors[i].getName()==name){
        return &m_masterInstructors[i];

    }
 }
 return nullptr;
}
Course* AppManager::findCourseByCode(const std::string& code){
    for(size_t i=0;i<m_masterCourses.size();++i){
       if(m_masterCourses[i].getCourseCode()==code){
        return &m_masterCourses[i];
       }
    }
    return nullptr;
}
StudentBatch* AppManager::findBatchById(const std::string& id){
    for(size_t i=0;i<m_masterBatches.size();i++){
        if(m_masterBatches[i].getBatchId()==id ){
            return &m_masterBatches[i];
        }
    }
    return nullptr;
}
Room* AppManager::findRoomById(const std::string& id){
    for(size_t i=0;i<m_masterRooms.size();i++){
        if(m_masterRooms[i].getRoomId()==id){
            return &m_masterRooms[i];

        }
    }
    return nullptr;

}
const std::vector<ClassSession>& AppManager::getTimetable() const{
    return m_timetable;
}
const std::vector<Instructor>& AppManager::getInstructors() const {
    return m_masterInstructors;
}
const std::vector<Course>& AppManager::getCourses() const {
    return m_masterCourses;
}
const std::vector<Room>& AppManager::getRooms() const {
    return m_masterRooms;
}
const std::vector<StudentBatch>& AppManager::getBatches() const {
    return m_masterBatches;
}

Instructor* AppManager::findInstructorById(const std::string& id) {
    for (size_t i = 0; i < m_masterInstructors.size(); ++i) {
        if (m_masterInstructors[i].getId() == id) {
            return &m_masterInstructors[i];
        }
    }
    return nullptr;
}

// Removal helpers
bool AppManager::removeInstructor(const std::string& id) {

    for (auto it = m_masterInstructors.begin(); it != m_masterInstructors.end(); ++it) {
        if (it->getId() == id) {
            m_masterInstructors.erase(it);
            return true;
        }
    }
    return false;
}

bool AppManager::removeCourse(const std::string& code) {
    for (auto it = m_masterCourses.begin(); it != m_masterCourses.end(); ++it) {
        if (it->getCourseCode() == code) {
            m_masterCourses.erase(it);
            return true;
        }
    }
    return false;
}

bool AppManager::removeRoom(const std::string& id) {
    for (auto it = m_masterRooms.begin(); it != m_masterRooms.end(); ++it) {
        if (it->getRoomId() == id) {
            m_masterRooms.erase(it);
            return true;
        }
    }
    return false;
}

bool AppManager::removeBatch(const std::string& id) {
    for (auto it = m_masterBatches.begin(); it != m_masterBatches.end(); ++it) {
        if (it->getBatchId() == id) {
            m_masterBatches.erase(it);
            return true;
        }
    }
    return false;
}

// Usage checks
bool AppManager::isInstructorUsed(const std::string& id) const {
    for (const auto& sess : m_timetable) {
        if (sess.getTeacherId() && sess.getTeacherId()->getId() == id) return true;
    }
    return false;
}

bool AppManager::isCourseUsed(const std::string& code) const {
    for (const auto& sess : m_timetable) {
        if (sess.getSubjectId() && sess.getSubjectId()->getCourseCode() == code) return true;
    }
    return false;
}

bool AppManager::isRoomUsed(const std::string& id) const {
    for (const auto& sess : m_timetable) {
        if (sess.getRoomId() && sess.getRoomId()->getRoomId() == id) return true;
    }
    return false;
}

bool AppManager::isBatchUsed(const std::string& id) const {
    for (const auto& sess : m_timetable) {
        if (sess.getBatchId() && sess.getBatchId()->getBatchId() == id) return true;
    }
    return false;
}

bool AppManager::updateInstructor(const Instructor& instructor) {
    for (auto& inst : m_masterInstructors) {
        if (inst.getId() == instructor.getId()) {
            inst = instructor;
            return true;
        }
    }
    return false;
}

bool AppManager::updateCourse(const Course& course) {
    for (auto& crs : m_masterCourses) {
        if (crs.getCourseCode() == course.getCourseCode()) {
            crs = course;
            return true;
        }
    }
    return false;
}

bool AppManager::updateRoom(const Room& room) {
    for (auto& rm : m_masterRooms) {
        if (rm.getRoomId() == room.getRoomId()) {
            rm = room;
            return true;
        }
    }
    return false;
}

bool AppManager::updateBatch(const StudentBatch& batch) {
    for (auto& b : m_masterBatches) {
        if (b.getBatchId() == batch.getBatchId()) {
            b = batch;
            return true;
        }
    }
    return false;
}
