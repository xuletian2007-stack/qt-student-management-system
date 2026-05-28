#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTabBar>

#include "../model/studentprofile.h"
#include "../model/databasemanager.h"
#include "../model/resumegenerator.h"
#include "../service/aiassistantservice.h"

#include "../controller/importcontroller.h"
#include "../controller/gpacontroller.h"
#include "../controller/achievementcontroller.h"
#include "../controller/resumecontroller.h"
#include "../controller/aiassistantcontroller.h"

#include "importview.h"
#include "gpaview.h"
#include "profileview.h"
#include "resumeview.h"
#include "aiassistantview.h"
#include "adminpanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &username, const QString &role,
                        QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchPage(int index);
    void onRefreshAllViews();

private:
    void setupUI();
    void setupStudentUI();
    void setupAdminUI();
    void setupConnections();
    void createMenuBar();

    QString m_username;
    QString m_role;
    QString m_origPhotoBase64;
    QString m_origPhotoPath;

    StudentProfile        *m_profile    = nullptr;
    DatabaseManager       *m_db         = nullptr;
    ResumeGenerator       *m_generator  = nullptr;
    AiAssistantService    *m_aiService  = nullptr;

    ImportController       *m_importCtrl    = nullptr;
    GpaController          *m_gpaCtrl       = nullptr;
    AchievementController  *m_achievementCtrl = nullptr;
    ResumeController       *m_resumeCtrl    = nullptr;
    AiAssistantController  *m_aiCtrl        = nullptr;

    QStackedWidget   *m_stack = nullptr;
    QTabBar          *m_navBar = nullptr;

    // 学生视图
    ImportView       *m_importView  = nullptr;
    GPAView          *m_gpaView     = nullptr;
    ProfileView      *m_profileView = nullptr;
    ResumeView       *m_resumeView  = nullptr;
    AiAssistantView  *m_aiView      = nullptr;

    // 管理员视图
    AdminPanel       *m_adminPanel  = nullptr;

    static constexpr const char *API_KEY_SETTING = "ai/apiKey";
};

#endif // MAINWINDOW_H
