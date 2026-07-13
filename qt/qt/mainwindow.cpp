#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Instructor.hpp"
#include "Course.hpp"
#include "room.hpp"
#include "Student_batch.hpp"
#include "classSession.hpp"
#include "timeslot.hpp"
#include "AppManager.hpp"

#include <QHeaderView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>


// Helper to convert Day enum to QString
static QString dayToString(Day d) {
    switch(d) {
        case Day::Monday:    return "Monday";
        case Day::Tuesday:   return "Tuesday";
        case Day::Wednesday: return "Wednesday";
        case Day::Thursday:  return "Thursday";
        case Day::Friday:    return "Friday";
        default:             return "Unknown";
    }
}

// Helper to format ClockTime as HH:MM
static QString formatClockTime(ClockTime t) {
    return QString("%1:%2")
        .arg(t.hours, 2, 10, QChar('0'))
        .arg(t.minutes, 2, 10, QChar('0'));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set modern Catppuccin-inspired dark stylesheet
    this->setStyleSheet(R"(
        QMainWindow {
            background-color: #1e1e2e;
        }
        QTabWidget::pane {
            border: 1px solid #313244;
            background-color: #181825;
            border-radius: 8px;
        }
        QTabBar::tab {
            background: #11111b;
            color: #cdd6f4;
            border: 1px solid #313244;
            padding: 10px 20px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            font-weight: bold;
            font-family: 'Segoe UI', Helvetica, sans-serif;
        }
        QTabBar::tab:selected, QTabBar::tab:hover {
            background: #181825;
            border-bottom-color: #181825;
            color: #89b4fa;
        }
        QLabel {
            color: #cdd6f4;
            font-size: 13px;
            font-family: 'Segoe UI', Helvetica, sans-serif;
            font-weight: 500;
        }
        QLineEdit, QSpinBox, QComboBox, QTimeEdit {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 6px;
            font-size: 13px;
            font-family: 'Segoe UI', Helvetica, sans-serif;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QTimeEdit:focus {
            border: 1px solid #89b4fa;
        }
        QLineEdit:disabled, QSpinBox:disabled, QComboBox:disabled {
            background-color: #1e1e2e;
            color: #6c7086;
            border: 1px solid #313244;
        }
        QPushButton {
            background-color: #89b4fa;
            color: #11111b;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            font-size: 13px;
            font-family: 'Segoe UI', Helvetica, sans-serif;
        }
        QPushButton:hover {
            background-color: #b4befe;
        }
        QPushButton:pressed {
            background-color: #74c7ec;
        }
        QListWidget, QTableWidget {
            background-color: #11111b;
            color: #cdd6f4;
            border: 1px solid #313244;
            border-radius: 8px;
            padding: 5px;
            font-family: 'Segoe UI', Helvetica, sans-serif;
        }
        QHeaderView::section {
            background-color: #313244;
            color: #cdd6f4;
            border: 1px solid #45475a;
            padding: 6px;
            font-weight: bold;
            font-family: 'Segoe UI', Helvetica, sans-serif;
        }
        QTableWidget QTableCornerButton::section {
            background-color: #313244;
        }
    )");

    setupUI();
    loadFromFile();
    refreshListsAndTables();
    populateCombos();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    // ==========================================
    // 1. INSTRUCTORS TAB
    // ==========================================
    QWidget *instTab = new QWidget();
    QHBoxLayout *instLayout = new QHBoxLayout(instTab);

    QWidget *instLeft = new QWidget();
    QVBoxLayout *instFormLayout = new QVBoxLayout(instLeft);
    QFormLayout *instForm = new QFormLayout();

    m_instIdEdit   = new QLineEdit();
    m_instNameEdit = new QLineEdit();
    m_instHoursSpin = new QSpinBox();
    m_instHoursSpin->setRange(1, 100);
    m_instHoursSpin->setValue(20);

    // Subject count spinner (1–3)
    m_instSubjectCountSpin = new QSpinBox();
    m_instSubjectCountSpin->setRange(1, 3);
    m_instSubjectCountSpin->setValue(1);
    m_instSubjectCountSpin->setToolTip("How many subjects will this instructor teach?");

    instForm->addRow(new QLabel("Instructor ID:"),        m_instIdEdit);
    instForm->addRow(new QLabel("Instructor Name:"),      m_instNameEdit);
    instForm->addRow(new QLabel("Max Weekly Hours:"),     m_instHoursSpin);
    instForm->addRow(new QLabel("Number of Subjects:"),   m_instSubjectCountSpin);

    // Container for the dynamic subject-selection combos
    m_instSubjectContainer = new QWidget();
    m_instSubjectLayout    = new QVBoxLayout(m_instSubjectContainer);
    m_instSubjectLayout->setContentsMargins(0, 0, 0, 0);
    m_instSubjectLayout->setSpacing(6);

    instFormLayout->addLayout(instForm);
    instFormLayout->addWidget(m_instSubjectContainer);

    connect(m_instSubjectCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onSubjectCountChanged);

    QPushButton *btnInstAdd    = new QPushButton("Add Instructor");
    QPushButton *btnInstEdit   = new QPushButton("Edit Instructor");
    QPushButton *btnInstDelete = new QPushButton("Delete Instructor");

    connect(btnInstAdd,    &QPushButton::clicked, this, &MainWindow::onAddInstructor);
    connect(btnInstEdit,   &QPushButton::clicked, this, &MainWindow::onEditInstructor);
    connect(btnInstDelete, &QPushButton::clicked, this, &MainWindow::onDeleteInstructor);

    QHBoxLayout *instBtnLayout = new QHBoxLayout();
    instBtnLayout->addWidget(btnInstAdd);
    instBtnLayout->addWidget(btnInstEdit);
    instBtnLayout->addWidget(btnInstDelete);

    instFormLayout->addLayout(instBtnLayout);
    instFormLayout->addStretch();

    m_instList = new QListWidget();
    instLayout->addWidget(instLeft, 1);
    instLayout->addWidget(m_instList, 2);
    m_tabWidget->addTab(instTab, "Instructors");

    // Build initial 1 subject combo (courses list may be empty at this point;
    // combos are re-populated in populateCombos() and after each course add).
    rebuildSubjectCombos(1);

    // ==========================================
    // 2. COURSES TAB  (semester / department removed)
    // ==========================================
    QWidget *courseTab = new QWidget();
    QHBoxLayout *courseLayout = new QHBoxLayout(courseTab);

    QWidget *courseLeft = new QWidget();
    QVBoxLayout *courseFormLayout = new QVBoxLayout(courseLeft);
    QFormLayout *courseForm = new QFormLayout();

    m_courseCodeEdit  = new QLineEdit();
    m_courseNameEdit  = new QLineEdit();
    m_courseHoursSpin = new QSpinBox();
    m_courseHoursSpin->setRange(1, 20);
    m_courseHoursSpin->setValue(3);

    courseForm->addRow(new QLabel("Course Code:"),  m_courseCodeEdit);
    courseForm->addRow(new QLabel("Course Name:"),  m_courseNameEdit);
    courseForm->addRow(new QLabel("Credit Hours:"), m_courseHoursSpin);

    QPushButton *btnCourseAdd    = new QPushButton("Add Course");
    QPushButton *btnCourseEdit   = new QPushButton("Edit Course");
    QPushButton *btnCourseDelete = new QPushButton("Delete Course");

    connect(btnCourseAdd,    &QPushButton::clicked, this, &MainWindow::onAddCourse);
    connect(btnCourseEdit,   &QPushButton::clicked, this, &MainWindow::onEditCourse);
    connect(btnCourseDelete, &QPushButton::clicked, this, &MainWindow::onDeleteCourse);

    QHBoxLayout *courseBtnLayout = new QHBoxLayout();
    courseBtnLayout->addWidget(btnCourseAdd);
    courseBtnLayout->addWidget(btnCourseEdit);
    courseBtnLayout->addWidget(btnCourseDelete);

    courseFormLayout->addLayout(courseForm);
    courseFormLayout->addLayout(courseBtnLayout);
    courseFormLayout->addStretch();

    m_courseList = new QListWidget();
    courseLayout->addWidget(courseLeft, 1);
    courseLayout->addWidget(m_courseList, 2);
    m_tabWidget->addTab(courseTab, "Courses");

    // ==========================================
    // 3. ROOMS TAB
    // ==========================================
    QWidget *roomTab = new QWidget();
    QHBoxLayout *roomLayout = new QHBoxLayout(roomTab);

    QWidget *roomLeft = new QWidget();
    QVBoxLayout *roomFormLayout = new QVBoxLayout(roomLeft);
    QFormLayout *roomForm = new QFormLayout();

    m_roomIdEdit       = new QLineEdit();
    m_roomBuildingEdit = new QLineEdit();
    m_roomCapSpin      = new QSpinBox();
    m_roomCapSpin->setRange(1, 500);
    m_roomCapSpin->setValue(60);
    m_roomTypeCombo = new QComboBox();
    m_roomTypeCombo->addItems({"Theory", "Lab", "Auditorium"});

    roomForm->addRow(new QLabel("Room Number:"), m_roomIdEdit);
    roomForm->addRow(new QLabel("Building:"),    m_roomBuildingEdit);
    roomForm->addRow(new QLabel("Capacity:"),    m_roomCapSpin);
    roomForm->addRow(new QLabel("Room Type:"),   m_roomTypeCombo);

    QPushButton *btnRoomAdd    = new QPushButton("Add Room");
    QPushButton *btnRoomEdit   = new QPushButton("Edit Room");
    QPushButton *btnRoomDelete = new QPushButton("Delete Room");

    connect(btnRoomAdd,    &QPushButton::clicked, this, &MainWindow::onAddRoom);
    connect(btnRoomEdit,   &QPushButton::clicked, this, &MainWindow::onEditRoom);
    connect(btnRoomDelete, &QPushButton::clicked, this, &MainWindow::onDeleteRoom);

    QHBoxLayout *roomBtnLayout = new QHBoxLayout();
    roomBtnLayout->addWidget(btnRoomAdd);
    roomBtnLayout->addWidget(btnRoomEdit);
    roomBtnLayout->addWidget(btnRoomDelete);

    roomFormLayout->addLayout(roomForm);
    roomFormLayout->addLayout(roomBtnLayout);
    roomFormLayout->addStretch();

    m_roomList = new QListWidget();
    roomLayout->addWidget(roomLeft, 1);
    roomLayout->addWidget(m_roomList, 2);
    m_tabWidget->addTab(roomTab, "Rooms");

    // ==========================================
    // 4. STUDENT BATCHES TAB
    // ==========================================
    QWidget *batchTab = new QWidget();
    QHBoxLayout *batchLayout = new QHBoxLayout(batchTab);

    QWidget *batchLeft = new QWidget();
    QVBoxLayout *batchFormLayout = new QVBoxLayout(batchLeft);
    QFormLayout *batchForm = new QFormLayout();

    m_batchIdEdit = new QLineEdit();
    m_batchStrengthSpin = new QSpinBox();
    m_batchStrengthSpin->setRange(1, 200);
    m_batchStrengthSpin->setValue(45);
    m_batchProgCombo = new QComboBox();
    m_batchProgCombo->addItems({"BIT", "BCE", "BCS"});
    m_batchDeptEdit = new QLineEdit();

    batchForm->addRow(new QLabel("Batch Name:"),  m_batchIdEdit);
    batchForm->addRow(new QLabel("Department:"),  m_batchDeptEdit);
    batchForm->addRow(new QLabel("Strength:"),    m_batchStrengthSpin);
    batchForm->addRow(new QLabel("Program:"),     m_batchProgCombo);

    QPushButton *btnBatchAdd    = new QPushButton("Add Batch");
    QPushButton *btnBatchEdit   = new QPushButton("Edit Batch");
    QPushButton *btnBatchDelete = new QPushButton("Delete Batch");

    connect(btnBatchAdd,    &QPushButton::clicked, this, &MainWindow::onAddBatch);
    connect(btnBatchEdit,   &QPushButton::clicked, this, &MainWindow::onEditBatch);
    connect(btnBatchDelete, &QPushButton::clicked, this, &MainWindow::onDeleteBatch);

    QHBoxLayout *batchBtnLayout = new QHBoxLayout();
    batchBtnLayout->addWidget(btnBatchAdd);
    batchBtnLayout->addWidget(btnBatchEdit);
    batchBtnLayout->addWidget(btnBatchDelete);

    batchFormLayout->addLayout(batchForm);
    batchFormLayout->addLayout(batchBtnLayout);
    batchFormLayout->addStretch();

    m_batchList = new QListWidget();
    batchLayout->addWidget(batchLeft, 1);
    batchLayout->addWidget(m_batchList, 2);
    m_tabWidget->addTab(batchTab, "Student Batches");

    // ==========================================
    // 5. TIMETABLE / SCHEDULER TAB
    // ==========================================
    QWidget *timetableTab = new QWidget();
    QHBoxLayout *timetableLayout = new QHBoxLayout(timetableTab);

    QWidget *timetableLeft = new QWidget();
    QVBoxLayout *timetableFormLayout = new QVBoxLayout(timetableLeft);
    QFormLayout *sessionForm = new QFormLayout();

    m_sessInstCombo   = new QComboBox();
    m_sessCourseCombo = new QComboBox();
    m_sessRoomCombo   = new QComboBox();
    m_sessBatchCombo  = new QComboBox();

    m_sessDayCombo = new QComboBox();
    m_sessDayCombo->addItems({"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"});

    m_sessStartEdit = new QTimeEdit(QTime(9, 0));
    m_sessEndEdit   = new QTimeEdit(QTime(10, 0));
    m_sessStartEdit->setTimeRange(QTime(9, 0), QTime(17, 0));
    m_sessEndEdit->setTimeRange(QTime(9, 0), QTime(17, 0));

    sessionForm->addRow(new QLabel("Instructor:"),    m_sessInstCombo);
    sessionForm->addRow(new QLabel("Course:"),        m_sessCourseCombo);
    sessionForm->addRow(new QLabel("Room:"),          m_sessRoomCombo);
    sessionForm->addRow(new QLabel("Student Batch:"), m_sessBatchCombo);
    sessionForm->addRow(new QLabel("Day:"),           m_sessDayCombo);
    sessionForm->addRow(new QLabel("Start Time:"),    m_sessStartEdit);
    sessionForm->addRow(new QLabel("End Time:"),      m_sessEndEdit);

    QPushButton *btnSessionAdd = new QPushButton("Schedule Class Session");
    QPushButton *btnSessionDelete = new QPushButton("Delete Class Session");
    connect(btnSessionAdd, &QPushButton::clicked, this, &MainWindow::onAddClassSession);
    connect(btnSessionDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClassSession);

    // ---- Auto Generate Button ----
    QPushButton *btnAutoGenerate = new QPushButton("Auto Generate Routine");
    btnAutoGenerate->setStyleSheet(R"(
        QPushButton {
            background-color: #a6e3a1;
            color: #11111b;
            border: none;
            border-radius: 8px;
            padding: 12px 20px;
            font-weight: bold;
            font-size: 15px;
            font-family: 'Segoe UI', Helvetica, sans-serif;
        }
        QPushButton:hover {
            background-color: #94e2d5;
        }
        QPushButton:pressed {
            background-color: #74c7ec;
        }
    )");
    connect(btnAutoGenerate, &QPushButton::clicked, this, &MainWindow::onAutoGenerate);

    timetableFormLayout->addLayout(sessionForm);
    timetableFormLayout->addWidget(btnAutoGenerate);
    timetableFormLayout->addSpacing(10);
    timetableFormLayout->addWidget(btnSessionAdd);
    timetableFormLayout->addWidget(btnSessionDelete);
    timetableFormLayout->addStretch();

    m_timetableTable = new QTableWidget();
    m_timetableTable->setColumnCount(7);
    m_timetableTable->setHorizontalHeaderLabels({"Day", "Time", "Course", "Instructor", "Room", "Batch", "Duration"});
    m_timetableTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_timetableTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    timetableLayout->addWidget(timetableLeft, 1);
    timetableLayout->addWidget(m_timetableTable, 2);
    m_tabWidget->addTab(timetableTab, "Generate & View Timetable");
}

// ==========================================
// Subject combo helpers
// ==========================================

void MainWindow::rebuildSubjectCombos(int count)
{
    // Remove all existing combos from the layout and delete them
    for (QComboBox* cb : m_instSubjectCombos) {
        m_instSubjectLayout->removeWidget(cb);
        delete cb;
    }
    m_instSubjectCombos.clear();

    // Build the course-code list from the master list
    QStringList courseCodes;
    for (const auto& crs : m_appManager.getCourses()) {
        courseCodes << QString::fromStdString(crs.getCourseCode());
    }

    for (int i = 0; i < count; ++i) {
        QLabel *lbl = new QLabel(QString("Subject %1:").arg(i + 1));
        lbl->setFixedWidth(90);

        QComboBox *cb = new QComboBox();
        cb->addItem("-- Select Course --");   // placeholder; index 0 is invalid
        cb->addItems(courseCodes);

        // Wrap label+combo in a widget so we can manage them together
        QWidget *rowWidget = new QWidget();
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->addWidget(lbl);
        rowLayout->addWidget(cb);

        m_instSubjectLayout->addWidget(rowWidget);
        m_instSubjectCombos.append(cb);
    }
}

void MainWindow::onSubjectCountChanged(int count)
{
    rebuildSubjectCombos(count);
}

// ==========================================
// Instructor list display helpers
// ==========================================

// Builds the display string for one instructor entry in the list.
static QString instDisplayString(const Instructor& inst)
{
    QString base = QString("%1 (Max Hours: %2)")
        .arg(QString::fromStdString(inst.getName()))
        .arg(inst.getMaxLimitHours());

    const auto& locked = inst.getLockedSubjects();
    if (!locked.empty()) {
        QStringList subjects;
        for (const auto& s : locked)
            subjects << QString::fromStdString(s);
        base += QString(" \u2014 Subjects: %1 [LOCKED]").arg(subjects.join(", "));
    }
    return base;
}

void MainWindow::refreshInstList()
{
    m_instList->clear();
    for (const auto& inst : m_appManager.getInstructors()) {
        m_instList->addItem(instDisplayString(inst));
    }
}

// ==========================================
// populateInitialData
// ==========================================
void MainWindow::populateInitialData()
{
    // Courses first (instructors reference course codes)
    m_appManager.addCourse(Course("COMP-102", "Computer Programming", 3));
    m_appManager.addCourse(Course("MATH-101", "Mathematics I", 4));

    // Instructors — explicitly assigned subjects
    {
        Instructor inst("Dr. Niraj Sharma", 12);
        inst.setLockedSubjects({"COMP-102"});
        m_appManager.addInstructor(inst);
    }
    {
        Instructor inst("Prof. Ram Prasad", 15);
        inst.setLockedSubjects({"MATH-101"});
        m_appManager.addInstructor(inst);
    }

    m_appManager.addRoom(Room("Block-C-102", 60, RoomType::Theory));
    m_appManager.addRoom(Room("Lab-A-301",   40, RoomType::Lab));

    m_appManager.addBatch(StudentBatch("BCT-2025-A", 48, ProgramType::BCE));
    m_appManager.addBatch(StudentBatch("BIT-2025-B", 45, ProgramType::BIT));

    // Populate list widgets
    refreshInstList();

    m_courseList->addItem("COMP-102 (Allocated Hours: 3)");
    m_courseList->addItem("MATH-101 (Allocated Hours: 4)");

    m_roomList->addItem("Block-C-102 (Capacity: 60, Type: Theory)");
    m_roomList->addItem("Lab-A-301 (Capacity: 40, Type: Lab)");

    m_batchList->addItem("BCT-2025-A (Strength: 48, Program: BCE)");
    m_batchList->addItem("BIT-2025-B (Strength: 45, Program: BIT)");
}

// ==========================================
// saveToFile
// ==========================================
void MainWindow::saveToFile()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/timetable_data.json";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonObject rootObj;

    // 1. Instructors
    QJsonArray instArray;
    for (const auto& inst : m_appManager.getInstructors()) {
        QJsonObject instObj;
        instObj["id"]            = QString::fromStdString(inst.getId());
        instObj["name"]          = QString::fromStdString(inst.getName());
        instObj["maxLimitHours"] = inst.getMaxLimitHours();

        // Locked subjects (permanent)
        QJsonArray lockedArray;
        for (const auto& s : inst.getLockedSubjects())
            lockedArray.append(QString::fromStdString(s));
        instObj["lockedSubjects"] = lockedArray;

        // Runtime-assigned courses (hour tracking)
        QJsonArray assignedArray;
        for (const auto& crs : inst.getAssignedCourses())
            assignedArray.append(QString::fromStdString(crs.getCourseCode()));
        instObj["assignedCourses"] = assignedArray;

        instArray.append(instObj);
    }
    rootObj["instructors"] = instArray;

    // 2. Courses  (no semester / department)
    QJsonArray courseArray;
    for (const auto& crs : m_appManager.getCourses()) {
        QJsonObject crsObj;
        crsObj["code"]           = QString::fromStdString(crs.getCode());
        crsObj["name"]           = QString::fromStdString(crs.getName());
        crsObj["creditHours"]    = crs.getCreditHours();
        crsObj["courseCode"]     = QString::fromStdString(crs.getCourseCode());
        crsObj["allocatedHours"] = crs.getAllocatedHours();
        courseArray.append(crsObj);
    }
    rootObj["courses"] = courseArray;

    // 3. Rooms
    QJsonArray roomArray;
    for (const auto& rm : m_appManager.getRooms()) {
        QJsonObject rmObj;
        rmObj["roomId"]   = QString::fromStdString(rm.getRoomId());
        rmObj["capacity"] = rm.getCapacity();
        rmObj["type"]     = static_cast<int>(rm.getType());
        rmObj["building"] = QString::fromStdString(rm.getBuilding());
        roomArray.append(rmObj);
    }
    rootObj["rooms"] = roomArray;

    // 4. Batches
    QJsonArray batchArray;
    for (const auto& b : m_appManager.getBatches()) {
        QJsonObject bObj;
        bObj["batchId"]    = QString::fromStdString(b.getBatchId());
        bObj["strength"]   = b.getStrength();
        bObj["program"]    = static_cast<int>(b.getProgram());
        bObj["department"] = QString::fromStdString(b.getDepartment());
        batchArray.append(bObj);
    }
    rootObj["batches"] = batchArray;

    // 5. Timetable (Class Sessions)
    QJsonArray sessionArray;
    for (const auto& s : m_appManager.getTimetable()) {
        QJsonObject sObj;
        TimeSlot ts = s.getTimeSlot();
        sObj["day"]        = static_cast<int>(ts.getDay());
        sObj["startH"]     = ts.getStartTime().hours;
        sObj["startM"]     = ts.getStartTime().minutes;
        sObj["endH"]       = ts.getEndTime().hours;
        sObj["endM"]       = ts.getEndTime().minutes;
        sObj["instructor"] = QString::fromStdString(s.getTeacherId()->getId());
        sObj["course"]     = QString::fromStdString(s.getSubjectId()->getCourseCode());
        sObj["room"]       = QString::fromStdString(s.getRoomId()->getRoomId());
        sObj["batch"]      = QString::fromStdString(s.getBatchId()->getBatchId());
        sessionArray.append(sObj);
    }
    rootObj["timetable"] = sessionArray;

    QJsonDocument doc(rootObj);
    file.write(doc.toJson());
    file.close();
}

