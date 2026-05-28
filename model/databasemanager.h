#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "studentprofile.h"
#include <QString>

class DatabaseManager
{
public:
    static DatabaseManager* getInstance();

    // 按用户名存储
    bool saveData(const QString &username, const StudentProfile &profile);
    StudentProfile loadData(const QString &username);

    bool exportBackup(const QString &filePath, QString &errorMsg);
    bool importBackup(const QString &filePath, QString &errorMsg);
    QString dbPath() const { return m_dbPath; }

    void initDefaultData();

private:
    DatabaseManager();
    DatabaseManager(const DatabaseManager&)            = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    static DatabaseManager* m_instance;
    QString m_dbPath;
};

#endif // DATABASEMANAGER_H
