#include "resumecontroller.h"

ResumeController::ResumeController(QObject *parent)
    : QObject(parent)
{
}

void ResumeController::onExportResumeRequested()
{
    generateStandardResume();
}

void ResumeController::generateStandardResume()
{
    if (!m_model || !m_generator) return;

    QString html = m_generator->fillTemplate(*m_model);
    emit standardResumeGenerated(html);

    // 同时导出 PDF
    m_generator->exportToPdf(html);
}
