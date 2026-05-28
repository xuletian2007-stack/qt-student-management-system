#include "achievementview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFormLayout>
#include <QLineEdit>
#include <QDateEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QFileInfo>

AchievementView::AchievementView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AchievementView::setupUI()
{
    auto *lay = new QVBoxLayout(this);

    // 操作栏
    auto *topLay = new QHBoxLayout();
    m_btnAdd    = new QPushButton("添加成就");
    m_btnEdit   = new QPushButton("编辑选中");
    m_btnDelete = new QPushButton("删除选中");

    m_cmbFilter = new QComboBox();
    m_cmbFilter->addItems({"全部", "奖项", "班级角色", "志愿者", "其他"});

    topLay->addWidget(m_btnAdd);
    topLay->addWidget(m_btnEdit);
    topLay->addWidget(m_btnDelete);
    topLay->addStretch();
    topLay->addWidget(new QLabel("筛选:"));
    topLay->addWidget(m_cmbFilter);
    lay->addLayout(topLay);

    // 表格
    m_table = new QTableWidget(0, 4, this);
    m_table->setHorizontalHeaderLabels({"标题", "类别", "日期", "描述"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    lay->addWidget(m_table);

    connect(m_btnAdd,    &QPushButton::clicked, this, &AchievementView::onAdd);
    connect(m_btnEdit,   &QPushButton::clicked, this, &AchievementView::onEdit);
    connect(m_btnDelete, &QPushButton::clicked, this, &AchievementView::onDelete);
    connect(m_cmbFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AchievementView::onFilterChanged);
}

void AchievementView::refreshList(const QList<Achievement> &data)
{
    QString filter = m_cmbFilter->currentText();
    m_table->setRowCount(0);
    int row = 0;
    for (const auto &a : data) {
        if (filter != "全部" && a.type() != filter) continue;
        m_table->setRowCount(row + 1);
        m_table->setItem(row, 0, new QTableWidgetItem(a.title()));
        m_table->setItem(row, 1, new QTableWidgetItem(a.type()));
        m_table->setItem(row, 2, new QTableWidgetItem(a.date().toString("yyyy-MM-dd")));
        m_table->setItem(row, 3, new QTableWidgetItem(a.description()));
        ++row;
    }
}

Achievement AchievementView::getFormData() const
{
    return Achievement();
}

void AchievementView::onAdd()
{
    QDialog dlg(this);
    dlg.setWindowTitle("添加成就");
    auto *form = new QFormLayout(&dlg);

    auto *edTitle = new QLineEdit(&dlg);
    auto *cmbCat  = new QComboBox(&dlg);
    cmbCat->addItems({"奖项", "班级角色", "志愿者", "其他"});
    auto *edDate  = new QDateEdit(QDate::currentDate(), &dlg);
    edDate->setCalendarPopup(true);
    auto *edDesc  = new QTextEdit(&dlg);
    edDesc->setMaximumHeight(80);
    auto *btnProof = new QPushButton("选择证明材料...", &dlg);
    auto *lblProof = new QLabel("", &dlg);
    lblProof->setStyleSheet("color: #7f8c8d; font-size: 11px;");
    QString proofPath;

    connect(btnProof, &QPushButton::clicked, [&]() {
        proofPath = QFileDialog::getOpenFileName(&dlg, "选择证明材料", "",
            "所有文件 (*);;图片 (*.png *.jpg *.jpeg);;PDF (*.pdf)");
        if (!proofPath.isEmpty()) {
            QFileInfo fi(proofPath);
            btnProof->setText(fi.fileName());
            lblProof->setText(proofPath);
        }
    });

    form->addRow("标题", edTitle);
    form->addRow("类别", cmbCat);
    form->addRow("日期", edDate);
    form->addRow("描述", edDesc);
    form->addRow("证明材料", btnProof);
    if (!proofPath.isEmpty()) form->addRow("", lblProof);

    auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(btnBox);
    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        Achievement a;
        a.setTitle(edTitle->text());
        a.setType(cmbCat->currentText());
        a.setDate(edDate->date());
        a.setDescription(edDesc->toPlainText());
        a.setProofPath(proofPath);
        emit addRequested(a);
    }
}

void AchievementView::onEdit()
{
    int r = m_table->currentRow();
    if (r < 0) { QMessageBox::information(this, "提示", "请先选中一行"); return; }

    // 通过标题+日期在原始数据中查找索引
    QString title = m_table->item(r, 0)->text();
    QString dateStr = m_table->item(r, 2)->text();

    // 我们需要从外部传入 data 才能精确匹配，这里先走简单的对话框
    QDialog dlg(this);
    dlg.setWindowTitle("编辑成就");
    auto *form = new QFormLayout(&dlg);

    auto *edTitle = new QLineEdit(title, &dlg);
    auto *cmbCat  = new QComboBox(&dlg);
    cmbCat->addItems({"奖项", "班级角色", "志愿者", "其他"});
    cmbCat->setCurrentText(m_table->item(r, 1)->text());
    auto *edDate  = new QDateEdit(QDate::fromString(dateStr, "yyyy-MM-dd"), &dlg);
    edDate->setCalendarPopup(true);
    auto *edDesc  = new QTextEdit(&dlg);
    edDesc->setMaximumHeight(80);
    edDesc->setPlainText(m_table->item(r, 3)->text());

    form->addRow("标题", edTitle);
    form->addRow("类别", cmbCat);
    form->addRow("日期", edDate);
    form->addRow("描述", edDesc);

    auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(btnBox);
    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        Achievement a;
        a.setTitle(edTitle->text());
        a.setType(cmbCat->currentText());
        a.setDate(edDate->date());
        a.setDescription(edDesc->toPlainText());
        // 索引需要通过外部查找，这里传 r 作为临时方案
        emit updateRequested(r, a);
    }
}

void AchievementView::onDelete()
{
    int r = m_table->currentRow();
    if (r < 0) { QMessageBox::information(this, "提示", "请先选中一行"); return; }

    auto reply = QMessageBox::question(this, "确认", "确定要删除这条成就记录吗？",
                                        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
        emit removeRequested(r);
}

void AchievementView::onFilterChanged(int) { /* 由外部 refreshList 触发 */ }
