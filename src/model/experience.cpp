#include "experience.h"

QJsonObject Experience::toJson() const
{
    QJsonObject obj;
    obj["title"]       = title;
    obj["role"]        = role;
    obj["startDate"]   = startDate;
    obj["endDate"]     = endDate;
    obj["description"] = description;
    return obj;
}

Experience Experience::fromJson(const QJsonObject &obj)
{
    Experience e;
    e.title       = obj["title"].toString();
    e.role        = obj["role"].toString();
    e.startDate   = obj["startDate"].toString();
    e.endDate     = obj["endDate"].toString();
    e.description = obj["description"].toString();
    return e;
}
