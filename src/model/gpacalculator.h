#ifndef GPACALCULATOR_H
#define GPACALCULATOR_H

#include "courserecord.h"
#include <QList>
#include <QMap>

// GPA 计算器（Model层 - 逻辑模块）
// 依赖于 CourseRecord
class GpaCalculator
{
public:
    // GPA = Σ(绩点 × 学分) / Σ学分
    static double calculate(const QList<CourseRecord> &courses);

    // 按学期统计 GPA 趋势：学期 → GPA
    static QMap<QString, double> getSemesterTrend(const QList<CourseRecord> &courses);

    // 总学分
    static double totalCredits(const QList<CourseRecord> &courses);

    // 加权平均分
    static double weightedAverage(const QList<CourseRecord> &courses);

    // 等级描述
    static QString gpaDescription(double gpa);
};

#endif // GPACALCULATOR_H
