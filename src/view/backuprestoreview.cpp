#include "backuprestoreview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>

BackupRestoreView::BackupRestoreView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void BackupRestoreView::setupUI()
{
    auto *mainLay = new QVBoxLayout(this);

    auto *gb = new QGroupBox("备份与恢复");
    auto *lay = new QVBoxLayout(gb);

    auto *hint = new QLabel("备份：导出当前所有数据到 PDP 文件\n恢复：从 PDP 备份文件覆盖现有数据");
    hint->setStyleSheet("color: #7f8c8d; margin-bottom: 8px;");
    lay->addWidget(hint);

    auto *pathLay = new QHBoxLayout();
    m_editPath = new QLineEdit();
    m_editPath->setPlaceholderText("选择或输入文件路径...");
    auto *btnBrowse = new QPushButton("浏览...");
    pathLay->addWidget(m_editPath);
    pathLay->addWidget(btnBrowse);
    lay->addLayout(pathLay);

    m_btnAction = new QPushButton("执行备份");
    m_btnAction->setMinimumHeight(36);
    m_btnAction->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; font-weight: bold; "
        "padding: 8px 24px; border-radius: 4px; }");
    lay->addWidget(m_btnAction);

    mainLay->addWidget(gb);
    mainLay->addStretch();

    connect(btnBrowse, &QPushButton::clicked, this, [this]() {
        QString path;
        if (m_isBackup) {
            path = QFileDialog::getSaveFileName(this, "备份位置", "backup.pdp",
                                                 "PDP文件 (*.pdp)");
        } else {
            path = QFileDialog::getOpenFileName(this, "选择备份文件", "",
                                                 "PDP文件 (*.pdp);;所有文件 (*)");
        }
        if (!path.isEmpty()) m_editPath->setText(path);
    });

    connect(m_btnAction, &QPushButton::clicked, this, [this]() {
        emit actionRequested();
    });
}

void BackupRestoreView::setMode(bool isBackup)
{
    m_isBackup = isBackup;
    if (isBackup) {
        m_btnAction->setText("执行备份");
        m_btnAction->setStyleSheet(
            "QPushButton { background-color: #27ae60; color: white; font-weight: bold; "
            "padding: 8px 24px; border-radius: 4px; }");
    } else {
        m_btnAction->setText("执行恢复");
        m_btnAction->setStyleSheet(
            "QPushButton { background-color: #e74c3c; color: white; font-weight: bold; "
            "padding: 8px 24px; border-radius: 4px; }");
    }
}

QString BackupRestoreView::getPath() const
{
    return m_editPath->text();
}
