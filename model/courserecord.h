#ifndef COURSERECORD_H
#define COURSERECORD_H

#include <QString>
#include <QJsonObject>

// 课程记录（Model层 - 数据实体）
class CourseRecord
{
public:
    CourseRecord();
    CourseRecord(const QString &name, double credit, double score, const QString &semester);

    // ----- 访问器 -----
    QString name()     const { return m_name; }
    double  credit()   const { return m_credit; }
    double  score()    const { return m_score; }
    QString semester() const { return m_semester; }

    void setName(const QString &v)     { m_name = v; }
    void setCredit(double v)           { m_credit = v; }
    void setScore(double v)            { m_score = v; }
    void setSemester(const QString &v) { m_semester = v; }

    // 百分制 → 4.0 绩点
    // 90-100→4.0  85-89→3.7  82-84→3.3  78-81→3.0
    // 75-77→2.7   72-74→2.3  68-71→2.0  64-67→1.5  60-63→1.0  <60→0
    double getGradePoint() const;

    // JSON 序列化
    QJsonObject toJson() const;
    static CourseRecord fromJson(const QJsonObject &obj);

private:
    QString m_name;
    double  m_credit;
    double  m_score;
    QString m_semester;   // e.g. "2024-2025-1"
};

#endif // COURSERECORD_H
