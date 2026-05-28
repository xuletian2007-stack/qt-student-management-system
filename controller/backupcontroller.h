#ifndef BACKUPCONTROLLER_H
#define BACKUPCONTROLLER_H

#include <QObject>
#include "../model/databasemanager.h"

// 备份控制器（Controller层）
class BackupController : public QObject
{
    Q_OBJECT
public:
    explicit BackupController(QObject *parent = nullptr);

    void onBackupRequested();

signals:
    void backupSuccess(const QString &path);
    void backupFailed(const QString &error);

private:
    DatabaseManager *m_db;
};

#endif // BACKUPCONTROLLER_H
