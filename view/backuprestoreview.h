#ifndef BACKUPRESTOREVIEW_H
#define BACKUPRESTOREVIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

// 备份/恢复视图（View层）
class BackupRestoreView : public QWidget
{
    Q_OBJECT
public:
    explicit BackupRestoreView(QWidget *parent = nullptr);

    void setMode(bool isBackup);  // true=备份, false=恢复
    QString getPath() const;

signals:
    void actionRequested();  // 通用请求信号，由 Controller 根据 mode 处理

private:
    void setupUI();

    QLineEdit   *m_editPath;
    QPushButton *m_btnAction;
    bool         m_isBackup = true;
};

#endif // BACKUPRESTOREVIEW_H
