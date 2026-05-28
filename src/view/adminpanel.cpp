#include "adminpanel.h"
#include "../model/accountmanager.h"
#include "../model/gpacalculator.h"
#include "../model/courserecord.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QFont>

AdminPanel::AdminPanel(QWidget *parent) : QWidget(parent)
{
    m_db = DatabaseManager::getInstance();
    setupUI();
}

void AdminPanel::setupUI()
{
    auto *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(12, 12, 12, 12);

    // 顶部：学生选择 + GPA 概览
    auto *topLay = new QHBoxLayout();
    auto *lbl = new QLabel("选择学生：");
    lbl->setStyleSheet("font-size:14px; font-weight:bold;");
    m_studentCombo = new QComboBox();
    m_studentCombo->setMinimumWidth(150);
    m_studentCombo->setStyleSheet("font-size:14px; padding:4px 8px;");
    m_lblGPA = new QLabel("");
    m_lblGPA->setStyleSheet("font-size:14px; font-weight:bold; color:#3498db; margin-left:16px;");
    topLay->addWidget(lbl);
    topLay->addWidget(m_studentCombo);
    topLay->addWidget(m_lblGPA);
    topLay->addStretch();
    mainLay->addLayout(topLay);

    // Tab：课程成绩 | 批量导入 | 全量数据
    m_tabWidget = new QTabWidget();
    m_tabWidget->setStyleSheet("QTabBar::tab { font-size:13px; padding:8px 16px; }");

    // --- Tab1: 课程成绩 ---
    auto *coursePage = new QWidget();
    auto *courseLay = new QVBoxLayout(coursePage);
    auto *btnLay = new QHBoxLayout();
    auto *btnAdd  = new QPushButton("+ 添加课程");
    auto *btnEdit = new QPushButton("编辑选中");
    auto *btnDel  = new QPushButton("删除选中");
    auto *btnSave = new QPushButton("💾 保存成绩修改");
    btnSave->setStyleSheet("QPushButton { background:#27ae60; color:white; font-weight:bold; "
                           "padding:6px 16px; border-radius:4px; }");
    btnLay->addWidget(btnAdd); btnLay->addWidget(btnEdit);
    btnLay->addWidget(btnDel); btnLay->addStretch(); btnLay->addWidget(btnSave);
    courseLay->addLayout(btnLay);

    m_courseTable = new QTableWidget(0, 5);
    m_courseTable->setHorizontalHeaderLabels({"课程名称", "学分", "成绩", "绩点", "学期"});
    m_courseTable->horizontalHeader()->setStretchLastSection(true);
    m_courseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_courseTable->setStyleSheet("QTableWidget { font-size:13px; } QHeaderView::section { font-size:13px; font-weight:bold; }");
    m_courseTable->setColumnWidth(0, 200);
    m_courseTable->setColumnWidth(1, 80);
    m_courseTable->setColumnWidth(2, 80);
    m_courseTable->setColumnWidth(3, 80);
    courseLay->addWidget(m_courseTable);
    m_tabWidget->addTab(coursePage, "📖 课程成绩");

    // --- Tab2: 批量导入 ---
    m_importPage = new QWidget();
    auto *importLay = new QVBoxLayout(m_importPage);
    auto *importHint = new QLabel("导入 CSV 或 JSON 文件，为当前选中学生添加课程成绩");
    importHint->setStyleSheet("color:#7f8c8d; margin-bottom:8px;");
    importLay->addWidget(importHint);
    m_btnSelectFile = new QPushButton("选择文件导入...");
    m_btnSelectFile->setMinimumHeight(40);
    m_btnSelectFile->setStyleSheet("QPushButton { background:#3498db; color:white; font-weight:bold; "
                                   "font-size:14px; padding:10px 32px; border-radius:6px; }");
    importLay->addWidget(m_btnSelectFile);
    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet("font-size:13px; margin-top:8px;");
    importLay->addWidget(m_statusLabel);
    importLay->addStretch();
    m_tabWidget->addTab(m_importPage, "📥 批量导入");

    mainLay->addWidget(m_tabWidget);

    // 连接
    connect(m_studentCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdminPanel::onStudentSelected);
    connect(btnAdd,  &QPushButton::clicked, this, &AdminPanel::onAddCourse);
    connect(btnEdit, &QPushButton::clicked, this, &AdminPanel::onEditCourse);
    connect(btnDel,  &QPushButton::clicked, this, &AdminPanel::onDeleteCourse);
    connect(btnSave, &QPushButton::clicked, this, &AdminPanel::saveCurrentCourses);
    connect(m_btnSelectFile, &QPushButton::clicked, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "选择导入文件", "",
            "数据文件 (*.json *.csv)");
        if (path.isEmpty()) return;
        m_statusLabel->setText("正在导入...");

        StudentProfile profile = m_db->loadData(m_currentStudent);
        bool ok = false;

        if (path.endsWith(".json")) {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
                QJsonArray arr;
                if (doc.isArray()) arr = doc.array();
                else if (doc.isObject() && doc.object().contains("courses"))
                    arr = doc.object()["courses"].toArray();
                for (const auto &v : arr) {
                    QJsonObject o = v.toObject();
                    double sc = o["score"].toDouble();
                    if (sc < 0 || sc > 100) {
                        m_statusLabel->setText("❌ 成绩不合法");
                        return;
                    }
                    profile.addCourse(CourseRecord::fromJson(o));
                }
                ok = true;
            }
        } else if (path.endsWith(".csv")) {
            QFile f(path);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&f);
                while (!in.atEnd()) {
                    QString line = in.readLine().trimmed();
                    if (line.isEmpty() || line.startsWith("#")) continue;
                    QStringList cols = line.split(",");
                    if (cols.size() < 4) continue;
                    CourseRecord c;
                    c.setName(cols[0].trimmed());
                    c.setCredit(cols[1].trimmed().toDouble());
                    c.setScore(cols[2].trimmed().toDouble());
                    c.setSemester(cols[3].trimmed());
                    if (c.score() >= 0 && c.score() <= 100)
                        profile.addCourse(c);
                }
                ok = true;
            }
        }

        if (ok) {
            m_db->saveData(m_currentStudent, profile);
            loadStudentData(m_currentStudent);
            m_statusLabel->setText("✅ 导入成功！");
            emit dataChanged();
        } else {
            m_statusLabel->setText("❌ 导入失败，请检查文件格式");
        }
    });

    // 初始加载
    loadAllStudents();
}

