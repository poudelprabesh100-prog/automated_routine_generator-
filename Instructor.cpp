#include "Instructor.hpp"
#include <iostream>
#include <algorithm>

// Full constructor: id + name + maxHours
Instructor::Instructor(std::string instructorId, std::string instructorName, int maxHours)
{
    id = std::move(instructorId);
    name = std::move(instructorName);
    maxLimitHours = maxHours;
}

// Backward-compat constructor: id defaults to name
Instructor::Instructor(std::string instructorName, int maxHours)
{
    id = instructorName;
    name = std::move(instructorName);
    maxLimitHours = maxHours;
}

std::string Instructor::getId() const { return id; }
std::string Instructor::getName() const { return name; }
int Instructor::getMaxLimitHours() const { return maxLimitHours; }

void Instructor::resetWeeklyHours() {
    m_hoursUsedThisWeek = 0;
}

void Instructor::addWeeklyHours(int hours) {
    m_hoursUsedThisWeek += hours;
}

int Instructor::getHoursUsedThisWeek() const {
    return m_hoursUsedThisWeek;
}

int Instructor::getRemainingWeeklyHours() const {
    return maxLimitHours - m_hoursUsedThisWeek;
}

// ---- Locked subjects ----

void Instructor::setLockedSubjects(const std::vector<std::string>& subjects) {
    m_lockedSubjects = subjects;
}

const std::vector<std::string>& Instructor::getLockedSubjects() const {
    return m_lockedSubjects;
}

bool Instructor::isQualifiedFor(const std::string& courseCode) const {
    // If no locked subjects have been set (legacy/empty), allow anything
    // so that old data files don't break. The UI always sets at least one.
    if (m_lockedSubjects.empty()) return true;
    return std::find(m_lockedSubjects.begin(), m_lockedSubjects.end(), courseCode)
           != m_lockedSubjects.end();
}

// ---- Hour tracking ----

