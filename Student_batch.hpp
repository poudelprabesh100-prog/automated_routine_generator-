#ifndef STUDENTBATCH_HPP
#define STUDENTBATCH_HPP

#include <string>

enum class ProgramType { BIT , BCE , BCS };

class StudentBatch {
    private:
        std::string m_batchId;
        int m_strength;
        ProgramType m_program;
        std::string m_department;
    
    public:
        StudentBatch(std::string batchId, int strength, ProgramType program, std::string department = "");
        
        std::string getBatchId() const { return m_batchId; }
        int getStrength() const { return m_strength; }
        ProgramType getProgram() const { return m_program; }
        std::string getProgramAsString() const;
        std::string getDepartment() const;
};

#endif