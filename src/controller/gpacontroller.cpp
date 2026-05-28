#include "gpacontroller.h"
#include "../model/gpacalculator.h"
#include "../view/gpaview.h"

GpaController::GpaController(QObject *parent)
    : QObject(parent)
{
}

void GpaController::onViewGPARequested()
{
    calculateAndPushData();
}

void GpaController::calculateAndPushData()
{
    if (!m_model) return;

    const auto &courses = m_model->courses();
    double gpa  = GpaCalculator::calculate(courses);
    auto trend  = GpaCalculator::getSemesterTrend(courses);
    double avg  = GpaCalculator::weightedAverage(courses);
    double cred = GpaCalculator::totalCredits(courses);
    QString desc = GpaCalculator::gpaDescription(avg);

    emit gpaCalculated(gpa, trend);

    if (m_view) {
        QStringList names; QList<double> scores, creditsList; QStringList semesters;
        for (const auto &c : courses) {
            names << c.name();
            scores << c.score();
            creditsList << c.credit();
            semesters << c.semester();
        }
        m_view->refreshAll(gpa, avg, cred, desc, names, scores, creditsList, semesters, trend);
    }
}
