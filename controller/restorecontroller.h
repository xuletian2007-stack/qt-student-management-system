#ifndef RESTORECONTROLLER_H
#define RESTORECONTROLLER_H

#include <QObject>
#include "../model/databasemanager.h"

// 恢复控制器（Controller层）
class RestoreController : public QObject
{
    Q_OBJECT
public:
    explicit RestoreController(QObject *parent = nullptr);

    void onRestoreRequested();

signals:
    void restoreSuccess();
    void restoreFailed(const QString &error);

private:
    DatabaseManager *m_db;
};

#endif // RESTORECONTROLLER_H
