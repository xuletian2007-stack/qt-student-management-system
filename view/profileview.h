#ifndef PROFILEVIEW_H
#define PROFILEVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include "../model/studentprofile.h"

class ProfileView : public QWidget
{
    Q_OBJECT
public:
    explicit ProfileView(QWidget *parent = nullptr);
    void loadFromProfile(const StudentProfile &profile);
    void saveToProfile(StudentProfile &profile) const;

signals:
    void profileChanged();

private:
    void setupUI();
    QWidget* createBasicInfoTab();
    QWidget* createEducationTab();
    QWidget* createSkillsTab();
    QWidget* createExperienceTab();
    QWidget* createAchievementTab();

    QTabWidget *m_tabWidget;

    // 基本信息
    QLineEdit *m_edName, *m_edStudentId, *m_cmbPolitical;
    QComboBox *m_cmbGender;
    QLineEdit *m_edBirthday, *m_edEmail, *m_edPhone;
    QTextEdit *m_edPersonality, *m_edHobby;
    QLabel    *m_lblPhoto;
    QString    m_photoPath;
    QString    m_photoBase64;

    // 教育背景表格
    QTableWidget *m_eduTable;

    // 专业技能表格
    QTableWidget *m_skillTable;

    // 项目经历表格
    QTableWidget *m_expTable;

    // 荣誉奖项表格
    QTableWidget *m_achTable;
};

#endif // PROFILEVIEW_H
