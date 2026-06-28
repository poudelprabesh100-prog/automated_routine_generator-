# University Routine Generator & Timetable Scheduler

An elegant C++17 and Qt 6 desktop application designed to manage, track, and schedule university course routines, instructors, classrooms, and student batches with automatic workload constraint validations.

---

## Key Features

*   **Comprehensive Entity Management:** Add and manage *Instructors*, *Courses*, *Classrooms (Rooms)*, and *Student Batches*.
*   **Automatic Workload Checks:** The application automatically tracks weekly teaching hours for each instructor and rejects scheduling slots that exceed their hourly threshold.
*   **Interactive Scheduling:** Schedule class sessions by assigning a timeslot (day of week, start time, end time), an instructor, a course, a classroom, and a target student batch.


## Directory Structure

```text
├── AppManager.cpp / .hpp      # Controller: Manages global timetable state and lookups
├── Course.cpp / .hpp          # Model: Course details and allocated lecture hours
├── Instructor.cpp / .hpp      # Model: Instructor details and weekly workload checks
├── room.cpp / .hpp            # Model: Classroom identifiers, capacities, and room types
├── Student_batch.cpp / .hpp   # Model: Batch IDs, student strengths, and program types
├── classSession.cpp / .hpp    # Model: Holds a scheduled course slot representation
├── timeslot.cpp / .hpp        # Model: Encapsulates weekdays, start and end clock times
├── main.cpp                   # Entry point for the Console-only test environment
├── .gitignore                 # Excludes build and compiler artifacts
└── qt/
    └── qt/
        ├── main.cpp           # Entry point for the Qt GUI application
        ├── mainwindow.h       # UI definitions and signal/slot bindings
        ├── mainwindow.ui      # Default XML UI layout file (configured dynamically)
        └── qt.pro             # Qt project configuration file
```

---

## Prerequisites

*   **C++ Compiler:** A compiler supporting C++17 (e.g., GCC 13+ or MinGW-w64).
*   **Qt SDK:** Qt 6.0 or higher with the **Widgets** module.

## Build & Run Instructions

### 1. Build and Run the Console Version (Raw C++)

To test the basic model integration in a command prompt or shell:

```bash
# Compile the console app
g++ main.cpp AppManager.cpp Course.cpp Instructor.cpp Student_batch.cpp classSession.cpp room.cpp timeslot.cpp -o generator.exe

# Run the executable
./generator.exe
```

---

### 2. Build and Run the Qt GUI Desktop Version

You can build the desktop app using Qt Creator or directly from a MinGW-w64/Qt command line:

```bash
# 1. Add Qt tools and MinGW compiler to your system PATH
# (Replace paths with your local installation coordinates)
export PATH="/c/Qt/Tools/mingw1310_64/bin:/c/Qt/6.11.1/mingw_64/bin:$PATH"

# 2. Navigate to the Qt project directory
cd qt/qt

# 3. Generate Makefiles
qmake

# 4. Compile the project
mingw32-make
```

To run the application standalone on Windows, deploy the required Qt dependencies into the output folder:

```bash
# Deploy DLL runtimes into the release output folder
windeployqt release/qt.exe

# Run the application
./release/qt.exe
```
