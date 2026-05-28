#include "profileview.h"
#include "../model/skill.h"
#include "../model/experience.h"
#include "../model/achievement.h"
#include "../model/education.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QMessageBox>
#include <QFont>
#include <QFileDialog>
#include <QPixmap>

ProfileView::ProfileView(QWidget *parent) : QWidget(parent) { setupUI(); }

void ProfileView::setupUI()
{
    auto *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(8, 8, 8, 8);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet("QTabBar::tab { font-size: 13px; padding: 8px 14px; }");
    m_tabWidget->addTab(createBasicInfoTab(),   "基本信息");
    m_tabWidget->addTab(createEducationTab(),   "教育背景");
    m_tabWidget->addTab(createSkillsTab(),      "专业技能");
    m_tabWidget->addTab(createExperienceTab(),  "项目与实习");
    m_tabWidget->addTab(createAchievementTab(), "荣誉与奖项");
    mainLay->addWidget(m_tabWidget);

    auto *btnLay = new QHBoxLayout();
    btnLay->addStretch();
    auto *btnSave = new QPushButton("💾 保存所有信息");
    btnSave->setMinimumHeight(36);
    btnSave->setStyleSheet("QPushButton { background:#27ae60; color:white; font-weight:bold; "
                           "padding:8px 32px; border-radius:4px; font-size:14px; }");
    connect(btnSave, &QPushButton::clicked, this, &ProfileView::profileChanged);
    btnLay->addWidget(btnSave);
    mainLay->addLayout(btnLay);
}

// ====== 基本信息 ======
QWidget* ProfileView::createBasicInfoTab()
{
    auto *w = new QWidget(this);
    auto *lay = new QHBoxLayout(w);

    auto *form = new QFormLayout();
    form->setSpacing(8);
    m_edName      = new QLineEdit(w); m_edName->setPlaceholderText("姓名");
    m_edStudentId = new QLineEdit(w); m_edStudentId->setPlaceholderText("学号");
    m_cmbGender   = new QComboBox(w); m_cmbGender->addItems({"", "男", "女"});
    m_cmbPolitical= new QLineEdit(w); m_cmbPolitical->setPlaceholderText("如：共青团员");
    m_edBirthday  = new QLineEdit(w); m_edBirthday->setPlaceholderText("如 2005-01-15");
    m_edEmail     = new QLineEdit(w); m_edEmail->setPlaceholderText("邮箱");
    m_edPhone     = new QLineEdit(w); m_edPhone->setPlaceholderText("手机号");

    form->addRow("姓名：", m_edName);
    form->addRow("学号：", m_edStudentId);
    form->addRow("性别：", m_cmbGender);
    form->addRow("政治面貌：", m_cmbPolitical);
    form->addRow("出生日期：", m_edBirthday);
    form->addRow("邮箱：", m_edEmail);
    form->addRow("电话：", m_edPhone);
    lay->addLayout(form);

    // 右侧：照片 + 性格爱好
    auto *right = new QVBoxLayout();
    m_lblPhoto = new QLabel(w);
    m_lblPhoto->setFixedSize(120, 140);
    m_lblPhoto->setStyleSheet("border:1px dashed #ccc; background:#f9f9f9;");
    m_lblPhoto->setAlignment(Qt::AlignCenter);
    m_lblPhoto->setText("点击上传\n个人照片");
    m_lblPhoto->setCursor(Qt::PointingHandCursor);
    right->addWidget(m_lblPhoto);

    auto *btnPhoto = new QPushButton("选择照片");
    btnPhoto->setStyleSheet("QPushButton { font-size:11px; }");
    right->addWidget(btnPhoto);
    right->addStretch();
    lay->addLayout(right);

    // 照片上传 — 存 base64 到数据文件
    connect(btnPhoto, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "选择个人照片", "",
            "图片文件 (*.png *.jpg *.jpeg)");
        if (!path.isEmpty()) {
            m_photoPath = path;
            QPixmap pm(path);
            if (!pm.isNull()) {
                m_lblPhoto->setPixmap(pm.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                // 同时编码 base64（保存在 saveToProfile 时写入）
                QFile f(path);
                if (f.open(QIODevice::ReadOnly)) {
                    QByteArray raw = f.readAll();
                    f.close();
                    QString ext = QFileInfo(path).suffix().toLower();
                    m_photoBase64 = QString("data:image/%1;base64,%2")
                        .arg(ext == "jpg" ? "jpeg" : ext)
                        .arg(QString::fromLatin1(raw.toBase64()));
                }
            }
        }
    });

    // 性格 & 爱好
    auto *form2 = new QFormLayout();
    m_edPersonality = new QTextEdit(w); m_edPersonality->setMaximumHeight(60);
    m_edPersonality->setPlaceholderText("描述你的性格特点");
    m_edHobby       = new QTextEdit(w); m_edHobby->setMaximumHeight(60);
    m_edHobby->setPlaceholderText("你的兴趣爱好");
    form2->addRow("个人性格：", m_edPersonality);
    form2->addRow("爱好：", m_edHobby);
    lay->addLayout(form2);

    return w;
}