void AdminPanel::loadAllStudents()
{
    m_studentCombo->clear();
    m_studentCombo->addItem("-- 请选择学生 --");
    QStringList students = {"喜羊羊","美羊羊","懒羊羊","沸羊羊","暖羊羊","慢羊羊","灰太狼","红太狼","小灰灰"};
    for (const auto &s : students)
        m_studentCombo->addItem(s);
}

void AdminPanel::onStudentSelected(int index)
{
    if (index <= 0) return; // "-- 请选择学生 --"
    m_currentStudent = m_studentCombo->currentText();
    loadStudentData(m_currentStudent);
}

void AdminPanel::loadStudentData(const QString &username)
{
    m_currentProfile = m_db->loadData(username);
    m_lblGPA->setText(QString("GPA：%1  |  加权平均分：%2  |  总学分：%3")
        .arg(GpaCalculator::calculate(m_currentProfile.courses()), 0, 'f', 2)
        .arg(GpaCalculator::weightedAverage(m_currentProfile.courses()), 0, 'f', 1)
        .arg(GpaCalculator::totalCredits(m_currentProfile.courses()), 0, 'f', 1));

    m_courseTable->setRowCount(0);
    for (const auto &c : m_currentProfile.courses()) {
        int r = m_courseTable->rowCount();
        m_courseTable->setRowCount(r+1);
        m_courseTable->setItem(r, 0, new QTableWidgetItem(c.name()));
        m_courseTable->setItem(r, 1, new QTableWidgetItem(QString::number(c.credit(),'f',1)));
        m_courseTable->setItem(r, 2, new QTableWidgetItem(QString::number(c.score(),'f',1)));
        m_courseTable->setItem(r, 3, new QTableWidgetItem(QString::number(c.getGradePoint(),'f',2)));
        m_courseTable->setItem(r, 4, new QTableWidgetItem(c.semester()));
    }
}

void AdminPanel::saveCurrentCourses()
{
    QList<CourseRecord> courses;
    for (int i = 0; i < m_courseTable->rowCount(); i++) {
        CourseRecord c;
        c.setName(m_courseTable->item(i, 0) ? m_courseTable->item(i, 0)->text() : "");
        c.setCredit(m_courseTable->item(i, 1) ? m_courseTable->item(i, 1)->text().toDouble() : 0);
        c.setScore(m_courseTable->item(i, 2) ? m_courseTable->item(i, 2)->text().toDouble() : 0);
        c.setSemester(m_courseTable->item(i, 4) ? m_courseTable->item(i, 4)->text() : "");
        if (!c.name().isEmpty()) courses.append(c);
    }
    m_currentProfile.setCourses(courses);
    m_db->saveData(m_currentStudent, m_currentProfile);
    loadStudentData(m_currentStudent);
    emit dataChanged();
    QMessageBox::information(this, "成功", m_currentStudent + " 的课程成绩已保存！");
}