// ==========================================
// loadFromFile
// ==========================================
void MainWindow::loadFromFile()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/timetable_data.json";
    if (!QFile::exists(filePath)) {
        populateInitialData();
        saveToFile();
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        populateInitialData();
        saveToFile();
        return;
    }

    QByteArray saveData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(saveData);
    if (doc.isNull() || !doc.isObject()) {
        populateInitialData();
        saveToFile();
        return;
    }

    QJsonObject rootObj = doc.object();

    // 1. Courses (loaded first so instructors can reference them)
    if (rootObj.contains("courses") && rootObj["courses"].isArray()) {
        QJsonArray courseArray = rootObj["courses"].toArray();
        for (const auto& val : courseArray) {
            QJsonObject crsObj = val.toObject();
            std::string code = crsObj.contains("code")
                               ? crsObj["code"].toString().toStdString()
                               : crsObj["courseCode"].toString().toStdString();
            std::string name = crsObj.contains("name") ? crsObj["name"].toString().toStdString() : code;
            int creditHours  = crsObj.contains("creditHours")
                               ? crsObj["creditHours"].toInt()
                               : crsObj["allocatedHours"].toInt();
            m_appManager.addCourse(Course(code, name, creditHours));
        }
    }

    // 2. Instructors
    if (rootObj.contains("instructors") && rootObj["instructors"].isArray()) {
        QJsonArray instArray = rootObj["instructors"].toArray();
        for (const auto& val : instArray) {
            QJsonObject instObj = val.toObject();
            std::string id   = instObj.contains("id")
                               ? instObj["id"].toString().toStdString()
                               : instObj["name"].toString().toStdString();
            std::string name = instObj["name"].toString().toStdString();
            int maxHours     = instObj["maxLimitHours"].toInt();

            Instructor inst(id, name, maxHours);

            // Restore locked subjects (permanent) — backward-compat: key may be absent
            if (instObj.contains("lockedSubjects") && instObj["lockedSubjects"].isArray()) {
                std::vector<std::string> locked;
                for (const auto& lv : instObj["lockedSubjects"].toArray())
                    locked.push_back(lv.toString().toStdString());
                inst.setLockedSubjects(locked);
            }

            // Restore runtime-assigned courses (hour tracking)
            if (instObj.contains("assignedCourses") && instObj["assignedCourses"].isArray()) {
                for (const auto& cVal : instObj["assignedCourses"].toArray()) {
                    std::string courseCode = cVal.toString().toStdString();
                    Course* crs = m_appManager.findCourseByCode(courseCode);
                    if (crs) inst.assignNewCourse(*crs);
                }
            }
            m_appManager.addInstructor(inst);
        }
    }

    // 3. Rooms
    if (rootObj.contains("rooms") && rootObj["rooms"].isArray()) {
        QJsonArray roomArray = rootObj["rooms"].toArray();
        for (const auto& val : roomArray) {
            QJsonObject rmObj = val.toObject();
            std::string id    = rmObj["roomId"].toString().toStdString();
            int cap           = rmObj["capacity"].toInt();
            RoomType type     = static_cast<RoomType>(rmObj["type"].toInt());
            std::string building = rmObj.contains("building")
                                   ? rmObj["building"].toString().toStdString()
                                   : "Main";
            m_appManager.addRoom(Room(id, cap, type, building));
        }
    }

    // 4. Batches
    if (rootObj.contains("batches") && rootObj["batches"].isArray()) {
        QJsonArray batchArray = rootObj["batches"].toArray();
        for (const auto& val : batchArray) {
            QJsonObject bObj = val.toObject();
            std::string id   = bObj["batchId"].toString().toStdString();
            int strength     = bObj["strength"].toInt();
            ProgramType prog = static_cast<ProgramType>(bObj["program"].toInt());
            std::string department = bObj.contains("department")
                                     ? bObj["department"].toString().toStdString()
                                     : "CS";
            m_appManager.addBatch(StudentBatch(id, strength, prog, department));
        }
    }

    // 5. Timetable (Class Sessions)
    if (rootObj.contains("timetable") && rootObj["timetable"].isArray()) {
        QJsonArray sessionArray = rootObj["timetable"].toArray();
        for (const auto& val : sessionArray) {
            QJsonObject sObj = val.toObject();
            Day day = static_cast<Day>(sObj["day"].toInt());
            ClockTime ctStart{ sObj["startH"].toInt(), sObj["startM"].toInt() };
            ClockTime ctEnd{   sObj["endH"].toInt(),   sObj["endM"].toInt()   };
            TimeSlot slot(day, ctStart, ctEnd);

            std::string instKey    = sObj["instructor"].toString().toStdString();
            std::string courseCode = sObj["course"].toString().toStdString();
            std::string roomId     = sObj["room"].toString().toStdString();
            std::string batchId    = sObj["batch"].toString().toStdString();

            Instructor*   inst = m_appManager.findInstructorById(instKey);
            if (!inst)    inst = m_appManager.findInstructorByName(instKey);
            Course*       crs  = m_appManager.findCourseByCode(courseCode);
            Room*         rm   = m_appManager.findRoomById(roomId);
            StudentBatch* btch = m_appManager.findBatchById(batchId);

            if (inst && crs && rm && btch) {
                m_appManager.validateAndAddClassSession(ClassSession(slot, inst, crs, rm, btch));
            }
        }
    }

    // Populate list widgets
    refreshInstList();
    for (const auto& crs : m_appManager.getCourses()) {
        m_courseList->addItem(QString("%1 (Allocated Hours: %2)")
            .arg(QString::fromStdString(crs.getCourseCode()))
            .arg(crs.getAllocatedHours()));
    }
    for (const auto& rm : m_appManager.getRooms()) {
        m_roomList->addItem(QString("%1 (Capacity: %2, Type: %3)")
            .arg(QString::fromStdString(rm.getRoomId()))
            .arg(rm.getCapacity())
            .arg(QString::fromStdString(rm.getTypeAsString())));
    }
    for (const auto& b : m_appManager.getBatches()) {
        m_batchList->addItem(QString("%1 (Strength: %2, Program: %3)")
            .arg(QString::fromStdString(b.getBatchId()))
            .arg(b.getStrength())
            .arg(QString::fromStdString(b.getProgramAsString())));
    }

    // Rebuild the subject combos so they're populated after courses are loaded
    rebuildSubjectCombos(m_instSubjectCountSpin->value());
}

