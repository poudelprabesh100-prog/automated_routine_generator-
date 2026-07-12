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
std::string AppManager::validateAndAddClassSession(const ClassSession& session) {
    TimeSlot newSlot = session.getTimeSlot();
    
    // Lunch break overlap logic
    // Max 20 mins overlap allowed (leaves 40 mins of the 1 hr break).
    if (newSlot.lunchBreakOverlapMinutes() > 20) {
        return "Cannot schedule: Class eats into lunch break (11:00 AM to 12:00 PM). At least 40 minutes of break must remain.";
    }
    
    // Iterate over existing timetable for conflicts
    for (const auto& existing : m_timetable) {
        if (existing.getTimeSlot().overlapsWith(newSlot)) {
            if (existing.getTeacherId()->getId() == session.getTeacherId()->getId()) {
                return "Teacher Conflict: Instructor is already teaching at this time.";
            }
            if (existing.getRoomId()->getRoomId() == session.getRoomId()->getRoomId()) {
                return "Classroom Conflict: Room is already booked at this time.";
            }
            if (existing.getBatchId()->getBatchId() == session.getBatchId()->getBatchId()) {
                return "Student Class Conflict: Batch already has a class at this time.";
            }
        }
    }
    
    m_timetable.push_back(session);
    return "";
}

bool AppManager::removeClassSession(int index) {
    if (index < 0 || index >= static_cast<int>(m_timetable.size())) return false;
    
    // Unassign the course from the instructor
    Instructor* teacher = m_timetable[index].getTeacherId();
    Course* course = m_timetable[index].getSubjectId();
    if (teacher && course) {
        teacher->unassignCourse(course->getCourseCode());
    }
    
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

void AppManager::clearTimetable() {
    // Also clear assigned courses from all instructors
    for (auto& inst : m_masterInstructors) {
        // Unassign all courses
        while (!inst.getAssignedCourses().empty()) {
            inst.unassignCourse(inst.getAssignedCourses().front().getCourseCode());
        }
    }
    m_timetable.clear();
}

bool AppManager::isInstructorBusyAt(const std::string& id, const TimeSlot& slot) const {
    for (const auto& sess : m_timetable) {
        if (sess.getTeacherId() && sess.getTeacherId()->getId() == id &&
            sess.getTimeSlot().overlapsWith(slot)) {
            return true;
        }
    }
    return false;
}

bool AppManager::isRoomBusyAt(const std::string& id, const TimeSlot& slot) const {
    for (const auto& sess : m_timetable) {
        if (sess.getRoomId() && sess.getRoomId()->getRoomId() == id &&
            sess.getTimeSlot().overlapsWith(slot)) {
            return true;
        }
    }
    return false;
}

bool AppManager::isBatchBusyAt(const std::string& id, const TimeSlot& slot) const {
    for (const auto& sess : m_timetable) {
        if (sess.getBatchId() && sess.getBatchId()->getBatchId() == id &&
            sess.getTimeSlot().overlapsWith(slot)) {
            return true;
        }
    }
    return false;
}

void AppManager::autoGenerateTimetable() {
    clearTimetable();

    // Define standard 1-hour time slots (9 AM to 5 PM, skip 11:00-12:00 lunch)
    struct SlotDef { int startH; int startM; int endH; int endM; };
    std::vector<SlotDef> slotDefs = {
        {9,  0,  10, 0},   // 9:00 - 10:00
        {10, 0,  11, 0},   // 10:00 - 11:00
        // 11:00 - 12:00 is lunch break
        {12, 0,  13, 0},   // 12:00 - 13:00
        {13, 0,  14, 0},   // 13:00 - 14:00
        {14, 0,  15, 0},   // 14:00 - 15:00
        {15, 0,  16, 0},   // 15:00 - 16:00
        {16, 0,  17, 0},   // 16:00 - 17:00
    };

    std::vector<Day> days = {
        Day::Monday, Day::Tuesday, Day::Wednesday, Day::Thursday, Day::Friday
    };

    int nextStartDay = 0;

    // For each batch, schedule all courses
    for (size_t bIdx = 0; bIdx < m_masterBatches.size(); ++bIdx) {
        StudentBatch& batch = m_masterBatches[bIdx];

        for (size_t cIdx = 0; cIdx < m_masterCourses.size(); ++cIdx) {
            Course& course = m_masterCourses[cIdx];
            int hoursNeeded = course.getAllocatedHours();
            int hoursScheduled = 0;

            // Try to schedule 'hoursNeeded' 1-hour sessions for this course+batch
            int attempts = 0;
            while (hoursScheduled < hoursNeeded && attempts < 10) {
                attempts++;
                for (size_t offset = 0; offset < days.size() && hoursScheduled < hoursNeeded; ++offset) {
                    size_t dIdx = (nextStartDay + offset) % days.size();
                    
                    for (size_t sIdx = 0; sIdx < slotDefs.size() && hoursScheduled < hoursNeeded; ++sIdx) {
                        ClockTime startT{slotDefs[sIdx].startH, slotDefs[sIdx].startM};
                        ClockTime endT{slotDefs[sIdx].endH, slotDefs[sIdx].endM};
                        TimeSlot slot(days[dIdx], startT, endT);

                        // Check if this batch is already busy at this slot
                        if (isBatchBusyAt(batch.getBatchId(), slot)) {
                            continue;
                        }

                        // Find a free instructor
                        Instructor* freeInst = nullptr;
                        for (size_t iIdx = 0; iIdx < m_masterInstructors.size(); ++iIdx) {
                            Instructor& inst = m_masterInstructors[iIdx];
                            if (!isInstructorBusyAt(inst.getId(), slot)) {
                                // Check if assigning this course would exceed their hour limit
                                int currentHours = inst.calculateTotalAssignedHours();
                                if (currentHours + course.getAllocatedHours() <= inst.getMaxLimitHours()) {
                                    freeInst = &inst;
                                    break;
                                }
                            }
                        }
                        if (!freeInst) continue;

                        // Find a free room with enough capacity
                        Room* freeRoom = nullptr;
                        for (size_t rIdx = 0; rIdx < m_masterRooms.size(); ++rIdx) {
                            Room& rm = m_masterRooms[rIdx];
                            if (!isRoomBusyAt(rm.getRoomId(), slot) &&
                                rm.getCapacity() >= batch.getStrength()) {
                                freeRoom = &rm;
                                break;
                            }
                        }
                        if (!freeRoom) continue;

                        // All resources free — schedule the session
                        freeInst->assignNewCourse(course);
                        ClassSession session(slot, freeInst, &course, freeRoom, &batch);
                        m_timetable.push_back(session);
                        hoursScheduled++;
                        
                        // Break out of the slot loop to move to the next day!
                        // This ensures the course is spread out across the week.
                        break; 
                    }
                }
            }
            // Stagger the start day for the next course so days are balanced evenly
            nextStartDay = (nextStartDay + 1) % days.size();
        }
    }
}
