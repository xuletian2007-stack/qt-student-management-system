#include "airesumecontroller.h"

AiResumeController::AiResumeController(QObject *parent)
    : QObject(parent)
{
}

void AiResumeController::onGenerateAiResumeRequested()
{
    if (!m_model || !m_service) return;
    callAiApi();
}

void AiResumeController::callAiApi()
{
    // 连接 AI 服务的信号到本控制器的信号
    // 使用 QMetaObject::Connection 手动管理一次连接
    auto *conn = new QMetaObject::Connection();
    *conn = connect(m_service, &AiResumeService::resumeFinished,
            this, [this, conn](const QString &html) {
        disconnect(*conn);
        delete conn;
        if (html.isEmpty())
            emit aiResumeFailed("AI 生成失败");
        else
            emit aiResumeGenerated(html);
    });

    m_service->requestAiResume(*m_model);
}
