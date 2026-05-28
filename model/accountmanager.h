#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QString>
#include <QMap>

// 账号信息
struct Account {
    QString username;
    QString password;   // 明文存储（教学项目简化）
    QString role;       // "student" / "admin"
};

// 本地账号管理器
class AccountManager
{
public:
    static AccountManager* getInstance();

    // 验证账号，返回角色（空字符串表示失败）
    QString authenticate(const QString &username, const QString &password) const;

    // 检查账号是否存在
    bool accountExists(const QString &username) const;

    // 获取账号信息
    Account getAccount(const QString &username) const;

private:
    AccountManager();
    AccountManager(const AccountManager&)            = delete;
    AccountManager& operator=(const AccountManager&) = delete;

    void initAccounts();

    static AccountManager* m_instance;
    QMap<QString, Account> m_accounts;  // username → Account
};

#endif // ACCOUNTMANAGER_H
