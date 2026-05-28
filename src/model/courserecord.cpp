#include "courserecord.h"

CourseRecord::CourseRecord()
    : m_credit(0), m_score(0)
{
}

CourseRecord::CourseRecord(const QString &name, double credit,
                           double score, const QString &semester)
    : m_name(name), m_credit(credit),
      m_score(score), m_semester(semester)
{
}

double CourseRecord::getGradePoint() const
{
    if (m_score >= 90) return 4.0;
    if (m_score >= 85) return 3.7;
    if (m_score >= 82) return 3.3;
    if (m_score >= 78) return 3.0;
    if (m_score >= 75) return 2.7;
    if (m_score >= 72) return 2.3;
    if (m_score >= 68) return 2.0;
    if (m_score >= 64) return 1.5;
    if (m_score >= 60) return 1.0;
    return 0.0;
}

QJsonObject CourseRecord::toJson() const
{
    QJsonObject obj;
    obj["name"]     = m_name;
    obj["credit"]   = m_credit;
    obj["score"]    = m_score;
    obj["semester"] = m_semester;
    return obj;
}

CourseRecord CourseRecord::fromJson(const QJsonObject &obj)
{
    CourseRecord c;
    c.m_name     = obj["name"].toString();
    c.m_credit   = obj["credit"].toDouble();
    c.m_score    = obj["score"].toDouble();
    c.m_semester = obj["semester"].toString();
    return c;
}
