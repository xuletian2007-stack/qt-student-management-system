#ifndef AIRESUMEVIEW_H
#define AIRESUMEVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

// AI 简历视图（View层）
// 注意：类图要求 QWebEngineView，但为避免依赖 QtWebEngine 而使用 QTextBrowser
// 如需 WebEngine，将 QTextBrowser 改为 QWebEngineView 即可
class QTextBrowser;

class AiResumeView : public QWidget
{
    Q_OBJECT
public:
    explicit AiResumeView(QWidget *parent = nullptr);

    void loadAiHtml(const QString &html);
    void toggleLoading(bool loading);

signals:
    void generateRequested();

private:
    void setupUI();

    QTextBrowser *m_browser;
    QPushButton  *m_btnGenerate;
    QLabel       *m_lblLoading;
};

#endif // AIRESUMEVIEW_H
