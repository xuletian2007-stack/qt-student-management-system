#include "importview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <QFont>

ImportView::ImportView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ImportView::setupUI()
{
    auto *mainLay = new QVBoxLayout(this);

    auto *gb = new QGroupBox("批量导入课程数据");
    auto *lay = new QVBoxLayout(gb);

    auto *hint = new QLabel("支持 JSON / CSV / PDP 格式文件");
    hint->setStyleSheet("color: #7f8c8d;");
    lay->addWidget(hint);

    auto *btnLay = new QHBoxLayout();
    m_btnSelect = new QPushButton("选择文件...");
    m_btnSelect->setMinimumHeight(36);
    btnLay->addWidget(m_btnSelect);
    btnLay->addStretch();
    lay->addLayout(btnLay);

    m_lblStatus = new QLabel("");
    m_lblStatus->setWordWrap(true);
    lay->addWidget(m_lblStatus);

    m_bar = new QProgressBar();
    m_bar->setRange(0, 0);  // 不确定模式
    m_bar->setVisible(false);
    lay->addWidget(m_bar);

    mainLay->addWidget(gb);
    mainLay->addStretch();

    connect(m_btnSelect, &QPushButton::clicked, this, &ImportView::onSelectFile);
}

void ImportView::onSelectFile()
{
    QString path = QFileDialog::getOpenFileName(this, "选择数据文件", "",
        "数据文件 (*.json *.csv *.pdp);;JSON (*.json);;CSV (*.csv);;PDP (*.pdp)");
    if (path.isEmpty()) return;

    m_selectedPath = path;
    m_lblStatus->setText("已选择：" + path + "\n正在导入...");
    m_bar->setVisible(true);

    emit importRequested(path);
}

void ImportView::showStatus(const QString &msg)
{
    m_lblStatus->setText(msg);
    m_bar->setVisible(false);
}

QString ImportView::getSelectedPath() const
{
    return m_selectedPath;
}
