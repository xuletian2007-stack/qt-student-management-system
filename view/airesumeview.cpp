#include "airesumeview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QFont>

AiResumeView::AiResumeView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AiResumeView::setupUI()
{
    auto *lay = new QVBoxLayout(this);

    auto *btnLay = new QHBoxLayout();
    m_btnGenerate = new QPushButton("生成 AI 简历");
    m_btnGenerate->setMinimumHeight(36);
    m_btnGenerate->setStyleSheet(
        "QPushButton { background-color: #8e44ad; color: white; font-weight: bold; "
        "padding: 8px 24px; border-radius: 4px; }");
    m_lblLoading = new QLabel("");
    m_lblLoading->setStyleSheet("color: #e67e22; font-weight: bold;");
    btnLay->addWidget(m_btnGenerate);
    btnLay->addWidget(m_lblLoading);
    btnLay->addStretch();
    lay->addLayout(btnLay);

    m_browser = new QTextBrowser(this);
    m_browser->setOpenExternalLinks(true);
    m_browser->setPlaceholderText("AI 生成的简历将在此预览...");
    lay->addWidget(m_browser);

    connect(m_btnGenerate, &QPushButton::clicked, this, [this]() {
        emit generateRequested();
    });
}

void AiResumeView::loadAiHtml(const QString &html)
{
    m_browser->setHtml(html);
    toggleLoading(false);
}

void AiResumeView::toggleLoading(bool loading)
{
    m_lblLoading->setText(loading ? "正在生成中..." : "");
    m_btnGenerate->setEnabled(!loading);
}
