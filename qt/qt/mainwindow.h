#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QLabel>
#include <QTimeEdit>
#include <QMessageBox>
#include <QVector>
#include <QScrollArea>

#include "AppManager.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddInstructor();
    void onEditInstructor();
    void onDeleteInstructor();
    void onSubjectCountChanged(int count);  // rebuilds the dynamic subject dropdowns
    void onAddCourse();
    void onEditCourse();
    void onDeleteCourse();
    void onAddRoom();
    void onEditRoom();
    void onDeleteRoom();
    void onAddBatch();
    void onEditBatch();
    void onDeleteBatch();
    void onAddClassSession();
    void onDeleteClassSession();
    void onAutoGenerate();

private:
    Ui::MainWindow *ui;
    AppManager m_appManager;

    // Helper UI setup
    void setupUI();
    void populateCombos();
    void refreshListsAndTables();
    void refreshInstList();          // rebuilds the instructor list widget
    void populateInitialData();
    void saveToFile();
    void loadFromFile();

    // Rebuilds the N subject-selection combos in the add-instructor form
    void rebuildSubjectCombos(int count);

    // Tab widgets
    QTabWidget *m_tabWidget;

    // Instructor UI
    QLineEdit   *m_instIdEdit;
    QLineEdit   *m_instNameEdit;
    QSpinBox    *m_instHoursSpin;
    QSpinBox    *m_instSubjectCountSpin;   // how many subjects (1–3)
    QWidget     *m_instSubjectContainer;   // holds the dynamic combo boxes
    QVBoxLayout *m_instSubjectLayout;      // layout inside that container
    QVector<QComboBox*> m_instSubjectCombos; // the dynamic subject combos
    QListWidget *m_instList;

    // Course UI  (semester / department removed)
    QLineEdit *m_courseCodeEdit;
    QLineEdit *m_courseNameEdit;
    QSpinBox  *m_courseHoursSpin;
    QListWidget *m_courseList;

    // Room UI
    QLineEdit *m_roomIdEdit;
    QLineEdit *m_roomBuildingEdit;
    QSpinBox  *m_roomCapSpin;
    QComboBox *m_roomTypeCombo;
    QListWidget *m_roomList;

    // Batch UI
    QLineEdit *m_batchIdEdit;
    QSpinBox  *m_batchStrengthSpin;
    QComboBox *m_batchProgCombo;
    QLineEdit *m_batchDeptEdit;
    QListWidget *m_batchList;

    // Session UI
    QComboBox *m_sessInstCombo;
    QComboBox *m_sessCourseCombo;
    QComboBox *m_sessRoomCombo;
    QComboBox *m_sessBatchCombo;
    QComboBox *m_sessDayCombo;
    QTimeEdit *m_sessStartEdit;
    QTimeEdit *m_sessEndEdit;
    QTableWidget *m_timetableTable;

    // Editing states
    std::string m_editingInstId;
    std::string m_editingCourseCode;
    std::string m_editingRoomId;
    std::string m_editingBatchId;
};


#endif // MAINWINDOW_H
