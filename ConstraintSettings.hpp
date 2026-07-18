#ifndef CONSTRAINT_SETTINGS_HPP
#define CONSTRAINT_SETTINGS_HPP

/**
 * ConstraintSettings — plain C++ struct (no Qt dependency).
 *
 * The MainWindow populates this from the Constraints tab UI and passes it
 * to AppManager::autoGenerateTimetable().  It is also serialised to JSON
 * via the mainwindow save/load helpers.
 *
 * Day index mapping (matches the extended Day enum in timeslot.hpp):
 *   0=Monday, 1=Tuesday, 2=Wednesday, 3=Thursday, 4=Friday, 5=Sunday,
 * 6=Saturday
 *
 * For the working-days array we use a UI-friendly indexing:
 *   workingDays[0]=Sunday, [1]=Monday, …, [6]=Saturday
 * This matches the checkbox order shown to the user (Sun through Sat).
 */
struct ConstraintSettings {

  // ── Working days ────────────────────────────────────────────────────────
  // Index: 0=Sunday, 1=Monday, 2=Tuesday, 3=Wednesday, 4=Thursday,
  //        5=Friday, 6=Saturday
  // Default: Sun–Fri checked, Saturday unchecked.
  bool workingDays[7] = {true, true, true, true, true, true, false};

  // ── Daily time window (minutes from midnight) ────────────────────────────
  int dayStartMinutes = 9 * 60; // 09:00
  int dayEndMinutes = 17 * 60;  // 17:00

  // ── Lunch break ─────────────────────────────────────────────────────────
  bool lunchBreakEnabled = true;
  int lunchStartMinutes = 13 * 60; // 13:00
  int lunchEndMinutes = 14 * 60;   // 14:00

  // ── Rule toggles ────────────────────────────────────────────────────────
  bool ruleNoInstructorDoubleBook = true; // same instructor, two slots
  bool ruleNoRoomDoubleBook = true;       // same room, two slots
  bool ruleNoBatchClash = true;           // same batch, two slots
  bool ruleInstructorDayGap =
      true; // instructor must skip ≥1 day between teaching days
  bool ruleNoSameSubjectConsecDays =
      true; // same course-batch pair on back-to-back days
  bool ruleRespectMaxWeeklyHours = true; // honour instructor.maxLimitHours
  bool ruleMaxConsecHoursEnabled =
      true; // limit consecutive teaching hours per day
  int ruleMaxConsecHoursPerDay =
      3; // max consecutive hours before mandatory break
  bool ruleEnforceSubjectLock =
      true; // always true — instructor must be qualified

  // ── Derived helpers (computed by MainWindow, stored for convenience) ────
  // Total available weekly teaching minutes (recomputed on UI changes).
  int weeklyAvailableMinutes() const {
    int workingDayCount = 0;
    for (int i = 0; i < 7; ++i)
      if (workingDays[i])
        ++workingDayCount;

    int dailyMinutes = dayEndMinutes - dayStartMinutes;
    if (lunchBreakEnabled) {
      int lunchLen = lunchEndMinutes - lunchStartMinutes;
      if (lunchLen > 0)
        dailyMinutes -= lunchLen;
    }
    if (dailyMinutes < 0)
      dailyMinutes = 0;

    return workingDayCount * dailyMinutes;
  }
};

#endif // CONSTRAINT_SETTINGS_HPP