// ====== 教育背景（多段列表） ======
QWidget* ProfileView::createEducationTab()
{
    auto *w = new QWidget(this);
    auto *lay = new QVBoxLayout(w);

    auto *btnLay = new QHBoxLayout();
    auto *btnAdd = new QPushButton("+ 添加教育经历");
    auto *btnEdit= new QPushButton("查看/编辑");
    auto *btnDel = new QPushButton("删除选中");
    btnLay->addWidget(btnAdd); btnLay->addWidget(btnEdit); btnLay->addWidget(btnDel); btnLay->addStretch();
    lay->addLayout(btnLay);

    m_eduTable = new QTableWidget(0, 4);
    m_eduTable->setHorizontalHeaderLabels({"学校", "专业", "学位", "起止时间"});
    m_eduTable->horizontalHeader()->setStretchLastSection(true);
    m_eduTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_eduTable->setColumnWidth(0, 140);
    m_eduTable->setColumnWidth(1, 160);
    m_eduTable->setColumnWidth(2, 100);
    m_eduTable->setStyleSheet("QTableWidget { font-size:13px; } QHeaderView::section { font-size:13px; font-weight:bold; }");
    lay->addWidget(m_eduTable);

    auto showDlg = [this](int editRow) {
        QDialog dlg(this);
        dlg.setWindowTitle(editRow >= 0 ? "编辑教育经历" : "添加教育经历");
        auto *f = new QFormLayout(&dlg);
        auto *e1 = new QLineEdit(&dlg); e1->setPlaceholderText("学校名称");
        auto *e2 = new QLineEdit(&dlg); e2->setPlaceholderText("专业");
        auto *cmb = new QComboBox(&dlg); cmb->addItems({"本科", "硕士", "博士", "高中"});
        auto *e4 = new QLineEdit(&dlg); e4->setPlaceholderText("如 2024");
        auto *e5 = new QLineEdit(&dlg); e5->setPlaceholderText("如 2028");
        if (editRow >= 0) {
            e1->setText(m_eduTable->item(editRow, 0) ? m_eduTable->item(editRow, 0)->text() : "");
            e2->setText(m_eduTable->item(editRow, 1) ? m_eduTable->item(editRow, 1)->text() : "");
            cmb->setCurrentText(m_eduTable->item(editRow, 2) ? m_eduTable->item(editRow, 2)->text() : "");
            QString tr = m_eduTable->item(editRow, 3) ? m_eduTable->item(editRow, 3)->text() : "";
            e4->setText(tr.split(" — ").value(0));
            e5->setText(tr.split(" — ").value(1));
        }
        f->addRow("学校：", e1); f->addRow("专业：", e2);
        f->addRow("学位：", cmb); f->addRow("入学：", e4); f->addRow("毕业：", e5);
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        f->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() == QDialog::Accepted && !e1->text().trimmed().isEmpty()) {
            int r = (editRow >= 0) ? editRow : m_eduTable->rowCount();
            if (editRow < 0) m_eduTable->setRowCount(r+1);
            m_eduTable->setItem(r, 0, new QTableWidgetItem(e1->text().trimmed()));
            m_eduTable->setItem(r, 1, new QTableWidgetItem(e2->text().trimmed()));
            m_eduTable->setItem(r, 2, new QTableWidgetItem(cmb->currentText()));
            m_eduTable->setItem(r, 3, new QTableWidgetItem(e4->text().trimmed() + " — " + e5->text().trimmed()));
        }
    };

    connect(btnAdd, &QPushButton::clicked, [showDlg]() { showDlg(-1); });
    connect(btnEdit, &QPushButton::clicked, [this, showDlg]() {
        int r = m_eduTable->currentRow(); if (r >= 0) showDlg(r);
    });
    connect(btnDel, &QPushButton::clicked, [this]() {
        int r = m_eduTable->currentRow(); if (r >= 0) m_eduTable->removeRow(r);
    });
    return w;
}

