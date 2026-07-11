#ifndef TEXTFILEHANDLER_H
#define TEXTFILEHANDLER_H

#include <QString>

/**
 * TextFileHandler
 * ───────────────
 * Provides static helper methods for appending entity records to plain-text
 * files.  One file per entity; one record per line; pipe-delimited fields.
 *
 * Files written next to the application executable (applicationDirPath()):
 *   instructors.txt  – name|maxHours
 *   courses.txt      – code|allocatedHours
 *   rooms.txt        – roomId|capacity|type
 *   batches.txt      – batchId|strength|program
 *   timetable.txt    – day|startTime|endTime|instructor|course|room|batch
 *
 * The Qt layer (mainwindow.cpp) calls these immediately after every successful
 * Add/Schedule action so the .txt files always mirror the JSON save file.
 */
class TextFileHandler
{
public:
    // ── Directory where all .txt files are written ────────────────────────────
    static QString dataDir();

    // ── Per-entity append methods ─────────────────────────────────────────────
    static bool appendInstructor(const QString& name, int maxHours);
    static bool appendCourse(const QString& code, int allocatedHours);
    static bool appendRoom(const QString& roomId, int capacity, const QString& type);
    static bool appendBatch(const QString& batchId, int strength, const QString& program);
    static bool appendSession(const QString& day,
                              const QString& startTime,
                              const QString& endTime,
                              const QString& instructor,
                              const QString& course,
                              const QString& room,
                              const QString& batch);

    // ── Full-rewrite helpers ──────────────────────────────────────────────────
    // Overwrite the entire file from the provided pre-formatted lines.
    // Call these from loadFromFile() on startup to sync txt files with JSON.
    static bool rewriteInstructors(const QStringList& lines);
    static bool rewriteCourses(const QStringList& lines);
    static bool rewriteRooms(const QStringList& lines);
    static bool rewriteBatches(const QStringList& lines);
    static bool rewriteSessions(const QStringList& lines);

    // ── Duplicate-detection helper ────────────────────────────────────────────
    /**
     * Reads @p filePath and checks whether any existing line, when split by '|',
     * has its field at index @p keyFieldIndex equal to @p key.
     * Returns true if a duplicate is found.
     */
    static bool isDuplicate(const QString& filePath,
                             const QString& key,
                             int keyFieldIndex = 0);

private:
    static bool appendLine(const QString& filePath, const QString& line);
    static bool rewriteFile(const QString& filePath, const QStringList& lines);
};

#endif // TEXTFILEHANDLER_H
