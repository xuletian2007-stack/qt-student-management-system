#ifndef ACHIEVEMENT_H
#define ACHIEVEMENT_H

#include <QString>
#include <QDate>
#include <QJsonObject>

// 成就/奖项（Model层 - 数据实体）
class Achievement
{
public:
    Achievement();

    // ----- 访问器 -----
    QString title()       const { return m_title; }
    QDate   date()        const { return m_date; }
    QString type()        const { return m_type; }
    QString description() const { return m_description; }
    QString proofPath()   const { return m_proofPath; }

    void setTitle(const QString &v)       { m_title = v; }
    void setDate(const QDate &v)          { m_date = v; }
    void setType(const QString &v)        { m_type = v; }
    void setDescription(const QString &v) { m_description = v; }
    void setProofPath(const QString &v)   { m_proofPath = v; }

    QString toString() const;

    // JSON 序列化
    QJsonObject toJson() const;
    static Achievement fromJson(const QJsonObject &obj);

private:
    QString m_title;
    QDate   m_date;
    QString m_type;        // 奖项 / 班级角色 / 志愿者 / 其他
    QString m_description;
    QString m_proofPath;   // 证明材料路径（可选）
};

#endif // ACHIEVEMENT_H
