#include "resumeview.h"
#include "../model/resumegenerator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QTextDocument>
#include <QPrinter>
#include <QPageSize>

ResumeView::ResumeView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ResumeView::setupUI()
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 8, 8, 8);

    auto *btnLay = new QHBoxLayout();
    m_btnExport = new QPushButton("📥 导出 PDF 简历文件");
    m_btnExport->setMinimumHeight(36);
    m_btnExport->setStyleSheet(
        "QPushButton { background:#27ae60; color:white; font-weight:bold; "
        "font-size:14px; padding:8px 24px; border-radius:4px; }");
    btnLay->addWidget(m_btnExport);
    btnLay->addStretch();
    lay->addLayout(btnLay);

    m_preview = new QTextBrowser(this);
    m_preview->setOpenExternalLinks(true);
    m_preview->setPlaceholderText("请先在「个人信息」页面完善资料，简历将在此实时预览...");
    lay->addWidget(m_preview);

    connect(m_btnExport, &QPushButton::clicked, this, &ResumeView::onExport);
}

void ResumeView::setHtmlPreview(const QString &html)
{
    m_preview->setHtml(html);
}

void ResumeView::onExport()
{
    QString path = QFileDialog::getSaveFileName(this, "导出简历",
        "个人简历.pdf", "PDF文件 (*.pdf)");
    if (path.isEmpty()) return;

    QTextDocument doc;
    doc.setHtml(m_preview->toHtml());

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageSize(QPageSize(QPageSize::A4));

    doc.print(&printer);
    QMessageBox::information(this, "成功", "PDF 简历已导出到：\n" + path);
}
