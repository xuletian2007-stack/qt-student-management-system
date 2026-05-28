#ifndef AIRESUMESERVICE_H
#define AIRESUMESERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../model/studentprofile.h"

// AI 简历服务（Service层）
// 依赖于 StudentProfile，包含 <<signals/slots>>
class AiResumeService : public QObject
{
    Q_OBJECT
public:
    explicit AiResumeService(QObject *parent = nullptr);

    void setApiKey(const QString &key)  { m_apiKey = key; }
    void setApiUrl(const QString &url)  { m_apiUrl = url; }

    void requestAiResume(const StudentProfile &profile);

signals:
    void resumeFinished(const QString &html);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString buildPrompt(const StudentProfile &profile) const;
    QString saveHtmlFile(const QString &htmlContent) const;

    QNetworkAccessManager *m_manager;
    QString m_apiKey;
    QString m_apiUrl;
};

#endif // AIRESUMESERVICE_H
