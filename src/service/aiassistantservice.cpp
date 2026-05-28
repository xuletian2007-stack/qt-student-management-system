#include "aiassistantservice.h"
#include "../model/gpacalculator.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QUrl>

AiAssistantService::AiAssistantService(QObject *parent)
    : QObject(parent), m_apiUrl("https://api.deepseek.com/chat/completions")
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &AiAssistantService::onReplyFinished);
}

QString AiAssistantService::buildPrompt(AiTask task, const StudentProfile &profile) const
{
    double gpa    = GpaCalculator::calculate(profile.courses());
    double avg    = GpaCalculator::weightedAverage(profile.courses());
    double cred   = GpaCalculator::totalCredits(profile.courses());
    auto edus     = profile.educations();
    QString level = GpaCalculator::gpaDescription(avg);

    QString info;
    info += QString("姓名：%1  学号：%2\n").arg(profile.name(), profile.studentId());
    info += QString("性别：%1  政治面貌：%2  生日：%3\n")
                .arg(profile.gender(), profile.political(), profile.birthday());
    if (!profile.personality().isEmpty())
        info += QString("性格：%1\n").arg(profile.personality());
    if (!profile.hobby().isEmpty())
        info += QString("爱好：%1\n").arg(profile.hobby());
    info += "教育背景：\n";
    for (const auto &edu : edus)
        info += QString("- %1 · %2 · %3  %4 — %5\n")
                    .arg(edu.schoolName, edu.major, edu.degree, edu.startYear, edu.endYear);
    info += QString("GPA：%1  加权平均分：%2  总学分：%3  等级：%4\n")
                .arg(gpa, 0, 'f', 2).arg(avg, 0, 'f', 1)
                .arg(cred, 0, 'f', 1).arg(level);

    if (!profile.courses().isEmpty()) {
        info += "课程成绩：\n";
        for (const auto &c : profile.courses())
            info += QString("- %1（%2学分）：%3分  绩点：%4  学期：%5\n")
                        .arg(c.name()).arg(c.credit()).arg(c.score())
                        .arg(c.getGradePoint(), 0, 'f', 2).arg(c.semester());
    }

    if (!profile.skills().isEmpty()) {
        info += "专业技能：";
        for (const auto &s : profile.skills())
            info += s.name + "（" + s.level + "） ";
        info += "\n";
    }

    if (!profile.experiences().isEmpty()) {
        info += "项目与实习经历：\n";
        for (const auto &e : profile.experiences())
            info += QString("- %1（%2）%3 — %4：%5\n")
                        .arg(e.title, e.role, e.startDate, e.endDate, e.description);
    }

    if (!profile.achievements().isEmpty()) {
        info += "荣誉与奖项：\n";
        for (const auto &a : profile.achievements())
            info += QString("- %1 [%2] %3\n")
                        .arg(a.title(), a.type(), a.date().toString("yyyy-MM-dd"));
    }

    QString prompt;
    switch (task) {
    case Evaluation:
        prompt = "你是一位资深的学业规划与职业发展导师。\n"
                 "请根据以下学生信息，进行深度学业评价，必须有：\n"
                 "1. 📊 学业现状分析（2-3句话，结合GPA和课程分布）\n"
                 "2. 💡 优势领域（列出2-3个亮点）\n"
                 "3. ⚠️ 待改进方向（具体指出薄弱课程或技能缺口）\n"
                 "4. 🎯 发展建议（2-3条具体可执行的建议，每条50字以上）\n"
                 "5. 📈 未来展望（基于当前表现预测发展路径）\n\n"
                 "格式：用清晰的小标题分段，每条建议有具体理由。" + info;
        break;
    case PolishResume:
        prompt = "你是一位专业的简历顾问。请根据以下学生信息，生成一份饱满的简历文案。\n"
                 "必须包含以下板块，每板块2-4句话，语言精炼有感染力：\n"
                 "1. 个人概述（姓名、性格特点、核心优势概括）\n"
                 "2. 教育背景（学校专业+学业亮点，引用GPA数据）\n"
                 "3. 专业技能（列出所有技能并简要说明掌握程度）\n"
                 "4. 项目与实践经历（每段经历展开描述，突出成果和收获）\n"
                 "5. 荣誉与奖项（列出并简要说明含金量）\n"
                 "6. 自我评价（结合性格和爱好，写出个人亮点）\n"
                 "字数：400-600字，语言正式但不死板。\n\n" + info;
        break;
    case GenerateHtml:
        prompt = "请根据以下信息生成一个美观、现代的 HTML 简历网页"
                 "（只返回完整HTML代码，不要任何解释）。\n"
                 "要求：现代化CSS设计（渐变、卡片、阴影、圆角）、中文显示、"
                 "响应式布局、专业配色、包含以下板块："
                 "个人基本信息、教育背景与学业成绩、专业技能、项目与实习经历、荣誉与奖项。\n\n" + info;
        break;
    }
    return prompt;
}

void AiAssistantService::request(AiTask task, const StudentProfile &profile)
{
    QString prompt = buildPrompt(task, profile);

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

    // 把 task 存到 reply property 里
    QNetworkReply *reply = m_manager->post(request, QJsonDocument(body).toJson());
    reply->setProperty("aiTask", (int)task);
}

void AiAssistantService::onReplyFinished(QNetworkReply *reply)
{
    AiTask task = (AiTask)reply->property("aiTask").toInt();

    if (reply->error() != QNetworkReply::NoError) {
        emit finished("❌ API 请求失败：" + reply->errorString()
                      + "\n\n请检查 API Key 是否正确，网络是否正常。");
        reply->deleteLater();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();

    QJsonArray choices = doc.object()["choices"].toArray();
    if (choices.isEmpty()) {
        emit finished("❌ AI 返回数据为空");
        return;
    }

    QString content = choices[0].toObject()["message"].toObject()["content"].toString();

    if (task == GenerateHtml) {
        QString path = saveResult(content, "ai_resume");
        emit finished("✅ HTML 简历已保存到桌面：\n" + path);
    } else {
        emit finished(content);
    }
}

QString AiAssistantService::saveResult(const QString &content, const QString &prefix) const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString path = dir + "/" + prefix + "_"
                 + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".html";

    QString clean = content;
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