// ==========================================
// populateCombos
// ==========================================
void MainWindow::populateCombos()
{
    m_sessInstCombo->clear();
    m_sessCourseCombo->clear();
    m_sessRoomCombo->clear();
    m_sessBatchCombo->clear();

    // Instructor entries now include " — Subjects: ... [LOCKED]" suffix.
    // We want just the name (stored as inst.getName()) in the session combo.
    for (const auto& inst : m_appManager.getInstructors()) {
        m_sessInstCombo->addItem(QString::fromStdString(inst.getName()));
    }
    for (int i = 0; i < m_courseList->count(); ++i) {
        QString text = m_courseList->item(i)->text();
        int idx = text.indexOf(" (Allocated Hours:");
        if (idx != -1) m_sessCourseCombo->addItem(text.left(idx));
    }
    for (int i = 0; i < m_roomList->count(); ++i) {
        QString text = m_roomList->item(i)->text();
        int idx = text.indexOf(" (Capacity:");
        if (idx != -1) m_sessRoomCombo->addItem(text.left(idx));
    }
    for (int i = 0; i < m_batchList->count(); ++i) {
        QString text = m_batchList->item(i)->text();
        int idx = text.indexOf(" (Strength:");
        if (idx != -1) m_sessBatchCombo->addItem(text.left(idx));
    }

    // Also refresh the subject combos in the instructor form so newly-added
    // courses show up.
    rebuildSubjectCombos(m_instSubjectCountSpin->value());
}

