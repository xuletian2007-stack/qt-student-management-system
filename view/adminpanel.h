#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include "../model/studentprofile.h"
#include "../model/databasemanager.h"

class AdminPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AdminPanel(QWidget *parent = nullptr);

    void loadAllStudents();

signals:
    void dataChanged();

private:
    void setupUI();
    void onStudentSelected(int index);
    void loadStudentData(const QString &username);
    void saveCurrentCourses();
    void onAddCourse();
    void onEditCourse();
    void onDeleteCourse();

    QComboBox    *m_studentCombo;
    QLabel       *m_lblGPA;
    QTabWidget   *m_tabWidget;

    // 成绩表格
    QTableWidget *m_courseTable;

    // 导入页
    QWidget      *m_importPage;
    QPushButton  *m_btnSelectFile;
    QLabel       *m_statusLabel;

    QString       m_currentStudent;
    StudentProfile m_currentProfile;
    DatabaseManager *m_db;
    QJsonObject    m_importedData;
};

#endif // ADMINPANEL_H
