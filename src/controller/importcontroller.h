#ifndef IMPORTCONTROLLER_H
#define IMPORTCONTROLLER_H

#include <QObject>
#include "../model/databasemanager.h"

// 导入控制器（Controller层）
class ImportController : public QObject
{
    Q_OBJECT
public:
    explicit ImportController(QObject *parent = nullptr);

    void onImportRequested(const QString &path);
    bool validateFile(const QString &path);

signals:
    void importSuccess();
    void importFailed(const QString &error);

private:
    DatabaseManager *m_db;
};

#endif // IMPORTCONTROLLER_H
