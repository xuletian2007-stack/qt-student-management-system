#ifndef IMPORTVIEW_H
#define IMPORTVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>

// 导入视图（View层）
class ImportView : public QWidget
{
    Q_OBJECT
public:
    explicit ImportView(QWidget *parent = nullptr);

    void showStatus(const QString &msg);
    QString getSelectedPath() const;

signals:
    void importRequested(const QString &path);

private slots:
    void onSelectFile();

private:
    void setupUI();

    QPushButton  *m_btnSelect;
    QProgressBar *m_bar;
    QLabel       *m_lblStatus;
    QString       m_selectedPath;
};

#endif // IMPORTVIEW_H