// ====== 专业技能 ======
QWidget* ProfileView::createSkillsTab()
{
    auto *w = new QWidget(this);
    auto *lay = new QVBoxLayout(w);
    auto *btnLay = new QHBoxLayout();
    auto *btnAdd = new QPushButton("+ 添加技能");
    auto *btnEdit= new QPushButton("查看/编辑");
    auto *btnDel = new QPushButton("删除选中");
    btnLay->addWidget(btnAdd); btnLay->addWidget(btnEdit); btnLay->addWidget(btnDel); btnLay->addStretch();
    lay->addLayout(btnLay);
    m_skillTable = new QTableWidget(0, 2);
    m_skillTable->setHorizontalHeaderLabels({"技能名称", "熟练程度"});
    m_skillTable->horizontalHeader()->setStretchLastSection(true);
    m_skillTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_skillTable->setColumnWidth(0, 300);
    m_skillTable->setStyleSheet("QTableWidget { font-size:13px; } QHeaderView::section { font-size:13px; font-weight:bold; }");
    lay->addWidget(m_skillTable);

    auto showDlg = [this](int editRow) {
        QDialog dlg(this); dlg.setWindowTitle(editRow >= 0 ? "编辑技能" : "添加技能");
        auto *f = new QFormLayout(&dlg);
        auto *e1 = new QLineEdit(&dlg); e1->setPlaceholderText("如 C++");
        auto *e2 = new QLineEdit(&dlg); e2->setPlaceholderText("如：精通、宗师级...");
        if (editRow >= 0) {
            e1->setText(m_skillTable->item(editRow, 0) ? m_skillTable->item(editRow, 0)->text() : "");
            e2->setText(m_skillTable->item(editRow, 1) ? m_skillTable->item(editRow, 1)->text() : "");
        }
        f->addRow("名称：", e1); f->addRow("程度：", e2);
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        f->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() == QDialog::Accepted && !e1->text().trimmed().isEmpty()) {
            int r = (editRow >= 0) ? editRow : m_skillTable->rowCount();
            if (editRow < 0) m_skillTable->setRowCount(r+1);
            m_skillTable->setItem(r, 0, new QTableWidgetItem(e1->text().trimmed()));
            m_skillTable->setItem(r, 1, new QTableWidgetItem(e2->text()));
        }
    };
    connect(btnAdd, &QPushButton::clicked, [showDlg]() { showDlg(-1); });
    connect(btnEdit, &QPushButton::clicked, [this, showDlg]() {
        int r = m_skillTable->currentRow(); if (r >= 0) showDlg(r);
    });
    connect(btnDel, &QPushButton::clicked, [this]() {
        int r = m_skillTable->currentRow(); if (r >= 0) m_skillTable->removeRow(r);
    });
    return w;
}

