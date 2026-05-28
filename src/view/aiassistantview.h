#ifndef AIASSISTANTVIEW_H
#define AIASSISTANTVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextBrowser>

// AI 助手视图（View层）
class AiAssistantView : public QWidget
{
    Q_OBJECT
public:
    explicit AiAssistantView(QWidget *parent = nullptr);

    void setApiKey(const QString &key);
    QString apiKey() const { return m_edApiKey->text().trimmed(); }
    void showResult(const QString &text);
    void setLoading(bool loading);

signals:
    void evaluationRequested();
    void polishRequested();
    void htmlResumeRequested();

private:
    void setupUI();

    QLineEdit    *m_edApiKey;
    QPushButton  *m_btnEval, *m_btnPolish, *m_btnHtml;
    QLabel       *m_lblLoading;
    QTextBrowser *m_browser;
};

#endif // AIASSISTANTVIEW_H
