#ifndef GPAVIEW_H
#define GPAVIEW_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QMap>

namespace QtCharts { class QChartView; class QChart; }

class GPAView : public QWidget
{
    Q_OBJECT
public:
    explicit GPAView(QWidget *parent = nullptr);

    void setGPAText(double val);
    void setAvg(double val);
    void setCredits(double val);
    void setDescription(const QString &desc);
    void renderGPAChart(const QMap<QString, double> &trend);
    void setCourseTable(const QList<QPair<QStringList, QMap<QString, double>>> &);
    void refreshAll(double gpa, double avg, double credits, const QString &desc,
                    const QStringList &courseNames, const QList<double> &courseScores,
                    const QList<double> &courseCredits, const QStringList &semesters,
                    const QMap<QString, double> &trend);

signals:
    void viewGPARequested();

private:
    void setupUI();

    QLabel                *m_lblGPA, *m_lblAvg, *m_lblCredits, *m_lblDesc;
    QTableWidget          *m_courseTable;
    QtCharts::QChartView  *m_chartView;
    QtCharts::QChart      *m_chart;
    bool m_loaded;
};

#endif // GPAVIEW_H
