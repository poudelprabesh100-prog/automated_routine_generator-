#include "Student_batch.hpp"

StudentBatch::StudentBatch(std::string batchId, int strength, ProgramType program, std::string department)
    : m_batchId(batchId), m_strength(strength), m_program(program), m_department(department) {}

std::string StudentBatch::getProgramAsString() const {
    switch (m_program) {
        case ProgramType::BCS: return "BCS";
        case ProgramType::BCE: return "BCE";
        case ProgramType::BIT: return "BIT";
        default:               return "Unknown";
    }
}

std::string StudentBatch::getDepartment() const {
    return m_department;
}