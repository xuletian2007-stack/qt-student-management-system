#ifndef AIASSISTANTCONTROLLER_H
#define AIASSISTANTCONTROLLER_H

#include <QObject>
#include "../model/studentprofile.h"
#include "../service/aiassistantservice.h"

class AiAssistantView;

class AiAssistantController : public QObject
{
    Q_OBJECT
public:
    explicit AiAssistantController(QObject *parent = nullptr);

    void setModel(StudentProfile *model)     { m_model = model; }
    void setService(AiAssistantService *svc) { m_service = svc; }
    void setView(AiAssistantView *view)      { m_view = view; }

public slots:
    void onEvalRequested();
    void onPolishRequested();
    void onHtmlResumeRequested();

signals:
    void resultReady(const QString &text);
    void requestFailed(const QString &error);

private:
    void beforeRequest();

    StudentProfile      *m_model   = nullptr;
    AiAssistantService  *m_service = nullptr;
    AiAssistantView     *m_view    = nullptr;
};

#endif // AIASSISTANTCONTROLLER_H
