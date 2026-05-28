#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

// 账号登录对话框
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);

    QString username() const { return m_username; }
    QString role()     const { return m_role; }       // "student" / "admin"

private slots:
    void onLogin();

private:
    void setupUI();

    QLineEdit  *m_userEdit;
    QLineEdit  *m_passEdit;
    QLabel     *m_errorLabel;
    QString     m_username;
    QString     m_role;
};

#endif // LOGINDIALOG_H
