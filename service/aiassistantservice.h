#ifndef AIASSISTANTSERVICE_H
#define AIASSISTANTSERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../model/studentprofile.h"

// AI 助手服务（Service层）
class AiAssistantService : public QObject
{
    Q_OBJECT
public:
    enum AiTask {
        Evaluation,     // 学业评价与发展建议
        PolishResume,   // 润色简历文本
        GenerateHtml    // 生成 HTML 网页简历
    };

    explicit AiAssistantService(QObject *parent = nullptr);

    void setApiKey(const QString &key) { m_apiKey = key; }
    QString apiKey() const { return m_apiKey; }
    void setApiUrl(const QString &url) { m_apiUrl = url; }

    void request(AiTask task, const StudentProfile &profile);

signals:
    void finished(const QString &result);  // 返回结果文本/HTML

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString buildPrompt(AiTask task, const StudentProfile &profile) const;
    QString saveResult(const QString &content, const QString &prefix) const;

    QNetworkAccessManager *m_manager;
    QString m_apiKey;
    QString m_apiUrl;
};

#endif // AIASSISTANTSERVICE_H