// ==========================================
// refreshListsAndTables
// ==========================================
void MainWindow::refreshListsAndTables()
{
    const auto& timetable = m_appManager.getTimetable();
    m_timetableTable->setRowCount(0);
    for (const auto& session : timetable) {
        int row = m_timetableTable->rowCount();
        m_timetableTable->insertRow(row);

        TimeSlot ts = session.getTimeSlot();
        QString timeStr = formatClockTime(ts.getStartTime()) + " - " + formatClockTime(ts.getEndTime());

        m_timetableTable->setItem(row, 0, new QTableWidgetItem(dayToString(ts.getDay())));
        m_timetableTable->setItem(row, 1, new QTableWidgetItem(timeStr));
        m_timetableTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(session.getSubjectId()->getCourseCode())));
        m_timetableTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(session.getTeacherId()->getName())));
        m_timetableTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(session.getRoomId()->getRoomId())));
        m_timetableTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(session.getBatchId()->getBatchId())));
        m_timetableTable->setItem(row, 6, new QTableWidgetItem(QString("%1 mins").arg(ts.getDurationmin())));
    }
}

// ==========================================
// INSTRUCTOR CRUD
// ==========================================
void MainWindow::onAddInstructor()
{
    std::string id      = m_instIdEdit->text().trimmed().toStdString();
    std::string name    = m_instNameEdit->text().trimmed().toStdString();
    int maxHours        = m_instHoursSpin->value();

    // -- Basic validation --
    if (id.empty()) {
        QMessageBox::warning(this, "Validation Error", "Instructor ID cannot be empty.");
        return;
    }
    if (name.empty()) {
        QMessageBox::warning(this, "Validation Error", "Instructor Name cannot be empty.");
        return;
    }

    bool isEditing = !m_editingInstId.empty();

    // Uniqueness check (only when adding a brand-new instructor)
    if (!isEditing) {
        if (m_appManager.findInstructorById(id) != nullptr) {
            QMessageBox::warning(this, "Validation Error", "Instructor ID must be unique.");
            return;
        }
    }

    // -- Collect subject selections --
    std::vector<std::string> lockedSubjects;

    if (isEditing && m_appManager.isInstructorUsed(m_editingInstId)) {
        // Subject section is read-only in this case; keep existing locked subjects
        Instructor* existing = m_appManager.findInstructorById(m_editingInstId);
        if (existing) lockedSubjects = existing->getLockedSubjects();
    } else {
        // Gather from the dynamic combos — validate all are selected
        for (int i = 0; i < m_instSubjectCombos.size(); ++i) {
            QComboBox* cb = m_instSubjectCombos[i];
            if (cb->currentIndex() == 0) {
                QMessageBox::warning(this, "Validation Error",
                    QString("Please select a course for Subject %1.").arg(i + 1));
                return;
            }
            QString selected = cb->currentText();
            std::string code = selected.toStdString();

            // Enforce distinct selections
            for (const auto& already : lockedSubjects) {
                if (already == code) {
                    QMessageBox::warning(this, "Validation Error",
                        QString("Subject %1 (\"%2\") is already selected. Each subject must be distinct.")
                            .arg(i + 1).arg(selected));
                    return;
                }
            }
            lockedSubjects.push_back(code);
        }
        if (lockedSubjects.empty()) {
            QMessageBox::warning(this, "Validation Error",
                "At least one subject must be assigned to the instructor.");
            return;
        }
    }

    // -- Create / update the instructor --
    Instructor inst(id, name, maxHours);
    inst.setLockedSubjects(lockedSubjects);

    if (isEditing) {
        // Preserve runtime-assigned courses from the existing record
        Instructor* existing = m_appManager.findInstructorById(m_editingInstId);
        if (existing) {
            for (const auto& c : existing->getAssignedCourses())
                inst.assignNewCourse(c);
        }
        m_appManager.updateInstructor(inst);
        m_editingInstId = "";
        m_instIdEdit->setEnabled(true);
        m_instSubjectCountSpin->setEnabled(true);
    } else {
        m_appManager.addInstructor(inst);
    }

    // -- Reset form --
    m_instIdEdit->clear();
    m_instNameEdit->clear();
    m_instHoursSpin->setValue(20);
    m_instSubjectCountSpin->setValue(1);
    m_instSubjectCountSpin->setEnabled(true);
    rebuildSubjectCombos(1);

    refreshInstList();
    saveToFile();
    populateCombos();
}

