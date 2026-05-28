#include "restorecontroller.h"
#include <QFileDialog>

RestoreController::RestoreController(QObject *parent)
    : QObject(parent)
{
    m_db = DatabaseManager::getInstance();
}

void RestoreController::onRestoreRequested()
{
    QString path = QFileDialog::getOpenFileName(
        nullptr, "恢复数据", "",
        "PDP备份文件 (*.pdp);;JSON文件 (*.json);;所有文件 (*)");

    if (path.isEmpty()) return;

    QString error;
    if (m_db->importBackup(path, error))
        emit restoreSuccess();
    else
        emit restoreFailed(error);
}
