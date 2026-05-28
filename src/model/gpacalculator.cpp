#include "gpacalculator.h"

double GpaCalculator::calculate(const QList<CourseRecord> &courses)
{
    double totalPoints = 0.0;
    double totalCred   = 0.0;
    for (const auto &c : courses) {
        totalPoints += c.getGradePoint() * c.credit();
        totalCred   += c.credit();
    }
    return (totalCred > 0) ? (totalPoints / totalCred) : 0.0;
}

QMap<QString, double> GpaCalculator::getSemesterTrend(const QList<CourseRecord> &courses)
{
    QMap<QString, double> semesterPoints;  // 学期 → Σ(绩点×学分)
    QMap<QString, double> semesterCredits;

    for (const auto &c : courses) {
        semesterPoints[c.semester()]  += c.getGradePoint() * c.credit();
        semesterCredits[c.semester()] += c.credit();
    }

    QMap<QString, double> result;
    for (auto it = semesterPoints.begin(); it != semesterPoints.end(); ++it) {
        double cred = semesterCredits[it.key()];
        result[it.key()] = (cred > 0) ? (it.value() / cred) : 0.0;
    }
    return result;
}

double GpaCalculator::totalCredits(const QList<CourseRecord> &courses)
{
    double sum = 0.0;
    for (const auto &c : courses) sum += c.credit();
    return sum;
}

double GpaCalculator::weightedAverage(const QList<CourseRecord> &courses)
{
    double sum = 0.0, cred = 0.0;
    for (const auto &c : courses) {
        sum  += c.score() * c.credit();
        cred += c.credit();
    }
    return (cred > 0) ? (sum / cred) : 0.0;
}

QString GpaCalculator::gpaDescription(double gpa)
{
    // 基于百分制加权平均分判断等级
    if (gpa >= 90) return "优秀";
    if (gpa >= 80) return "良好";
    if (gpa >= 70) return "中等";
    if (gpa >= 60) return "及格";
    return "不及格";
}