void MainWindow::onEditInstructor()
{
    QListWidgetItem *item = m_instList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No instructor selected to edit.");
        return;
    }

    // The display string starts with the instructor name before " (Max Hours:"
    QString text = item->text();
    // Name is everything before " (Max Hours:"
    int parenIdx = text.indexOf(" (Max Hours:");
    QString displayName = (parenIdx != -1) ? text.left(parenIdx) : text;

    // Find by name (the ID may differ; we stored name as display text)
    Instructor* inst = m_appManager.findInstructorByName(displayName.toStdString());
    if (!inst) {
        // Fallback: try by ID in case name == id
        inst = m_appManager.findInstructorById(displayName.toStdString());
    }
    if (!inst) {
        QMessageBox::warning(this, "Error", "Selected instructor not found.");
        return;
    }

    m_instIdEdit->setText(QString::fromStdString(inst->getId()));
    m_instNameEdit->setText(QString::fromStdString(inst->getName()));
    m_instHoursSpin->setValue(inst->getMaxLimitHours());

    m_editingInstId = inst->getId();
    m_instIdEdit->setEnabled(false);  // ID cannot change once set

    const auto& locked = inst->getLockedSubjects();
    bool isUsed = m_appManager.isInstructorUsed(inst->getId());

    if (isUsed) {
        // Subject list is frozen — show as read-only
        m_instSubjectCountSpin->setValue(static_cast<int>(locked.size() > 0 ? locked.size() : 1));
        m_instSubjectCountSpin->setEnabled(false);

        // Rebuild combos, select existing values, then disable them
        rebuildSubjectCombos(static_cast<int>(locked.size() > 0 ? locked.size() : 1));
        for (int i = 0; i < m_instSubjectCombos.size() && i < static_cast<int>(locked.size()); ++i) {
            QString code = QString::fromStdString(locked[i]);
            int idx = m_instSubjectCombos[i]->findText(code);
            if (idx != -1) m_instSubjectCombos[i]->setCurrentIndex(idx);
            m_instSubjectCombos[i]->setEnabled(false);
        }

        QMessageBox::information(this, "Subject List Locked",
            "This instructor has scheduled sessions. Name and Max Hours can still be edited,\n"
            "but the subject list is locked and cannot be changed.");
    } else {
        // No sessions yet — allow free editing of subjects
        int subCount = static_cast<int>(locked.size() > 0 ? locked.size() : 1);
        m_instSubjectCountSpin->setValue(subCount);
        m_instSubjectCountSpin->setEnabled(true);
        rebuildSubjectCombos(subCount);

        for (int i = 0; i < m_instSubjectCombos.size() && i < static_cast<int>(locked.size()); ++i) {
            QString code = QString::fromStdString(locked[i]);
            int idx = m_instSubjectCombos[i]->findText(code);
            if (idx != -1) m_instSubjectCombos[i]->setCurrentIndex(idx);
        }
    }
}