// ====== 项目实习 ======
QWidget* ProfileView::createExperienceTab()
{
    auto *w = new QWidget(this);
    auto *lay = new QVBoxLayout(w);
    auto *btnLay = new QHBoxLayout();
    auto *btnAdd = new QPushButton("+ 添加经历");
    auto *btnEdit= new QPushButton("查看/编辑");
    auto *btnDel = new QPushButton("删除选中");
    btnLay->addWidget(btnAdd); btnLay->addWidget(btnEdit); btnLay->addWidget(btnDel); btnLay->addStretch();
    lay->addLayout(btnLay);
    m_expTable = new QTableWidget(0, 4);
    m_expTable->setHorizontalHeaderLabels({"项目/公司", "角色", "时间", "描述"});
    m_expTable->horizontalHeader()->setStretchLastSection(true);
    m_expTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_expTable->setColumnWidth(0, 170);
    m_expTable->setColumnWidth(1, 100);
    m_expTable->setColumnWidth(2, 120);
    m_expTable->setStyleSheet("QTableWidget { font-size:13px; } QHeaderView::section { font-size:13px; font-weight:bold; }");
    lay->addWidget(m_expTable);

    auto showDlg = [this](int editRow) {
        QDialog dlg(this); dlg.setWindowTitle(editRow >= 0 ? "编辑经历" : "添加经历");
        dlg.resize(400, 300);
        auto *f = new QFormLayout(&dlg);
        auto *e1 = new QLineEdit(&dlg); e1->setPlaceholderText("项目/实习名称");
        auto *e2 = new QLineEdit(&dlg); e2->setPlaceholderText("角色");
        auto *e3 = new QLineEdit(&dlg); e3->setPlaceholderText("如 2024-06");
        auto *e4 = new QLineEdit(&dlg); e4->setPlaceholderText("如 2024-09");
        auto *e5 = new QTextEdit(&dlg); e5->setMaximumHeight(100);
        if (editRow >= 0) {
            e1->setText(m_expTable->item(editRow, 0) ? m_expTable->item(editRow, 0)->text() : "");
            e2->setText(m_expTable->item(editRow, 1) ? m_expTable->item(editRow, 1)->text() : "");
            QString tr = m_expTable->item(editRow, 2) ? m_expTable->item(editRow, 2)->text() : "";
            e3->setText(tr.split(" — ").value(0));
            e4->setText(tr.split(" — ").value(1));
            e5->setPlainText(m_expTable->item(editRow, 3) ? m_expTable->item(editRow, 3)->text() : "");
        }
        f->addRow("名称：", e1); f->addRow("角色：", e2);
        f->addRow("开始：", e3); f->addRow("结束：", e4); f->addRow("描述：", e5);
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        f->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() == QDialog::Accepted && !e1->text().trimmed().isEmpty()) {
            int r = (editRow >= 0) ? editRow : m_expTable->rowCount();
            if (editRow < 0) m_expTable->setRowCount(r+1);
            m_expTable->setItem(r, 0, new QTableWidgetItem(e1->text().trimmed()));
            m_expTable->setItem(r, 1, new QTableWidgetItem(e2->text().trimmed()));
            m_expTable->setItem(r, 2, new QTableWidgetItem(e3->text().trimmed() + " — " + e4->text().trimmed()));
            m_expTable->setItem(r, 3, new QTableWidgetItem(e5->toPlainText().trimmed()));
        }
    };
    connect(btnAdd, &QPushButton::clicked, [showDlg]() { showDlg(-1); });
    connect(btnEdit, &QPushButton::clicked, [this, showDlg]() {
        int r = m_expTable->currentRow(); if (r >= 0) showDlg(r);
    });
    connect(btnDel, &QPushButton::clicked, [this]() {
        int r = m_expTable->currentRow(); if (r >= 0) m_expTable->removeRow(r);
    });
    return w;
}

