#ifndef RESUMEVIEW_H
#define RESUMEVIEW_H

#include <QWidget>
#include <QTextBrowser>
#include <QPushButton>

// 标准简历视图（View层）
class ResumeView : public QWidget
{
    Q_OBJECT
public:
    explicit ResumeView(QWidget *parent = nullptr);

    void setHtmlPreview(const QString &html);

signals:
    void exportRequested();

private:
    void setupUI();
    void onExport();

    QTextBrowser *m_preview;
    QPushButton  *m_btnExport;
};

#endif // RESUMEVIEW_H