void MainWindow::onDeleteInstructor()
{
    QListWidgetItem *item = m_instList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No instructor selected to delete.");
        return;
    }

    QString text = item->text();
    int parenIdx = text.indexOf(" (Max Hours:");
    QString displayName = (parenIdx != -1) ? text.left(parenIdx) : text;

    Instructor* inst = m_appManager.findInstructorByName(displayName.toStdString());
    if (!inst) inst = m_appManager.findInstructorById(displayName.toStdString());
    if (!inst) {
        QMessageBox::warning(this, "Error", "Selected instructor not found.");
        return;
    }
    std::string id = inst->getId();

    if (m_appManager.isInstructorUsed(id)) {
        QMessageBox::critical(this, "Cannot Delete",
            "This item is currently used in one or more scheduled classes and cannot be deleted.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this instructor?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    if (m_appManager.removeInstructor(id)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete instructor.");
    }
}

// ==========================================
// COURSE CRUD
// ==========================================
void MainWindow::onAddCourse()
{
    std::string code = m_courseCodeEdit->text().trimmed().toStdString();
    std::string name = m_courseNameEdit->text().trimmed().toStdString();
    int hours        = m_courseHoursSpin->value();

    if (code.empty()) {
        QMessageBox::warning(this, "Validation Error", "Course Code cannot be empty.");
        return;
    }
    if (m_editingCourseCode.empty() || m_editingCourseCode != code) {
        if (m_appManager.findCourseByCode(code) != nullptr) {
            QMessageBox::warning(this, "Validation Error", "Course Code must be unique.");
            return;
        }
    }
    if (name.empty()) {
        QMessageBox::warning(this, "Validation Error", "Course Name cannot be empty.");
        return;
    }
    if (hours <= 0) {
        QMessageBox::warning(this, "Validation Error", "Credit Hours must be a positive integer.");
        return;
    }

    Course crs(code, name, hours);

    if (!m_editingCourseCode.empty()) {
        m_appManager.updateCourse(crs);
        m_editingCourseCode = "";
        m_courseCodeEdit->setEnabled(true);
    } else {
        m_appManager.addCourse(crs);
    }

    m_courseCodeEdit->clear();
    m_courseNameEdit->clear();
    m_courseHoursSpin->setValue(3);

    m_courseList->clear();
    for (const auto& c : m_appManager.getCourses()) {
        m_courseList->addItem(QString("%1 (Allocated Hours: %2)")
            .arg(QString::fromStdString(c.getCourseCode()))
            .arg(c.getAllocatedHours()));
    }

    saveToFile();
    populateCombos();
}

