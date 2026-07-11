#include "TextFileHandler.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>

// ─── Internal file paths ──────────────────────────────────────────────────────

static const QString INST_FILE    = "instructors.txt";
static const QString COURSE_FILE  = "courses.txt";
static const QString ROOM_FILE    = "rooms.txt";
static const QString BATCH_FILE   = "batches.txt";
static const QString SESSION_FILE = "timetable.txt";

// ─── dataDir() ───────────────────────────────────────────────────────────────

QString TextFileHandler::dataDir()
{
    return QCoreApplication::applicationDirPath();
}

// ─── Private helpers ──────────────────────────────────────────────────────────

bool TextFileHandler::appendLine(const QString& filePath, const QString& line)
{
    QFile file(filePath);
    // QIODevice::Append opens/creates the file and positions at the end
    if (!file.open(QIODevice::Append | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << line << "\n";
    file.close();
    return true;
}

bool TextFileHandler::rewriteFile(const QString& filePath, const QStringList& lines)
{
    QFile file(filePath);
    // WriteOnly | Truncate clears the file before writing
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;

    QTextStream out(&file);
    for (const QString& line : lines)
        out << line << "\n";

    file.close();
    return true;
}

// ─── isDuplicate ─────────────────────────────────────────────────────────────

bool TextFileHandler::isDuplicate(const QString& filePath,
                                   const QString& key,
                                   int keyFieldIndex)
{
    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split('|');
        if (keyFieldIndex < fields.size() &&
            fields[keyFieldIndex].trimmed().compare(key.trimmed(), Qt::CaseInsensitive) == 0) {
            file.close();
            return true; // Duplicate found
        }
    }

    file.close();
    return false;
}

// ─── Append methods ───────────────────────────────────────────────────────────

bool TextFileHandler::appendInstructor(const QString& name, int maxHours)
{
    QString path = dataDir() + "/" + INST_FILE;
    // field 0 = name
    QString line = QString("%1|%2").arg(name).arg(maxHours);
    return appendLine(path, line);
}

bool TextFileHandler::appendCourse(const QString& code, int allocatedHours)
{
    QString path = dataDir() + "/" + COURSE_FILE;
    // field 0 = course code
    QString line = QString("%1|%2").arg(code).arg(allocatedHours);
    return appendLine(path, line);
}

bool TextFileHandler::appendRoom(const QString& roomId, int capacity, const QString& type)
{
    QString path = dataDir() + "/" + ROOM_FILE;
    // field 0 = room id
    QString line = QString("%1|%2|%3").arg(roomId).arg(capacity).arg(type);
    return appendLine(path, line);
}

bool TextFileHandler::appendBatch(const QString& batchId, int strength, const QString& program)
{
    QString path = dataDir() + "/" + BATCH_FILE;
    // field 0 = batch id
    QString line = QString("%1|%2|%3").arg(batchId).arg(strength).arg(program);
    return appendLine(path, line);
}

bool TextFileHandler::appendSession(const QString& day,
                                     const QString& startTime,
                                     const QString& endTime,
                                     const QString& instructor,
                                     const QString& course,
                                     const QString& room,
                                     const QString& batch)
{
    QString path = dataDir() + "/" + SESSION_FILE;
    QString line = QString("%1|%2|%3|%4|%5|%6|%7")
                       .arg(day)
                       .arg(startTime)
                       .arg(endTime)
                       .arg(instructor)
                       .arg(course)
                       .arg(room)
                       .arg(batch);
    return appendLine(path, line);
}

// ─── Full-rewrite methods (called at startup after JSON load) ─────────────────

bool TextFileHandler::rewriteInstructors(const QStringList& lines)
{
    return rewriteFile(dataDir() + "/" + INST_FILE, lines);
}

bool TextFileHandler::rewriteCourses(const QStringList& lines)
{
    return rewriteFile(dataDir() + "/" + COURSE_FILE, lines);
}

bool TextFileHandler::rewriteRooms(const QStringList& lines)
{
    return rewriteFile(dataDir() + "/" + ROOM_FILE, lines);
}

bool TextFileHandler::rewriteBatches(const QStringList& lines)
{
    return rewriteFile(dataDir() + "/" + BATCH_FILE, lines);
}

bool TextFileHandler::rewriteSessions(const QStringList& lines)
{
    return rewriteFile(dataDir() + "/" + SESSION_FILE, lines);
}
