#include "aiassistantcontroller.h"
#include "../view/aiassistantview.h"
#include <QMessageBox>

AiAssistantController::AiAssistantController(QObject *parent)
    : QObject(parent)
{
}

void AiAssistantController::beforeRequest()
{
    if (!m_model || !m_service || !m_view) return;

    QString key = m_view->apiKey();
    if (key.trimmed().isEmpty()) {
        if (m_view) m_view->showResult("⚠️ 请先输入 API Key");
        emit requestFailed("请先输入 API Key");
        return;
    }
    m_service->setApiKey(key.trimmed());
    m_view->setLoading(true);
}

void AiAssistantController::onEvalRequested()
{
    beforeRequest();
    if (m_service->apiKey().isEmpty()) return;

    auto *conn = new QMetaObject::Connection();
    *conn = connect(m_service, &AiAssistantService::finished,
        this, [this, conn](const QString &result) {
            disconnect(*conn); delete conn;
            if (m_view) m_view->showResult(result);
            emit resultReady(result);
        });

    m_service->request(AiAssistantService::Evaluation, *m_model);
}

void AiAssistantController::onPolishRequested()
{
    beforeRequest();
    if (m_service->apiKey().isEmpty()) return;

    auto *conn = new QMetaObject::Connection();
    *conn = connect(m_service, &AiAssistantService::finished,
        this, [this, conn](const QString &result) {
            disconnect(*conn); delete conn;
            if (m_view) m_view->showResult(result);
            emit resultReady(result);
        });

    m_service->request(AiAssistantService::PolishResume, *m_model);
}

void AiAssistantController::onHtmlResumeRequested()
{
    beforeRequest();
    if (m_service->apiKey().isEmpty()) return;

    auto *conn = new QMetaObject::Connection();
    *conn = connect(m_service, &AiAssistantService::finished,
        this, [this, conn](const QString &result) {
            disconnect(*conn); delete conn;
            if (m_view) m_view->showResult(result);
            emit resultReady(result);
        });

    m_service->request(AiAssistantService::GenerateHtml, *m_model);
}
