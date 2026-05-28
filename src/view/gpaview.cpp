#include "gpaview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>
#include <QShowEvent>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

GPAView::GPAView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void GPAView::setupUI()
{
    auto *mainLay = new QVBoxLayout(this);

    // 数据卡片行
    auto *cardLay = new QHBoxLayout();

    auto makeCard = [](const QString &title, QLabel *&label) -> QGroupBox* {
        auto *gb = new QGroupBox(title);
        auto *vl = new QVBoxLayout(gb);
        label = new QLabel("--");
        QFont f = label->font(); f.setPointSize(22); f.setBold(true);
        label->setFont(f);
        label->setAlignment(Qt::AlignCenter);
        vl->addWidget(label);
        return gb;
    };

    cardLay->addWidget(makeCard("GPA", m_lblGPA));
    cardLay->addWidget(makeCard("加权平均分", m_lblAvg));
    cardLay->addWidget(makeCard("总学分", m_lblCredits));
    cardLay->addWidget(makeCard("等级", m_lblDesc));
    mainLay->addLayout(cardLay);

    // 图表 + 表格并排
    auto *bottomLay = new QHBoxLayout();
    
    // 左侧课程表格
    auto *tableGb = new QGroupBox("课程成绩");
    auto *tableLay = new QVBoxLayout(tableGb);
    m_courseTable = new QTableWidget(0, 5);
    m_courseTable->setHorizontalHeaderLabels({"课程名称", "学分", "成绩", "绩点", "学期"});
    m_courseTable->horizontalHeader()->setStretchLastSection(true);
    m_courseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_courseTable->setStyleSheet("QTableWidget { font-size:13px; } QHeaderView::section { font-size:13px; font-weight:bold; }");
    m_courseTable->setColumnWidth(0, 130);
    m_courseTable->setColumnWidth(1, 45);
    m_courseTable->setColumnWidth(2, 45);
    m_courseTable->setColumnWidth(3, 45);
    m_courseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_courseTable->setMinimumWidth(320);
    tableLay->addWidget(m_courseTable);
    bottomLay->addWidget(tableGb);

    // 右侧折线图
    m_chart = new QChart();
    m_chart->setTitle("GPA 趋势");
    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumWidth(350);
    bottomLay->addWidget(m_chartView);
    
    mainLay->addLayout(bottomLay);
}

void GPAView::setGPAText(double val)
{
    m_lblGPA->setText(QString::number(val, 'f', 2));
}

void GPAView::setAvg(double val)
{
    m_lblAvg->setText(QString::number(val, 'f', 1));
}

void GPAView::setCredits(double val)
{
    m_lblCredits->setText(QString::number(val, 'f', 1));
}

void GPAView::setDescription(const QString &desc)
{
    m_lblDesc->setText(desc);
}

void GPAView::refreshAll(double gpa, double avg, double credits, const QString &desc,
                         const QStringList &courseNames, const QList<double> &courseScores,
                         const QList<double> &courseCredits, const QStringList &semesters,
                         const QMap<QString, double> &trend)
{
    setGPAText(gpa);
    setAvg(avg);
    setCredits(credits);
    setDescription(desc);

    // 填充课程表格
    m_courseTable->setRowCount(0);
    for (int i = 0; i < courseNames.size(); i++) {
        int r = m_courseTable->rowCount();
        m_courseTable->setRowCount(r + 1);
        m_courseTable->setItem(r, 0, new QTableWidgetItem(courseNames[i]));
        m_courseTable->setItem(r, 1, new QTableWidgetItem(QString::number(courseCredits[i], 'f', 1)));
        m_courseTable->setItem(r, 2, new QTableWidgetItem(QString::number(courseScores[i], 'f', 1)));

        // 计算绩点
        double sc = courseScores[i];
        double gp = 0;
        if (sc >= 90) gp = 4.0; else if (sc >= 85) gp = 3.7;
        else if (sc >= 82) gp = 3.3; else if (sc >= 78) gp = 3.0;
        else if (sc >= 75) gp = 2.7; else if (sc >= 72) gp = 2.3;
        else if (sc >= 68) gp = 2.0; else if (sc >= 64) gp = 1.5;
        else if (sc >= 60) gp = 1.0;
        m_courseTable->setItem(r, 3, new QTableWidgetItem(QString::number(gp, 'f', 2)));
        m_courseTable->setItem(r, 4, new QTableWidgetItem(semesters.value(i, "")));
    }

    renderGPAChart(trend);
}

void GPAView::renderGPAChart(const QMap<QString, double> &trend)
{
    m_chart->removeAllSeries();
    const auto axes = m_chart->axes();
    for (auto *ax : axes) m_chart->removeAxis(ax);

    if (trend.isEmpty()) return;

    auto *series = new QLineSeries();
    series->setName("GPA");

    QStringList sortedKeys = trend.keys();
    sortedKeys.sort();

    double maxGpa = 0;
    int idx = 1;
    for (const auto &key : sortedKeys) {
        double val = trend[key];
        series->append(idx, val);
        if (val > maxGpa) maxGpa = val;
        idx++;
    }

    m_chart->addSeries(series);

    auto *axisX = new QValueAxis();
    axisX->setTitleText("学期");
    axisX->setRange(0.5, sortedKeys.size() + 0.5);
    axisX->setTickCount(sortedKeys.size() + 1);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText("GPA");
    axisY->setRange(0, qMax(4.5, maxGpa + 0.5));
    m_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}
