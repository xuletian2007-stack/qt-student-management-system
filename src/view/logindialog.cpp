#include "logindialog.h"
#include "../model/accountmanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QFont>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("PersonalDevPlan - 用户登录");
    setFixedSize(360, 260);
    setupUI();
}

void LoginDialog::setupUI()
{
    auto *mainLay = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("📋 学生发展计划系统");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(title);

    auto *subtitle = new QLabel("请输入账号和密码登录");
    subtitle->setStyleSheet("color: #7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(subtitle);
    mainLay->addSpacing(12);

    // 输入区
    auto *form = new QFormLayout();
    m_userEdit = new QLineEdit(this);
    m_userEdit->setPlaceholderText("请输入用户名");
    form->addRow("用户名：", m_userEdit);

    m_passEdit = new QLineEdit(this);
    m_passEdit->setPlaceholderText("请输入密码");
    m_passEdit->setEchoMode(QLineEdit::Password);
    form->addRow("密　码：", m_passEdit);
    mainLay->addLayout(form);

    // 错误提示
    m_errorLabel = new QLabel("");
    m_errorLabel->setStyleSheet("color: #e74c3c; font-size: 12px;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(m_errorLabel);

    mainLay->addSpacing(4);

    // 按钮
    auto *btnLay = new QHBoxLayout();
    btnLay->addStretch();
    auto *btnLogin = new QPushButton("登录", this);
    btnLogin->setDefault(true);
    btnLogin->setMinimumWidth(120);
    btnLogin->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; font-weight: bold; "
        "padding: 8px 20px; border-radius: 4px; }");
    auto *btnCancel = new QPushButton("退出", this);
    btnCancel->setMinimumWidth(80);
    btnLay->addWidget(btnCancel);
    btnLay->addWidget(btnLogin);
    mainLay->addLayout(btnLay);

    connect(btnLogin,  &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    // 回车登录
    connect(m_passEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
}

void LoginDialog::onLogin()
{
    QString user = m_userEdit->text().trimmed();
    QString pass = m_passEdit->text().trimmed();

    if (user.isEmpty()) {
        m_errorLabel->setText("请输入用户名");
        return;
    }

    QString result = AccountManager::getInstance()->authenticate(user, pass);

    if (result.isEmpty()) {
        if (AccountManager::getInstance()->accountExists(user))
            m_errorLabel->setText("密码错误，请重试");
        else
            m_errorLabel->setText("账号不存在");
        m_passEdit->clear();
        m_passEdit->setFocus();
        return;
    }

    m_username = user;
    m_role = result;
    accept();
}