void MainWindow::onEditCourse()
{
    QListWidgetItem *item = m_courseList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No course selected to edit.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Allocated Hours:");
    std::string code = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    Course* crs = m_appManager.findCourseByCode(code);
    if (!crs) {
        QMessageBox::warning(this, "Error", "Selected course not found.");
        return;
    }

    m_courseCodeEdit->setText(QString::fromStdString(crs->getCourseCode()));
    m_courseNameEdit->setText(QString::fromStdString(crs->getName()));
    m_courseHoursSpin->setValue(crs->getAllocatedHours());

    m_editingCourseCode = crs->getCourseCode();
    m_courseCodeEdit->setEnabled(false);
}

void MainWindow::onDeleteCourse()
{
    QListWidgetItem *item = m_courseList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No course selected to delete.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Allocated Hours:");
    std::string code = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    if (m_appManager.isCourseUsed(code)) {
        QMessageBox::critical(this, "Cannot Delete",
            "This item is currently used in one or more scheduled classes and cannot be deleted.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this course?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    if (m_appManager.removeCourse(code)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete course.");
    }
}

// ==========================================
// ROOM CRUD
// ==========================================
void MainWindow::onAddRoom()
{
    std::string number   = m_roomIdEdit->text().trimmed().toStdString();
    std::string building = m_roomBuildingEdit->text().trimmed().toStdString();
    int capacity         = m_roomCapSpin->value();
    RoomType type        = static_cast<RoomType>(m_roomTypeCombo->currentIndex());

    if (number.empty()) {
        QMessageBox::warning(this, "Validation Error", "Room Number cannot be empty.");
        return;
    }
    if (m_editingRoomId.empty() || m_editingRoomId != number) {
        if (m_appManager.findRoomById(number) != nullptr) {
            QMessageBox::warning(this, "Validation Error", "Room Number must be unique.");
            return;
        }
    }
    if (building.empty()) {
        QMessageBox::warning(this, "Validation Error", "Building cannot be empty.");
        return;
    }
    if (capacity <= 0) {
        QMessageBox::warning(this, "Validation Error", "Capacity must be a positive integer.");
        return;
    }

    Room rm(number, capacity, type, building);

    if (!m_editingRoomId.empty()) {
        m_appManager.updateRoom(rm);
        m_editingRoomId = "";
        m_roomIdEdit->setEnabled(true);
    } else {
        m_appManager.addRoom(rm);
    }

    m_roomIdEdit->clear();
    m_roomBuildingEdit->clear();
    m_roomCapSpin->setValue(60);
    m_roomTypeCombo->setCurrentIndex(0);

    m_roomList->clear();
    for (const auto& r : m_appManager.getRooms()) {
        m_roomList->addItem(QString("%1 (Capacity: %2, Type: %3)")
            .arg(QString::fromStdString(r.getRoomId()))
            .arg(r.getCapacity())
            .arg(QString::fromStdString(r.getTypeAsString())));
    }

    saveToFile();
    populateCombos();
}

void MainWindow::onEditRoom()
{
    QListWidgetItem *item = m_roomList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No room selected to edit.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Capacity:");
    std::string number = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    Room* rm = m_appManager.findRoomById(number);
    if (!rm) {
        QMessageBox::warning(this, "Error", "Selected room not found.");
        return;
    }

    m_roomIdEdit->setText(QString::fromStdString(rm->getRoomId()));
    m_roomBuildingEdit->setText(QString::fromStdString(rm->getBuilding()));
    m_roomCapSpin->setValue(rm->getCapacity());
    m_roomTypeCombo->setCurrentIndex(static_cast<int>(rm->getType()));

    m_editingRoomId = rm->getRoomId();
    m_roomIdEdit->setEnabled(false);
}

void MainWindow::onDeleteRoom()
{
    QListWidgetItem *item = m_roomList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No room selected to delete.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Capacity:");
    std::string number = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    if (m_appManager.isRoomUsed(number)) {
        QMessageBox::critical(this, "Cannot Delete",
            "This item is currently used in one or more scheduled classes and cannot be deleted.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this room?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    if (m_appManager.removeRoom(number)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete room.");
    }
}

// ==========================================
// BATCH CRUD
// ==========================================
void MainWindow::onAddBatch()
{
    std::string name = m_batchIdEdit->text().trimmed().toStdString();
    std::string dept = m_batchDeptEdit->text().trimmed().toStdString();
    int strength     = m_batchStrengthSpin->value();
    ProgramType program = static_cast<ProgramType>(m_batchProgCombo->currentIndex());

    if (name.empty()) {
        QMessageBox::warning(this, "Validation Error", "Batch Name cannot be empty.");
        return;
    }
    if (m_editingBatchId.empty() || m_editingBatchId != name) {
        if (m_appManager.findBatchById(name) != nullptr) {
            QMessageBox::warning(this, "Validation Error", "Batch Name must be unique.");
            return;
        }
    }
    if (dept.empty()) {
        QMessageBox::warning(this, "Validation Error", "Department cannot be empty.");
        return;
    }

    StudentBatch b(name, strength, program, dept);

    if (!m_editingBatchId.empty()) {
        m_appManager.updateBatch(b);
        m_editingBatchId = "";
        m_batchIdEdit->setEnabled(true);
    } else {
        m_appManager.addBatch(b);
    }

    m_batchIdEdit->clear();
    m_batchDeptEdit->clear();
    m_batchStrengthSpin->setValue(45);
    m_batchProgCombo->setCurrentIndex(0);

    m_batchList->clear();
    for (const auto& bat : m_appManager.getBatches()) {
        m_batchList->addItem(QString("%1 (Strength: %2, Program: %3)")
            .arg(QString::fromStdString(bat.getBatchId()))
            .arg(bat.getStrength())
            .arg(QString::fromStdString(bat.getProgramAsString())));
    }

    saveToFile();
    populateCombos();
}

void MainWindow::onEditBatch()
{
    QListWidgetItem *item = m_batchList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No student batch selected to edit.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Strength:");
    std::string name = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    StudentBatch* b = m_appManager.findBatchById(name);
    if (!b) {
        QMessageBox::warning(this, "Error", "Selected student batch not found.");
        return;
    }

    m_batchIdEdit->setText(QString::fromStdString(b->getBatchId()));
    m_batchDeptEdit->setText(QString::fromStdString(b->getDepartment()));
    m_batchStrengthSpin->setValue(b->getStrength());
    m_batchProgCombo->setCurrentIndex(static_cast<int>(b->getProgram()));

    m_editingBatchId = b->getBatchId();
    m_batchIdEdit->setEnabled(false);
}