void AdminPanel::onAddCourse()
{
    QDialog dlg(this);
    dlg.setWindowTitle("添加课程");
    auto *f = new QFormLayout(&dlg);
    auto *e1 = new QLineEdit(&dlg); e1->setPlaceholderText("课程名称");
    auto *e2 = new QDoubleSpinBox(&dlg); e2->setRange(0, 20); e2->setDecimals(1);
    auto *e3 = new QDoubleSpinBox(&dlg); e3->setRange(0, 100); e3->setDecimals(1);
    auto *e4 = new QLineEdit(&dlg); e4->setPlaceholderText("如 第一学期");
    f->addRow("名称：", e1); f->addRow("学分：", e2);
    f->addRow("成绩：", e3); f->addRow("学期：", e4);
    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    f->addRow(bb);
    connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() == QDialog::Accepted && !e1->text().trimmed().isEmpty()) {
        int r = m_courseTable->rowCount();
        m_courseTable->setRowCount(r+1);
        m_courseTable->setItem(r, 0, new QTableWidgetItem(e1->text().trimmed()));
        m_courseTable->setItem(r, 1, new QTableWidgetItem(QString::number(e2->value(),'f',1)));
        m_courseTable->setItem(r, 2, new QTableWidgetItem(QString::number(e3->value(),'f',1)));
        double gp = 0;
        double sc = e3->value();
        if (sc >= 90) gp = 4.0; else if (sc >= 85) gp = 3.7; else if (sc >= 82) gp = 3.3;
        else if (sc >= 78) gp = 3.0; else if (sc >= 75) gp = 2.7; else if (sc >= 72) gp = 2.3;
        else if (sc >= 68) gp = 2.0; else if (sc >= 64) gp = 1.5; else if (sc >= 60) gp = 1.0;
        m_courseTable->setItem(r, 3, new QTableWidgetItem(QString::number(gp,'f',2)));
        m_courseTable->setItem(r, 4, new QTableWidgetItem(e4->text().trimmed()));
    }
}

void AdminPanel::onEditCourse()
{
    int r = m_courseTable->currentRow();
    if (r < 0) { QMessageBox::information(this, "提示", "请先选中一行"); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("编辑课程");
    auto *f = new QFormLayout(&dlg);
    auto *e1 = new QLineEdit(&dlg); e1->setText(m_courseTable->item(r, 0)->text());
    auto *e2 = new QDoubleSpinBox(&dlg); e2->setRange(0, 20); e2->setDecimals(1);
    e2->setValue(m_courseTable->item(r, 1)->text().toDouble());
    auto *e3 = new QDoubleSpinBox(&dlg); e3->setRange(0, 100); e3->setDecimals(1);
    e3->setValue(m_courseTable->item(r, 2)->text().toDouble());
    auto *e4 = new QLineEdit(&dlg); e4->setText(m_courseTable->item(r, 4)->text());
    f->addRow("名称：", e1); f->addRow("学分：", e2);
    f->addRow("成绩：", e3); f->addRow("学期：", e4);
    auto *bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    f->addRow(bb);
    connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() == QDialog::Accepted) {
        m_courseTable->item(r, 0)->setText(e1->text().trimmed());
        m_courseTable->item(r, 1)->setText(QString::number(e2->value(),'f',1));
        m_courseTable->item(r, 2)->setText(QString::number(e3->value(),'f',1));
        double gp = 0; double sc = e3->value();
        if (sc >= 90) gp = 4.0; else if (sc >= 85) gp = 3.7; else if (sc >= 82) gp = 3.3;
        else if (sc >= 78) gp = 3.0; else if (sc >= 75) gp = 2.7; else if (sc >= 72) gp = 2.3;
        else if (sc >= 68) gp = 2.0; else if (sc >= 64) gp = 1.5; else if (sc >= 60) gp = 1.0;
        m_courseTable->item(r, 3)->setText(QString::number(gp,'f',2));
        m_courseTable->item(r, 4)->setText(e4->text().trimmed());
    }
}

void AdminPanel::onDeleteCourse()
{
    int r = m_courseTable->currentRow();
    if (r < 0) { QMessageBox::information(this, "提示", "请先选中一行"); return; }
    m_courseTable->removeRow(r);
}
