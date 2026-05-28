#include "airesumeservice.h"
#include "../model/gpacalculator.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

AiResumeService::AiResumeService(QObject *parent)
    : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &AiResumeService::onReplyFinished);
}

QString AiResumeService::buildPrompt(const StudentProfile &profile) const
{
    double gpa = GpaCalculator::calculate(profile.courses());
    double avg = GpaCalculator::weightedAverage(profile.courses());

    QString prompt = "请根据以下信息生成一个美观、现代的 HTML 简历网页（只返回完整HTML代码，不要任何解释）：\n\n";

    prompt += QString("姓名：%1  学号：%2   GPA：%3  加权平均分：%4\n\n")
        .arg(profile.name(), profile.studentId())
        .arg(gpa, 0, 'f', 2)
        .arg(avg, 0, 'f', 1);

    prompt += "课程成绩：\n";
    for (const auto &c : profile.courses()) {
        prompt += QString("- %1（%2学分）：%3分 | 学期：%4\n")
            .arg(c.name())
            .arg(c.credit())
            .arg(c.score())
            .arg(c.semester());
    }

    if (!profile.achievements().isEmpty()) {
        prompt += "\n个人成就：\n";
        for (const auto &a : profile.achievements()) {
            prompt += QString("- [%1] %2（%3）\n")
                .arg(a.type(), a.title(), a.date().toString("yyyy-MM-dd"));
            if (!a.description().isEmpty())
                prompt += QString("  %1\n").arg(a.description());
        }
    }

    prompt += "\n要求：\n1. 使用现代化CSS设计（渐变、卡片、阴影、圆角等）\n"
              "2. 包含中文显示\n3. 使用响应式布局\n"
              "4. 配色方案专业大方\n5. 只返回完整的HTML代码，不要markdown标记\n";

    return prompt;
}

void AiResumeService::requestAiResume(const StudentProfile &profile)
{
    QString prompt = buildPrompt(profile);

    QUrl url(m_apiUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QJsonObject body;
    body["model"] = "deepseek-chat";
    QJsonArray messages;
    QJsonObject msg;
    msg["role"] = "user";
    msg["content"] = prompt;
    messages.append(msg);
    body["messages"] = messages;
    body["max_tokens"] = 4096;
    body["temperature"] = 0.7;

    m_manager->post(request, QJsonDocument(body).toJson());
}

void AiResumeService::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit resumeFinished("");  // 空表示失败
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    QJsonArray choices = doc.object()["choices"].toArray();
    if (choices.isEmpty()) {
        emit resumeFinished("");
        return;
    }

    QString htmlContent = choices[0].toObject()["message"].toObject()["content"].toString();
    QString path = saveHtmlFile(htmlContent);
    emit resumeFinished(path);
}

QString AiResumeService::saveHtmlFile(const QString &htmlContent) const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString path = dir + "/ai_resume_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".html";

    QString clean = htmlContent;
    if (clean.startsWith("```html")) {
        clean = clean.mid(7);
        if (clean.endsWith("```")) clean.chop(3);
    } else if (clean.startsWith("```")) {
        clean = clean.mid(3);
        if (clean.endsWith("```")) clean.chop(3);
    }

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(clean.toUtf8());
        file.close();
    }
    return path;
}
