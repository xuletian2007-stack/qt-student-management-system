#include "accountmanager.h"

AccountManager* AccountManager::m_instance = nullptr;

AccountManager::AccountManager()
{
    initAccounts();
}

AccountManager* AccountManager::getInstance()
{
    if (!m_instance)
        m_instance = new AccountManager();
    return m_instance;
}

void AccountManager::initAccounts()
{
    // ====== 学生账号（9个） ======
    m_accounts["喜羊羊"] = {"喜羊羊", "35060525", "student"};
    m_accounts["美羊羊"] = {"美羊羊", "35070420", "student"};
    m_accounts["懒羊羊"] = {"懒羊羊", "35070626", "student"};
    m_accounts["沸羊羊"] = {"沸羊羊", "35050318", "student"};
    m_accounts["暖羊羊"] = {"暖羊羊", "35050817", "student"};
    m_accounts["慢羊羊"] = {"慢羊羊", "34540125", "student"};
    m_accounts["灰太狼"] = {"灰太狼", "34751212", "student"};
    m_accounts["红太狼"] = {"红太狼", "34760801", "student"};

    // ====== 管理员账号（1个） ======
    m_accounts["小灰灰"] = {"小灰灰", "35131001", "admin"};
}

QString AccountManager::authenticate(const QString &username, const QString &password) const
{
    if (!m_accounts.contains(username))
        return QString();  // 用户不存在

    const Account &acc = m_accounts[username];
    if (acc.password == password)
        return acc.role;

    return QString();  // 密码错误
}

bool AccountManager::accountExists(const QString &username) const
{
    return m_accounts.contains(username);
}

Account AccountManager::getAccount(const QString &username) const
{
    if (m_accounts.contains(username))
        return m_accounts[username];
    return Account();
}
