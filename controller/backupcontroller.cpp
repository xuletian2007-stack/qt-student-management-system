#include "backupcontroller.h"
#include <QFileDialog>
#include <QDateTime>

BackupController::BackupController(QObject *parent)
    : QObject(parent)
{
    m_db = DatabaseManager::getInstance();
}

void BackupController::onBackupRequested()
{
    QString path = QFileDialog::getSaveFileName(
        nullptr, "数据备份",
        "backup_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".pdp",
        "PDP备份文件 (*.pdp)");

    if (path.isEmpty()) return;

    QString error;
    if (m_db->exportBackup(path, error))
        emit backupSuccess(path);
    else
        emit backupFailed(error);
}
