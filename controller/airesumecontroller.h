#ifndef AIRESUMECONTROLLER_H
#define AIRESUMECONTROLLER_H

#include <QObject>
#include "../model/studentprofile.h"
#include "../service/airesumeservice.h"

// AI 简历控制器（Controller层）
class AiResumeController : public QObject
{
    Q_OBJECT
public:
    explicit AiResumeController(QObject *parent = nullptr);

    void setModel(StudentProfile *model) { m_model = model; }
    void setService(AiResumeService *svc) { m_service = svc; }

public slots:
    void onGenerateAiResumeRequested();

signals:
    void aiResumeGenerated(const QString &htmlPath);
    void aiResumeFailed(const QString &error);

private:
    void callAiApi();

    StudentProfile  *m_model   = nullptr;
    AiResumeService *m_service = nullptr;
};

#endif // AIRESUMECONTROLLER_H