void MainWindow::onDeleteBatch()
{
    QListWidgetItem *item = m_batchList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No student batch selected to delete.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Strength:");
    std::string name = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    if (m_appManager.isBatchUsed(name)) {
        QMessageBox::critical(this, "Cannot Delete",
            "This item is currently used in one or more scheduled classes and cannot be deleted.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to delete this student batch?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    if (m_appManager.removeBatch(name)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete student batch.");
    }
}

// ==========================================
// SESSION CRUD
// ==========================================
void MainWindow::onAddClassSession()
{
    QString instName   = m_sessInstCombo->currentText();
    QString courseCode = m_sessCourseCombo->currentText();
    QString roomId     = m_sessRoomCombo->currentText();
    QString batchId    = m_sessBatchCombo->currentText();

    if (instName.isEmpty() || courseCode.isEmpty() || roomId.isEmpty() || batchId.isEmpty()) {
        QMessageBox::warning(this, "Selection Error",
            "Please ensure all entities (Instructors, Courses, Rooms, Batches) are created and selected.");
        return;
    }

    Instructor*   inst = m_appManager.findInstructorByName(instName.toStdString());
    if (!inst)    inst = m_appManager.findInstructorById(instName.toStdString());
    Course*       crs  = m_appManager.findCourseByCode(courseCode.toStdString());
    Room*         rm   = m_appManager.findRoomById(roomId.toStdString());
    StudentBatch* btch = m_appManager.findBatchById(batchId.toStdString());

    if (!inst || !crs || !rm || !btch) {
        QMessageBox::critical(this, "System Error",
            "Failed to retrieve references for the selected objects.");
        return;
    }

    // -- Subject qualification guard --
    if (!inst->isQualifiedFor(crs->getCourseCode())) {
        QStringList lockedList;
        for (const auto& s : inst->getLockedSubjects())
            lockedList << QString::fromStdString(s);
        QMessageBox::warning(this, "Subject Not Assigned",
            QString("Instructor \"%1\" is not qualified to teach \"%2\".\n\n"
                    "Assigned subjects: %3")
                .arg(QString::fromStdString(inst->getName()))
                .arg(QString::fromStdString(crs->getCourseCode()))
                .arg(lockedList.isEmpty() ? "(none)" : lockedList.join(", ")));
        return;
    }

    QTime startTime = m_sessStartEdit->time();
    QTime endTime   = m_sessEndEdit->time();

    if (startTime >= endTime) {
        QMessageBox::warning(this, "Time Range Error", "End time must be strictly after start time.");
        return;
    }

    if (startTime < QTime(9, 0) || endTime > QTime(17, 0)) {
        QMessageBox::warning(this, "Time Range Error", "Classes must be scheduled between 9:00 AM and 5:00 PM.");
        return;
    }

    Day day = static_cast<Day>(m_sessDayCombo->currentIndex());
    ClockTime ctStart{ startTime.hour(), startTime.minute() };
    ClockTime ctEnd{   endTime.hour(),   endTime.minute()   };
    TimeSlot slot(day, ctStart, ctEnd);

    // Soft constraint: 1 hr break after every 2 classes
    std::vector<TimeSlot> batchDaySlots;
    batchDaySlots.push_back(slot);
    for (const auto& existing : m_appManager.getTimetable()) {
        if (existing.getBatchId()->getBatchId() == btch->getBatchId() &&
            existing.getTimeSlot().getDay() == day) {
            batchDaySlots.push_back(existing.getTimeSlot());
        }
    }
    
    std::sort(batchDaySlots.begin(), batchDaySlots.end(), [](const TimeSlot& a, const TimeSlot& b) {
        int aStart = a.getStartTime().hours * 60 + a.getStartTime().minutes;
        int bStart = b.getStartTime().hours * 60 + b.getStartTime().minutes;
        return aStart < bStart;
    });

    bool needsBreakWarning = false;
    int consecutiveClasses = 1;
    for (size_t i = 1; i < batchDaySlots.size(); ++i) {
        int prevEnd = batchDaySlots[i-1].getEndTime().hours * 60 + batchDaySlots[i-1].getEndTime().minutes;
        int currStart = batchDaySlots[i].getStartTime().hours * 60 + batchDaySlots[i].getStartTime().minutes;
        
        if (currStart - prevEnd < 60) {
            consecutiveClasses++;
            if (consecutiveClasses > 2) {
                needsBreakWarning = true;
                break;
            }
        } else {
            consecutiveClasses = 1;
        }
    }

    if (needsBreakWarning) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Batch Overload Warning",
            "This schedule places 3 or more classes for the student batch without a 1-hour break in between.\n\n"
            "Do you want to proceed anyway?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) return;
    }

    if (!inst->assignNewCourse(*crs)) {
        int currentHours = inst->calculateTotalAssignedHours();
        QMessageBox::warning(this, "Workload Limit Exceeded",
            QString("Cannot schedule: %1 would exceed weekly hour limit!\n\n"
                    "Instructor: %2\n"
                    "Max Weekly Limit: %3 hours\n"
                    "Assigned so far: %4 hours\n"
                    "Course to assign: %5 (%6 hours)")
            .arg(QString::fromStdString(crs->getCourseCode()))
            .arg(QString::fromStdString(inst->getName()))
            .arg(inst->getMaxLimitHours())
            .arg(currentHours)
            .arg(QString::fromStdString(crs->getCourseCode()))
            .arg(crs->getAllocatedHours()));
        return;
    }

    ClassSession session(slot, inst, crs, rm, btch);
    std::string err = m_appManager.validateAndAddClassSession(session);
    if (!err.empty()) {
        inst->unassignCourse(crs->getCourseCode());
        QMessageBox::warning(this, "Scheduling Constraint Violation", QString::fromStdString(err));
        return;
    }

    saveToFile();
    refreshListsAndTables();

    QMessageBox::information(this, "Schedule Succeeded", "Class session scheduled successfully!");
}

void MainWindow::onDeleteClassSession()
{
    int row = m_timetableTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Selection Error", "No class session selected to delete.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
        "Are you sure you want to remove this scheduled session?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    if (m_appManager.removeClassSession(row)) {
        saveToFile();
        refreshListsAndTables();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to remove the selected session.");
    }
}

void MainWindow::onAutoGenerate()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Auto Generate Routine",
        "This will clear the current timetable and automatically generate a new routine "
        "for all batches across Monday to Friday.\n\nProceed?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    m_appManager.autoGenerateTimetable();

    saveToFile();
    refreshListsAndTables();

    int total = static_cast<int>(m_appManager.getTimetable().size());
    QMessageBox::information(this, "Generation Complete",
        QString("Routine generated successfully!\n\n"
                "Total sessions scheduled: %1\n"
                "Days: Monday - Friday\n"
                "Timeslots: 9:00-11:00 & 12:00-17:00 (Lunch: 11:00-12:00)")
        .arg(total));
}
