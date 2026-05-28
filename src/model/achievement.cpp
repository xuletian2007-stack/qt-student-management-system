#include "achievement.h"

Achievement::Achievement()
{
}

QString Achievement::toString() const
{
    return QString("[%1] %2 — %3")
        .arg(m_type, m_title, m_date.toString("yyyy-MM-dd"));
}

QJsonObject Achievement::toJson() const
{
    QJsonObject obj;
    obj["title"]       = m_title;
    obj["date"]        = m_date.toString(Qt::ISODate);
    obj["type"]        = m_type;
    obj["description"] = m_description;
    obj["proofPath"]   = m_proofPath;
    return obj;
}

Achievement Achievement::fromJson(const QJsonObject &obj)
{
    Achievement a;
    a.m_title       = obj["title"].toString();
    a.m_date        = QDate::fromString(obj["date"].toString(), Qt::ISODate);
    a.m_type        = obj["type"].toString();
    a.m_description = obj["description"].toString();
    a.m_proofPath   = obj["proofPath"].toString();
    return a;
}