// ====== 荣誉奖项 ======
QWidget* ProfileView::createAchievementTab()
{
    auto *w = new QWidget(this);
    auto *lay = new QVBoxLayout(w);
    auto *btnLay = new QHBoxLayout();
    auto *btnAdd = new QPushButton("+ 添加奖项");
    auto *btnEdit= new QPushButton("查看/编辑");
    auto *btnDel = new QPushButton("删除选中");
    btnLay->addWidget(btnAdd); btnLay->addWidget(btnEdit); btnLay->addWidget(btnDel); btnLay->addStretch();
    lay->addLayout(btnLay);
    m_achTable = new QTableWidget(0, 4);
    m_achTable->setHorizontalHeaderLabels({"标题", "级别", "日期", "描述"});
    m_achTable->horizontalHeader()->setStretchLastSection(true);
    m_achTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_achTable->setColumnWidth(0, 200);
    m_achTable->setColumnWidth(1, 80);
    m_achTable->setColumnWidth(2, 110);
    m_achTable->setStyleSheet("QTableWidget { font-size:13px; } QHeaderView::section { font-size:13px; font-weight:bold; }");
    lay->addWidget(m_achTable);

    auto showDlg = [this](int editRow) {
        QDialog dlg(this); dlg.setWindowTitle(editRow >= 0 ? "编辑奖项" : "添加荣誉/奖项");
        auto *f = new QFormLayout(&dlg);
        auto *e1 = new QLineEdit(&dlg); e1->setPlaceholderText("奖项标题");
        auto *e2 = new QLineEdit(&dlg); e2->setPlaceholderText("如：国家级、世界级...");
        auto *e3 = new QDateEdit(QDate::currentDate(), &dlg); e3->setCalendarPopup(true);
        auto *e4 = new QTextEdit(&dlg); e4->setMaximumHeight(60);
        if (editRow >= 0) {
            e1->setText(m_achTable->item(editRow, 0) ? m_achTable->item(editRow, 0)->text() : "");
            e2->setText(m_achTable->item(editRow, 1) ? m_achTable->item(editRow, 1)->text() : "");
            e3->setDate(QDate::fromString(m_achTable->item(editRow, 2) ? m_achTable->item(editRow, 2)->text() : "", "yyyy-MM-dd"));
            e4->setPlainText(m_achTable->item(editRow, 3) ? m_achTable->item(editRow, 3)->text() : "");
        }
        f->addRow("标题：", e1); f->addRow("级别：", e2);
        f->addRow("日期：", e3); f->addRow("描述：", e4);
        auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        f->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() == QDialog::Accepted && !e1->text().trimmed().isEmpty()) {
            int r = (editRow >= 0) ? editRow : m_achTable->rowCount();
            if (editRow < 0) m_achTable->setRowCount(r+1);
            m_achTable->setItem(r, 0, new QTableWidgetItem(e1->text().trimmed()));
            m_achTable->setItem(r, 1, new QTableWidgetItem(e2->text().trimmed()));
            m_achTable->setItem(r, 2, new QTableWidgetItem(e3->date().toString("yyyy-MM-dd")));
            m_achTable->setItem(r, 3, new QTableWidgetItem(e4->toPlainText().trimmed()));
        }
    };
    connect(btnAdd, &QPushButton::clicked, [showDlg]() { showDlg(-1); });
    connect(btnEdit, &QPushButton::clicked, [this, showDlg]() {
        int r = m_achTable->currentRow(); if (r >= 0) showDlg(r);
    });
    connect(btnDel, &QPushButton::clicked, [this]() {
        int r = m_achTable->currentRow(); if (r >= 0) m_achTable->removeRow(r);
    });
    return w;
}

