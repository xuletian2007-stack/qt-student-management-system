#include "mainwindow.h"
#include "../model/gpacalculator.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(const QString &username, const QString &role, QWidget *parent)
    : QMainWindow(parent), m_username(username), m_role(role)
{
    bool isStudent = (role == "student");
    setWindowTitle("PersonalDevPlan - " + (isStudent
        ? QString("学生：%1").arg(username) : QString("管理员：%1").arg(username)));
    resize(960, 680);

    m_db      = DatabaseManager::getInstance();
    m_profile = new StudentProfile(m_db->loadData(m_username));
    m_origPhotoBase64 = m_profile->photoBase64();  // 记住原始照片数据
    m_origPhotoPath   = m_profile->photoPath();
    if (isStudent) {
        if (m_profile->name().isEmpty()) m_profile->setName(username);
        if (m_profile->studentId().isEmpty()) m_profile->setStudentId(username);
    }

    m_generator = new ResumeGenerator();
    m_aiService = new AiAssistantService(this);
    QSettings settings;
    m_aiService->setApiKey(settings.value(API_KEY_SETTING).toString());

    m_importCtrl = new ImportController(this);
    m_gpaCtrl    = new GpaController(this);
    m_gpaCtrl->setModel(m_profile);
    m_achievementCtrl = new AchievementController(this);
    m_achievementCtrl->setModel(m_profile);
    m_resumeCtrl = new ResumeController(this);
    m_resumeCtrl->setModel(m_profile);
    m_resumeCtrl->setGenerator(m_generator);
    m_aiCtrl = new AiAssistantController(this);
    m_aiCtrl->setModel(m_profile);
    m_aiCtrl->setService(m_aiService);

    createMenuBar();
    setupUI();
    if (isStudent) setupStudentUI();
    else           setupAdminUI();
    setupConnections();
    // setupStudentUI 之后 m_gpaView 才存在，此时刷新数据
    onRefreshAllViews();
}

MainWindow::~MainWindow() {
    // 保护照片不被空值覆盖
    if (m_profile->photoBase64().isEmpty() && !m_origPhotoBase64.isEmpty())
        m_profile->setPhotoBase64(m_origPhotoBase64);
    if (m_profile->photoPath().isEmpty() && !m_origPhotoPath.isEmpty())
        m_profile->setPhotoPath(m_origPhotoPath);
    m_db->saveData(m_username, *m_profile);
    delete m_profile;
}

void MainWindow::createMenuBar()
{
    menuBar()->setStyleSheet(
        "QMenuBar { font-size: 14px; padding: 2px 0px; }"
        "QMenuBar::item { padding: 4px 12px; }"
        "QMenu { font-size: 14px; }"
        "QMenu::item { padding: 6px 28px; }");
    auto *actLogout = menuBar()->addAction(QStringLiteral("退出登录"));
    connect(actLogout, &QAction::triggered, this, [this]() {
        if (QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_db->saveData(m_username, *m_profile);
            close();
        }
    });
}

void MainWindow::setupUI()
{
    auto *central = new QWidget(this);
    auto *mainLay = new QVBoxLayout(central);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(0);
    m_navBar = new QTabBar(central);
    m_navBar->setExpanding(false);
    m_navBar->setStyleSheet(
        "QTabBar { font-size: 14px; }"
        "QTabBar::tab { min-width: 120px; padding: 12px 20px; }");
    mainLay->addWidget(m_navBar);
    m_stack = new QStackedWidget(central);
    mainLay->addWidget(m_stack);
    setCentralWidget(central);
    connect(m_navBar, &QTabBar::currentChanged, this, &MainWindow::switchPage);
}

void MainWindow::setupStudentUI()
{
    setWindowTitle("PersonalDevPlan - 学生：" + m_profile->name());
    m_navBar->addTab("📊 学业看板");
    m_navBar->addTab("📝 个人信息");
    m_navBar->addTab("📄 标准简历");
    m_navBar->addTab("🤖 AI 助手");
    m_navBar->addTab("🚪 退出登录");

    m_gpaView     = new GPAView(this);
    m_gpaCtrl->setView(m_gpaView);
    m_profileView = new ProfileView(this);
    m_resumeView  = new ResumeView(this);
    m_aiView      = new AiAssistantView(this);

    m_stack->addWidget(m_gpaView);
    m_stack->addWidget(m_profileView);
    m_stack->addWidget(m_resumeView);
    m_stack->addWidget(m_aiView);

    m_aiView->setApiKey(m_aiService->apiKey());
    m_aiCtrl->setView(m_aiView);
}

void MainWindow::setupAdminUI()
{
    setWindowTitle("PersonalDevPlan - 管理员：小灰灰");
    m_navBar->addTab("📊 管理中心");
    m_navBar->addTab("🚪 退出登录");

    m_adminPanel = new AdminPanel(this);
    m_stack->addWidget(m_adminPanel);
}

void MainWindow::setupConnections()
{
    if (m_role == "admin") {
        connect(m_adminPanel, &AdminPanel::dataChanged, this, [this]() {
            // 管理员修改数据后不需要刷新学生视图，管理员没有这些 view
        });
        return;
    }

    // 个人信息保存
    connect(m_profileView, &ProfileView::profileChanged, this, [this]() {
        m_profileView->saveToProfile(*m_profile);
        m_db->saveData(m_username, *m_profile);
        onRefreshAllViews();
        QMessageBox::information(this, "成功", "个人信息已保存！");
    });

    connect(m_resumeView, &ResumeView::exportRequested,
            m_resumeCtrl, &ResumeController::onExportResumeRequested);
    connect(m_resumeCtrl, &ResumeController::standardResumeGenerated,
            m_resumeView, &ResumeView::setHtmlPreview);

    connect(m_aiView, &AiAssistantView::evaluationRequested,
            m_aiCtrl, &AiAssistantController::onEvalRequested);
    connect(m_aiView, &AiAssistantView::polishRequested,
            m_aiCtrl, &AiAssistantController::onPolishRequested);
    connect(m_aiView, &AiAssistantView::htmlResumeRequested,
            m_aiCtrl, &AiAssistantController::onHtmlResumeRequested);

    auto saveKey = [this]() { QSettings().setValue(API_KEY_SETTING, m_aiView->apiKey()); };
    connect(m_aiView, &AiAssistantView::evaluationRequested, this, saveKey);
    connect(m_aiView, &AiAssistantView::polishRequested, this, saveKey);
    connect(m_aiView, &AiAssistantView::htmlResumeRequested, this, saveKey);
}

void MainWindow::switchPage(int index)
{
    if (m_role == "student" && index == 4) {
        if (QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_db->saveData(m_username, *m_profile);
            close();
        }
        return;
    }
    if (m_role == "admin" && index == 1) {
        if (QMessageBox::question(this, "确认退出", "确定要退出登录吗？",
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            close();
        return;
    }

    m_stack->setCurrentIndex(index);
    if (m_role == "student") {
        if (index == 0) m_gpaCtrl->onViewGPARequested();
        if (index == 1 && m_profileView) m_profileView->loadFromProfile(*m_profile);
    }
}

void MainWindow::onRefreshAllViews()
{
    if (m_role != "student") return;
    m_gpaCtrl->onViewGPARequested();
    if (m_profileView) m_profileView->loadFromProfile(*m_profile);
    if (m_resumeView) {
        QString html = m_generator->fillTemplate(*m_profile);
        m_resumeView->setHtmlPreview(html);
    }
}
