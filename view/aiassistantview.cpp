#include "aiassistantview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QFont>

AiAssistantView::AiAssistantView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AiAssistantView::setupUI()
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 8, 8, 8);

    // API Key 输入区
    auto *keyGb = new QGroupBox("🔑 API 配置");
    auto *keyLay = new QFormLayout(keyGb);
    m_edApiKey = new QLineEdit();
    m_edApiKey->setEchoMode(QLineEdit::Password);
    m_edApiKey->setPlaceholderText("输入你的 DeepSeek API Key（sk-...）");
    auto *hint = new QLabel("使用 DeepSeek API（兼容 OpenAI 格式），Key 将保存在本地");
    hint->setStyleSheet("color: #7f8c8d; font-size: 11px;");
    keyLay->addRow("API Key：", m_edApiKey);
    keyLay->addRow("", hint);
    lay->addWidget(keyGb);

    // 功能按钮区
    auto *btnGb = new QGroupBox("🤖 AI 功能");
    auto *btnLay = new QVBoxLayout(btnGb);

    auto *row1 = new QHBoxLayout();
    m_btnEval   = new QPushButton("📊 学业评价与发展建议");
    m_btnPolish = new QPushButton("✨ 润色简历文本");
    m_btnEval->setMinimumHeight(40);
    m_btnPolish->setMinimumHeight(40);
    m_btnEval->setStyleSheet(
        "QPushButton { background:#3498db; color:white; font-weight:bold; "
        "font-size:14px; border-radius:6px; padding:8px 20px; }"
        "QPushButton:hover { background:#2980b9; }");
    m_btnPolish->setStyleSheet(
        "QPushButton { background:#27ae60; color:white; font-weight:bold; "
        "font-size:14px; border-radius:6px; padding:8px 20px; }"
        "QPushButton:hover { background:#219a52; }");
    row1->addWidget(m_btnEval);
    row1->addWidget(m_btnPolish);
    btnLay->addLayout(row1);

    auto *row2 = new QHBoxLayout();
    m_btnHtml = new QPushButton("🎨 生成 HTML 网页简历");
    m_btnHtml->setMinimumHeight(40);
    m_btnHtml->setStyleSheet(
        "QPushButton { background:#8e44ad; color:white; font-weight:bold; "
        "font-size:14px; border-radius:6px; padding:8px 20px; }"
        "QPushButton:hover { background:#7d3c98; }");
    row2->addWidget(m_btnHtml);
    btnLay->addLayout(row2);

    m_lblLoading = new QLabel("");
    m_lblLoading->setStyleSheet("color: #e67e22; font-weight: bold; font-size:13px;");
    m_lblLoading->setAlignment(Qt::AlignCenter);
    btnLay->addWidget(m_lblLoading);

    lay->addWidget(btnGb);

    // 结果展示
    m_browser = new QTextBrowser();
    m_browser->setOpenExternalLinks(true);
    m_browser->setPlaceholderText(
        "💡 使用说明：\n"
        "1. 在上方输入你的 DeepSeek API Key\n"
        "2. 先在「个人信息」页面完善个人资料\n"
        "3. 点击下方按钮使用 AI 功能\n\n"
        "📊 学业评价：AI 分析你的学业情况并给出发展建议\n"
        "✨ 润色简历：将你的信息整理成精炼的简历文本\n"
        "🎨 生成网页简历：生成一个精美的 HTML 简历网页");
    lay->addWidget(m_browser);

    connect(m_btnEval,   &QPushButton::clicked, this, &AiAssistantView::evaluationRequested);
    connect(m_btnPolish, &QPushButton::clicked, this, &AiAssistantView::polishRequested);
    connect(m_btnHtml,   &QPushButton::clicked, this, &AiAssistantView::htmlResumeRequested);
}

void AiAssistantView::setApiKey(const QString &key) { m_edApiKey->setText(key); }

void AiAssistantView::showResult(const QString &text)
{
    // 用 QTextBrowser 的文档字体显示，避免 html 渲染问题
    QFont font("PingFang SC", 14);
    font.setStyleHint(QFont::SansSerif);
    m_browser->document()->setDefaultFont(font);
    m_browser->setPlainText(text);
    setLoading(false);
}

void AiAssistantView::setLoading(bool loading)
{
    m_lblLoading->setText(loading ? "⏳ AI 正在思考中..." : "");
    m_btnEval->setEnabled(!loading);
    m_btnPolish->setEnabled(!loading);
    m_btnHtml->setEnabled(!loading);
}