// ====== 加载 ======
void ProfileView::loadFromProfile(const StudentProfile &profile)
{
    m_edName->setText(profile.name());
    m_edStudentId->setText(profile.studentId());
    m_cmbGender->setCurrentText(profile.gender());
    m_cmbPolitical->setText(profile.political());
    m_edBirthday->setText(profile.birthday());
    m_edEmail->setText(profile.email());
    m_edPhone->setText(profile.phone());
    m_edPersonality->setPlainText(profile.personality());
    m_edHobby->setPlainText(profile.hobby());
    m_photoPath = profile.photoPath();
    m_photoBase64 = profile.photoBase64();

    // 优先用 base64，fallback 到文件路径
    if (!m_photoBase64.isEmpty()) {
        QByteArray raw = QByteArray::fromBase64(
            m_photoBase64.section(",", 1).toLatin1());
        QPixmap pm;
        pm.loadFromData(raw);
        if (!pm.isNull())
            m_lblPhoto->setPixmap(pm.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (!m_photoPath.isEmpty()) {
        QPixmap pm(m_photoPath);
        if (!pm.isNull())
            m_lblPhoto->setPixmap(pm.scaled(120, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // 教育
    m_eduTable->setRowCount(0);
    for (const auto &e : profile.educations()) {
        int r = m_eduTable->rowCount(); m_eduTable->setRowCount(r+1);
        m_eduTable->setItem(r, 0, new QTableWidgetItem(e.schoolName));
        m_eduTable->setItem(r, 1, new QTableWidgetItem(e.major));
        m_eduTable->setItem(r, 2, new QTableWidgetItem(e.degree));
        m_eduTable->setItem(r, 3, new QTableWidgetItem(e.startYear + " — " + e.endYear));
    }

    // 技能
    m_skillTable->setRowCount(0);
    for (const auto &s : profile.skills()) {
        int r = m_skillTable->rowCount(); m_skillTable->setRowCount(r+1);
        m_skillTable->setItem(r, 0, new QTableWidgetItem(s.name));
        m_skillTable->setItem(r, 1, new QTableWidgetItem(s.level));
    }

    // 经历
    m_expTable->setRowCount(0);
    for (const auto &e : profile.experiences()) {
        int r = m_expTable->rowCount(); m_expTable->setRowCount(r+1);
        m_expTable->setItem(r, 0, new QTableWidgetItem(e.title));
        m_expTable->setItem(r, 1, new QTableWidgetItem(e.role));
        m_expTable->setItem(r, 2, new QTableWidgetItem(e.startDate + " — " + e.endDate));
        m_expTable->setItem(r, 3, new QTableWidgetItem(e.description));
    }

    // 奖项
    m_achTable->setRowCount(0);
    for (const auto &a : profile.achievements()) {
        int r = m_achTable->rowCount(); m_achTable->setRowCount(r+1);
        m_achTable->setItem(r, 0, new QTableWidgetItem(a.title()));
        m_achTable->setItem(r, 1, new QTableWidgetItem(a.type()));
        m_achTable->setItem(r, 2, new QTableWidgetItem(a.date().toString("yyyy-MM-dd")));
        m_achTable->setItem(r, 3, new QTableWidgetItem(a.description()));
    }
}

// ====== 保存 ======
void ProfileView::saveToProfile(StudentProfile &profile) const
{
    profile.setName(m_edName->text().trimmed());
    profile.setStudentId(m_edStudentId->text().trimmed());
    profile.setGender(m_cmbGender->currentText());
    profile.setPolitical(m_cmbPolitical->text());
    profile.setBirthday(m_edBirthday->text().trimmed());
    profile.setEmail(m_edEmail->text().trimmed());
    profile.setPhone(m_edPhone->text().trimmed());
    profile.setPersonality(m_edPersonality->toPlainText().trimmed());
    profile.setHobby(m_edHobby->toPlainText().trimmed());
    profile.setPhotoPath(m_photoPath);
    profile.setPhotoBase64(m_photoBase64);

    // 教育背景
    QList<Education> edus;
    for (int i = 0; i < m_eduTable->rowCount(); i++) {
        Education e;
        e.schoolName = m_eduTable->item(i, 0) ? m_eduTable->item(i, 0)->text() : "";
        e.major      = m_eduTable->item(i, 1) ? m_eduTable->item(i, 1)->text() : "";
        e.degree     = m_eduTable->item(i, 2) ? m_eduTable->item(i, 2)->text() : "";
        QString tr   = m_eduTable->item(i, 3) ? m_eduTable->item(i, 3)->text() : "";
        e.startYear  = tr.split(" — ").value(0);
        e.endYear    = tr.split(" — ").value(1);
        if (!e.schoolName.isEmpty()) edus.append(e);
    }
    profile.setEducations(edus);

    QList<Skill> skills;
    for (int i = 0; i < m_skillTable->rowCount(); i++) {
        Skill s;
        s.name  = m_skillTable->item(i, 0) ? m_skillTable->item(i, 0)->text() : "";
        s.level = m_skillTable->item(i, 1) ? m_skillTable->item(i, 1)->text() : "";
        if (!s.name.isEmpty()) skills.append(s);
    }
    profile.setSkills(skills);

    QList<Experience> exps;
    for (int i = 0; i < m_expTable->rowCount(); i++) {
        Experience e;
        e.title = m_expTable->item(i, 0) ? m_expTable->item(i, 0)->text() : "";
        e.role  = m_expTable->item(i, 1) ? m_expTable->item(i, 1)->text() : "";
        QString tr = m_expTable->item(i, 2) ? m_expTable->item(i, 2)->text() : "";
        e.startDate = tr.split(" — ").value(0);
        e.endDate   = tr.split(" — ").value(1);
        e.description = m_expTable->item(i, 3) ? m_expTable->item(i, 3)->text() : "";
        if (!e.title.isEmpty()) exps.append(e);
    }
    profile.setExperiences(exps);

    QList<Achievement> achs;
    for (int i = 0; i < m_achTable->rowCount(); i++) {
        Achievement a;
        a.setTitle(m_achTable->item(i, 0) ? m_achTable->item(i, 0)->text() : "");
        a.setType(m_achTable->item(i, 1) ? m_achTable->item(i, 1)->text() : "");
        a.setDate(QDate::fromString(m_achTable->item(i, 2) ? m_achTable->item(i, 2)->text() : "", "yyyy-MM-dd"));
        a.setDescription(m_achTable->item(i, 3) ? m_achTable->item(i, 3)->text() : "");
        if (!a.title().isEmpty()) achs.append(a);
    }
    profile.setAchievements(achs);
}
