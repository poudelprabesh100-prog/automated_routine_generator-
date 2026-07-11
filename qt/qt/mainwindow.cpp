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
#include <QRegularExpression>


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
    
    m_instIdEdit = new QLineEdit();
    m_instNameEdit = new QLineEdit();
    m_instDeptEdit = new QLineEdit();
    m_instEmailEdit = new QLineEdit();
    m_instPhoneEdit = new QLineEdit();
    m_instHoursSpin = new QSpinBox();
    m_instHoursSpin->setRange(1, 100);
    m_instHoursSpin->setValue(20);
    
    instForm->addRow(new QLabel("Instructor ID:"), m_instIdEdit);
    instForm->addRow(new QLabel("Instructor Name:"), m_instNameEdit);
    instForm->addRow(new QLabel("Department:"), m_instDeptEdit);
    instForm->addRow(new QLabel("Email:"), m_instEmailEdit);
    instForm->addRow(new QLabel("Phone:"), m_instPhoneEdit);
    instForm->addRow(new QLabel("Max Weekly Hours:"), m_instHoursSpin);
    
    QPushButton *btnInstAdd = new QPushButton("Add Instructor");
    QPushButton *btnInstEdit = new QPushButton("Edit Instructor");
    QPushButton *btnInstDelete = new QPushButton("Delete Instructor");
    
    connect(btnInstAdd, &QPushButton::clicked, this, &MainWindow::onAddInstructor);
    connect(btnInstEdit, &QPushButton::clicked, this, &MainWindow::onEditInstructor);
    connect(btnInstDelete, &QPushButton::clicked, this, &MainWindow::onDeleteInstructor);
    
    QHBoxLayout *instBtnLayout = new QHBoxLayout();
    instBtnLayout->addWidget(btnInstAdd);
    instBtnLayout->addWidget(btnInstEdit);
    instBtnLayout->addWidget(btnInstDelete);
    
    instFormLayout->addLayout(instForm);
    instFormLayout->addLayout(instBtnLayout);
    instFormLayout->addStretch();
    
    m_instList = new QListWidget();
    instLayout->addWidget(instLeft, 1);
    instLayout->addWidget(m_instList, 2);
    m_tabWidget->addTab(instTab, "Instructors");

    // ==========================================
    // 2. COURSES TAB
    // ==========================================
    QWidget *courseTab = new QWidget();
    QHBoxLayout *courseLayout = new QHBoxLayout(courseTab);
    
    QWidget *courseLeft = new QWidget();
    QVBoxLayout *courseFormLayout = new QVBoxLayout(courseLeft);
    QFormLayout *courseForm = new QFormLayout();
    
    m_courseCodeEdit = new QLineEdit();
    m_courseNameEdit = new QLineEdit();
    m_courseHoursSpin = new QSpinBox();
    m_courseHoursSpin->setRange(1, 20);
    m_courseHoursSpin->setValue(3);
    m_courseSemesterSpin = new QSpinBox();
    m_courseSemesterSpin->setRange(1, 12);
    m_courseSemesterSpin->setValue(1);
    m_courseDeptEdit = new QLineEdit();
    
    courseForm->addRow(new QLabel("Course Code:"), m_courseCodeEdit);
    courseForm->addRow(new QLabel("Course Name:"), m_courseNameEdit);
    courseForm->addRow(new QLabel("Credit Hours:"), m_courseHoursSpin);
    courseForm->addRow(new QLabel("Semester:"), m_courseSemesterSpin);
    courseForm->addRow(new QLabel("Department:"), m_courseDeptEdit);
    
    QPushButton *btnCourseAdd = new QPushButton("Add Course");
    QPushButton *btnCourseEdit = new QPushButton("Edit Course");
    QPushButton *btnCourseDelete = new QPushButton("Delete Course");
    
    connect(btnCourseAdd, &QPushButton::clicked, this, &MainWindow::onAddCourse);
    connect(btnCourseEdit, &QPushButton::clicked, this, &MainWindow::onEditCourse);
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
    
    m_roomIdEdit = new QLineEdit();
    m_roomBuildingEdit = new QLineEdit();
    m_roomCapSpin = new QSpinBox();
    m_roomCapSpin->setRange(1, 500);
    m_roomCapSpin->setValue(60);
    m_roomTypeCombo = new QComboBox();
    m_roomTypeCombo->addItems({"Theory", "Lab", "Auditorium"});
    
    roomForm->addRow(new QLabel("Room Number:"), m_roomIdEdit);
    roomForm->addRow(new QLabel("Building:"), m_roomBuildingEdit);
    roomForm->addRow(new QLabel("Capacity:"), m_roomCapSpin);
    roomForm->addRow(new QLabel("Room Type:"), m_roomTypeCombo);
    
    QPushButton *btnRoomAdd = new QPushButton("Add Room");
    QPushButton *btnRoomEdit = new QPushButton("Edit Room");
    QPushButton *btnRoomDelete = new QPushButton("Delete Room");
    
    connect(btnRoomAdd, &QPushButton::clicked, this, &MainWindow::onAddRoom);
    connect(btnRoomEdit, &QPushButton::clicked, this, &MainWindow::onEditRoom);
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
    
    batchForm->addRow(new QLabel("Batch Name:"), m_batchIdEdit);
    batchForm->addRow(new QLabel("Department:"), m_batchDeptEdit);
    batchForm->addRow(new QLabel("Strength:"), m_batchStrengthSpin);
    batchForm->addRow(new QLabel("Program:"), m_batchProgCombo);
    
    QPushButton *btnBatchAdd = new QPushButton("Add Batch");
    QPushButton *btnBatchEdit = new QPushButton("Edit Batch");
    QPushButton *btnBatchDelete = new QPushButton("Delete Batch");
    
    connect(btnBatchAdd, &QPushButton::clicked, this, &MainWindow::onAddBatch);
    connect(btnBatchEdit, &QPushButton::clicked, this, &MainWindow::onEditBatch);
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
    
    m_sessInstCombo = new QComboBox();
    m_sessCourseCombo = new QComboBox();
    m_sessRoomCombo = new QComboBox();
    m_sessBatchCombo = new QComboBox();
    
    m_sessDayCombo = new QComboBox();
    m_sessDayCombo->addItems({"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"});
    
    m_sessStartEdit = new QTimeEdit(QTime(9, 30));
    m_sessEndEdit = new QTimeEdit(QTime(11, 30));
    
    sessionForm->addRow(new QLabel("Instructor:"), m_sessInstCombo);
    sessionForm->addRow(new QLabel("Course:"), m_sessCourseCombo);
    sessionForm->addRow(new QLabel("Room:"), m_sessRoomCombo);
    sessionForm->addRow(new QLabel("Student Batch:"), m_sessBatchCombo);
    sessionForm->addRow(new QLabel("Day:"), m_sessDayCombo);
    sessionForm->addRow(new QLabel("Start Time:"), m_sessStartEdit);
    sessionForm->addRow(new QLabel("End Time:"), m_sessEndEdit);
    
    QPushButton *btnSessionAdd = new QPushButton("Schedule Class Session");
    connect(btnSessionAdd, &QPushButton::clicked, this, &MainWindow::onAddClassSession);
    
    timetableFormLayout->addLayout(sessionForm);
    timetableFormLayout->addWidget(btnSessionAdd);
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


void MainWindow::populateInitialData()
{
    // Populate some default data matching console main.cpp to make tests easy
    m_appManager.addInstructor(Instructor("Dr. Niraj Sharma", 12));
    m_appManager.addInstructor(Instructor("Prof. Ram Prasad", 15));
    
    m_appManager.addCourse(Course("COMP-102", 3));
    m_appManager.addCourse(Course("MATH-101", 4));
    
    m_appManager.addRoom(Room("Block-C-102", 60, RoomType::Theory));
    m_appManager.addRoom(Room("Lab-A-301", 40, RoomType::Lab));
    
    m_appManager.addBatch(StudentBatch("BCT-2025-A", 48, ProgramType::BCE));
    m_appManager.addBatch(StudentBatch("BIT-2025-B", 45, ProgramType::BIT));

    m_instList->addItem("Dr. Niraj Sharma (Max Hours: 12)");
    m_instList->addItem("Prof. Ram Prasad (Max Hours: 15)");
    
    m_courseList->addItem("COMP-102 (Allocated Hours: 3)");
    m_courseList->addItem("MATH-101 (Allocated Hours: 4)");
    
    m_roomList->addItem("Block-C-102 (Capacity: 60, Type: Theory)");
    m_roomList->addItem("Lab-A-301 (Capacity: 40, Type: Lab)");
    
    m_batchList->addItem("BCT-2025-A (Strength: 48, Program: BCE)");
    m_batchList->addItem("BIT-2025-B (Strength: 45, Program: BIT)");
}

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
        instObj["id"] = QString::fromStdString(inst.getId());
        instObj["name"] = QString::fromStdString(inst.getName());
        instObj["department"] = QString::fromStdString(inst.getDepartment());
        instObj["email"] = QString::fromStdString(inst.getEmail());
        instObj["phone"] = QString::fromStdString(inst.getPhone());
        instObj["maxLimitHours"] = inst.getMaxLimitHours();
        
        QJsonArray assignedArray;
        for (const auto& crs : inst.getAssignedCourses()) {
            assignedArray.append(QString::fromStdString(crs.getCourseCode()));
        }
        instObj["assignedCourses"] = assignedArray;
        instArray.append(instObj);
    }
    rootObj["instructors"] = instArray;

    // 2. Courses
    QJsonArray courseArray;
    for (const auto& crs : m_appManager.getCourses()) {
        QJsonObject crsObj;
        crsObj["code"] = QString::fromStdString(crs.getCode());
        crsObj["name"] = QString::fromStdString(crs.getName());
        crsObj["creditHours"] = crs.getCreditHours();
        crsObj["semester"] = crs.getSemester();
        crsObj["department"] = QString::fromStdString(crs.getDepartment());
        crsObj["courseCode"] = QString::fromStdString(crs.getCourseCode());
        crsObj["allocatedHours"] = crs.getAllocatedHours();
        courseArray.append(crsObj);
    }
    rootObj["courses"] = courseArray;

    // 3. Rooms
    QJsonArray roomArray;
    for (const auto& rm : m_appManager.getRooms()) {
        QJsonObject rmObj;
        rmObj["roomId"] = QString::fromStdString(rm.getRoomId());
        rmObj["capacity"] = rm.getCapacity();
        rmObj["type"] = static_cast<int>(rm.getType());
        rmObj["building"] = QString::fromStdString(rm.getBuilding());
        roomArray.append(rmObj);
    }
    rootObj["rooms"] = roomArray;

    // 4. Batches
    QJsonArray batchArray;
    for (const auto& b : m_appManager.getBatches()) {
        QJsonObject bObj;
        bObj["batchId"] = QString::fromStdString(b.getBatchId());
        bObj["strength"] = b.getStrength();
        bObj["program"] = static_cast<int>(b.getProgram());
        bObj["department"] = QString::fromStdString(b.getDepartment());
        batchArray.append(bObj);
    }
    rootObj["batches"] = batchArray;

    // 5. Timetable (Class Sessions)
    QJsonArray sessionArray;
    for (const auto& s : m_appManager.getTimetable()) {
        QJsonObject sObj;
        TimeSlot ts = s.getTimeSlot();
        sObj["day"] = static_cast<int>(ts.getDay());
        sObj["startH"] = ts.getStartTime().hours;
        sObj["startM"] = ts.getStartTime().minutes;
        sObj["endH"] = ts.getEndTime().hours;
        sObj["endM"] = ts.getEndTime().minutes;
        sObj["instructor"] = QString::fromStdString(s.getTeacherId()->getId()); // Use unique ID
        sObj["course"] = QString::fromStdString(s.getSubjectId()->getCourseCode());
        sObj["room"] = QString::fromStdString(s.getRoomId()->getRoomId());
        sObj["batch"] = QString::fromStdString(s.getBatchId()->getBatchId());
        sessionArray.append(sObj);
    }
    rootObj["timetable"] = sessionArray;

    QJsonDocument doc(rootObj);
    file.write(doc.toJson());
    file.close();
}

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

    // 1. Courses
    if (rootObj.contains("courses") && rootObj["courses"].isArray()) {
        QJsonArray courseArray = rootObj["courses"].toArray();
        for (const auto& val : courseArray) {
            QJsonObject crsObj = val.toObject();
            std::string code = crsObj.contains("code") ? crsObj["code"].toString().toStdString() : crsObj["courseCode"].toString().toStdString();
            std::string name = crsObj.contains("name") ? crsObj["name"].toString().toStdString() : code;
            int creditHours = crsObj.contains("creditHours") ? crsObj["creditHours"].toInt() : crsObj["allocatedHours"].toInt();
            int semester = crsObj.contains("semester") ? crsObj["semester"].toInt() : 1;
            std::string department = crsObj.contains("department") ? crsObj["department"].toString().toStdString() : "CS";
            m_appManager.addCourse(Course(code, name, creditHours, semester, department));
        }
    }

    // 2. Instructors
    if (rootObj.contains("instructors") && rootObj["instructors"].isArray()) {
        QJsonArray instArray = rootObj["instructors"].toArray();
        for (const auto& val : instArray) {
            QJsonObject instObj = val.toObject();
            std::string id = instObj.contains("id") ? instObj["id"].toString().toStdString() : instObj["name"].toString().toStdString();
            std::string name = instObj["name"].toString().toStdString();
            std::string department = instObj.contains("department") ? instObj["department"].toString().toStdString() : "CS";
            std::string email = instObj.contains("email") ? instObj["email"].toString().toStdString() : "";
            std::string phone = instObj.contains("phone") ? instObj["phone"].toString().toStdString() : "";
            int maxHours = instObj["maxLimitHours"].toInt();
            
            Instructor inst(id, name, department, email, phone, maxHours);
            
            // Re-assign courses
            if (instObj.contains("assignedCourses") && instObj["assignedCourses"].isArray()) {
                QJsonArray assignedArray = instObj["assignedCourses"].toArray();
                for (const auto& cVal : assignedArray) {
                    std::string courseCode = cVal.toString().toStdString();
                    Course* crs = m_appManager.findCourseByCode(courseCode);
                    if (crs) {
                        inst.assignNewCourse(*crs);
                    }
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
            std::string id = rmObj["roomId"].toString().toStdString();
            int cap = rmObj["capacity"].toInt();
            RoomType type = static_cast<RoomType>(rmObj["type"].toInt());
            std::string building = rmObj.contains("building") ? rmObj["building"].toString().toStdString() : "Main";
            m_appManager.addRoom(Room(id, cap, type, building));
        }
    }

    // 4. Batches
    if (rootObj.contains("batches") && rootObj["batches"].isArray()) {
        QJsonArray batchArray = rootObj["batches"].toArray();
        for (const auto& val : batchArray) {
            QJsonObject bObj = val.toObject();
            std::string id = bObj["batchId"].toString().toStdString();
            int strength = bObj["strength"].toInt();
            ProgramType prog = static_cast<ProgramType>(bObj["program"].toInt());
            std::string department = bObj.contains("department") ? bObj["department"].toString().toStdString() : "CS";
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
            ClockTime ctEnd{ sObj["endH"].toInt(), sObj["endM"].toInt() };
            TimeSlot slot(day, ctStart, ctEnd);

            std::string instKey = sObj["instructor"].toString().toStdString();
            std::string courseCode = sObj["course"].toString().toStdString();
            std::string roomId = sObj["room"].toString().toStdString();
            std::string batchId = sObj["batch"].toString().toStdString();

            // Try resolving instructor by ID first, then by name for compatibility
            Instructor* inst = m_appManager.findInstructorById(instKey);
            if (!inst) inst = m_appManager.findInstructorByName(instKey);
            
            Course* crs = m_appManager.findCourseByCode(courseCode);
            Room* rm = m_appManager.findRoomById(roomId);
            StudentBatch* btch = m_appManager.findBatchById(batchId);

            if (inst && crs && rm && btch) {
                m_appManager.addClassSession(ClassSession(slot, inst, crs, rm, btch));
            }
        }
    }

    // Populate list widgets based on loaded data
    for (const auto& inst : m_appManager.getInstructors()) {
        m_instList->addItem(QString("%1 (Max Hours: %2)")
            .arg(QString::fromStdString(inst.getId()))
            .arg(inst.getMaxLimitHours()));
    }
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
}


void MainWindow::populateCombos()
{
    m_sessInstCombo->clear();
    m_sessCourseCombo->clear();
    m_sessRoomCombo->clear();
    m_sessBatchCombo->clear();

    for(int i = 0; i < m_instList->count(); ++i) {
        QString text = m_instList->item(i)->text();
        int idx = text.indexOf(" (Max Hours:");
        if (idx != -1) {
            m_sessInstCombo->addItem(text.left(idx));
        }
    }

    for(int i = 0; i < m_courseList->count(); ++i) {
        QString text = m_courseList->item(i)->text();
        int idx = text.indexOf(" (Allocated Hours:");
        if (idx != -1) {
            m_sessCourseCombo->addItem(text.left(idx));
        }
    }

    for(int i = 0; i < m_roomList->count(); ++i) {
        QString text = m_roomList->item(i)->text();
        int idx = text.indexOf(" (Capacity:");
        if (idx != -1) {
            m_sessRoomCombo->addItem(text.left(idx));
        }
    }

    for(int i = 0; i < m_batchList->count(); ++i) {
        QString text = m_batchList->item(i)->text();
        int idx = text.indexOf(" (Strength:");
        if (idx != -1) {
            m_sessBatchCombo->addItem(text.left(idx));
        }
    }
}

void MainWindow::refreshListsAndTables()
{
    // Refresh Timetable Table
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

void MainWindow::onAddInstructor()
{
    std::string id = m_instIdEdit->text().trimmed().toStdString();
    std::string name = m_instNameEdit->text().trimmed().toStdString();
    std::string department = m_instDeptEdit->text().trimmed().toStdString();
    std::string email = m_instEmailEdit->text().trimmed().toStdString();
    std::string phone = m_instPhoneEdit->text().trimmed().toStdString();
    int maxHours = m_instHoursSpin->value();

    if (id.empty()) {
        QMessageBox::warning(this, "Validation Error", "Instructor ID cannot be empty.");
        return;
    }

    if (m_editingInstId.empty() || m_editingInstId != id) {
        if (m_appManager.findInstructorById(id) != nullptr) {
            QMessageBox::warning(this, "Validation Error", "Instructor ID must be unique.");
            return;
        }
    }

    if (name.empty()) {
        QMessageBox::warning(this, "Validation Error", "Instructor Name cannot be empty.");
        return;
    }

    if (department.empty()) {
        QMessageBox::warning(this, "Validation Error", "Department cannot be empty.");
        return;
    }

    if (!email.empty()) {
        QRegularExpression emailRegex("^[\\w\\.\\-\\+]+@[\\w\\.\\-]+\\.[a-zA-Z]{2,}$");
        if (!emailRegex.match(QString::fromStdString(email)).hasMatch()) {
            QMessageBox::warning(this, "Validation Error", "Email format must be name@domain.com.");
            return;
        }
    }

    if (!phone.empty()) {
        QRegularExpression phoneRegex("^[0-9\\s\\+\\-]+$");
        if (!phoneRegex.match(QString::fromStdString(phone)).hasMatch()) {
            QMessageBox::warning(this, "Validation Error", "Phone may only contain digits, spaces, +, or -.");
            return;
        }
    }

    Instructor inst(id, name, department, email, phone, maxHours);

    if (!m_editingInstId.empty()) {
        m_appManager.updateInstructor(inst);
        m_editingInstId = "";
        m_instIdEdit->setEnabled(true);
    } else {
        m_appManager.addInstructor(inst);
    }

    m_instIdEdit->clear();
    m_instNameEdit->clear();
    m_instDeptEdit->clear();
    m_instEmailEdit->clear();
    m_instPhoneEdit->clear();
    m_instHoursSpin->setValue(20);

    m_instList->clear();
    for (const auto& i : m_appManager.getInstructors()) {
        m_instList->addItem(QString("%1 (Max Hours: %2)").arg(QString::fromStdString(i.getId())).arg(i.getMaxLimitHours()));
    }

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

    QString text = item->text();
    int idx = text.indexOf(" (Max Hours:");
    std::string id = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    Instructor* inst = m_appManager.findInstructorById(id);
    if (!inst) {
        QMessageBox::warning(this, "Error", "Selected instructor not found.");
        return;
    }

    m_instIdEdit->setText(QString::fromStdString(inst->getId()));
    m_instNameEdit->setText(QString::fromStdString(inst->getName()));
    m_instDeptEdit->setText(QString::fromStdString(inst->getDepartment()));
    m_instEmailEdit->setText(QString::fromStdString(inst->getEmail()));
    m_instPhoneEdit->setText(QString::fromStdString(inst->getPhone()));
    m_instHoursSpin->setValue(inst->getMaxLimitHours());

    m_editingInstId = inst->getId();
    m_instIdEdit->setEnabled(false);
}

void MainWindow::onDeleteInstructor()
{
    QListWidgetItem *item = m_instList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Selection Error", "No instructor selected to delete.");
        return;
    }

    QString text = item->text();
    int idx = text.indexOf(" (Max Hours:");
    std::string id = (idx != -1) ? text.left(idx).toStdString() : text.toStdString();

    if (m_appManager.isInstructorUsed(id)) {
        QMessageBox::critical(this, "Cannot Delete",
                              "This item is currently used in one or more scheduled classes and cannot be deleted.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
                                                              "Are you sure you want to delete this instructor?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    if (m_appManager.removeInstructor(id)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete instructor.");
    }
}

void MainWindow::onAddCourse()
{
    std::string code = m_courseCodeEdit->text().trimmed().toStdString();
    std::string name = m_courseNameEdit->text().trimmed().toStdString();
    int hours = m_courseHoursSpin->value();
    int semester = m_courseSemesterSpin->value();
    std::string dept = m_courseDeptEdit->text().trimmed().toStdString();

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

    if (semester <= 0) {
        QMessageBox::warning(this, "Validation Error", "Semester must be a positive integer.");
        return;
    }

    Course crs(code, name, hours, semester, dept);

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
    m_courseSemesterSpin->setValue(1);
    m_courseDeptEdit->clear();

    m_courseList->clear();
    for (const auto& c : m_appManager.getCourses()) {
        m_courseList->addItem(QString("%1 (Allocated Hours: %2)").arg(QString::fromStdString(c.getCourseCode())).arg(c.getAllocatedHours()));
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
    m_courseSemesterSpin->setValue(crs->getSemester());
    m_courseDeptEdit->setText(QString::fromStdString(crs->getDepartment()));

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
    if (reply == QMessageBox::No) {
        return;
    }

    if (m_appManager.removeCourse(code)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete course.");
    }
}

void MainWindow::onAddRoom()
{
    std::string number = m_roomIdEdit->text().trimmed().toStdString();
    std::string building = m_roomBuildingEdit->text().trimmed().toStdString();
    int capacity = m_roomCapSpin->value();
    RoomType type = static_cast<RoomType>(m_roomTypeCombo->currentIndex());

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
    if (reply == QMessageBox::No) {
        return;
    }

    if (m_appManager.removeRoom(number)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete room.");
    }
}

void MainWindow::onAddBatch()
{
    std::string name = m_batchIdEdit->text().trimmed().toStdString();
    std::string dept = m_batchDeptEdit->text().trimmed().toStdString();
    int strength = m_batchStrengthSpin->value();
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
    if (reply == QMessageBox::No) {
        return;
    }

    if (m_appManager.removeBatch(name)) {
        delete item;
        saveToFile();
        populateCombos();
    } else {
        QMessageBox::warning(this, "Delete Failed", "Unable to delete student batch.");
    }
}

void MainWindow::onAddClassSession()
{
    QString instName = m_sessInstCombo->currentText();
    QString courseCode = m_sessCourseCombo->currentText();
    QString roomId = m_sessRoomCombo->currentText();
    QString batchId = m_sessBatchCombo->currentText();
    
    if (instName.isEmpty() || courseCode.isEmpty() || roomId.isEmpty() || batchId.isEmpty()) {
        QMessageBox::warning(this, "Selection Error", "Please ensure all entities (Instructors, Courses, Rooms, Batches) are created and selected.");
        return;
    }
    
    // Resolve instructor by ID first, then by name
    Instructor* inst = m_appManager.findInstructorById(instName.toStdString());
    if (!inst) inst = m_appManager.findInstructorByName(instName.toStdString());
    
    Course* crs = m_appManager.findCourseByCode(courseCode.toStdString());
    Room* rm = m_appManager.findRoomById(roomId.toStdString());
    StudentBatch* btch = m_appManager.findBatchById(batchId.toStdString());
    
    if (!inst || !crs || !rm || !btch) {
        QMessageBox::critical(this, "System Error", "Failed to retrieve references for the selected objects.");
        return;
    }
    
    QTime startTime = m_sessStartEdit->time();
    QTime endTime = m_sessEndEdit->time();
    
    if (startTime >= endTime) {
        QMessageBox::warning(this, "Time Range Error", "End time must be strictly after start time.");
        return;
    }
    
    Day day = static_cast<Day>(m_sessDayCombo->currentIndex());
    ClockTime ctStart{ startTime.hour(), startTime.minute() };
    ClockTime ctEnd{ endTime.hour(), endTime.minute() };
    TimeSlot slot(day, ctStart, ctEnd);
    
    // Assign course to instructor and track weekly hours workload limit
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
    
    // Create class session and add it
    ClassSession session(slot, inst, crs, rm, btch);
    m_appManager.addClassSession(session);
    
    saveToFile();
    refreshListsAndTables();
    
    QMessageBox::information(this, "Schedule Succeeded", "Class session scheduled successfully!");
}

