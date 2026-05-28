#include "education.h"

QJsonObject Education::toJson() const
{
    QJsonObject obj;
    obj["schoolName"] = schoolName;
    obj["major"]      = major;
    obj["degree"]     = degree;
    obj["startYear"]  = startYear;
    obj["endYear"]    = endYear;
    return obj;
}

Education Education::fromJson(const QJsonObject &obj)
{
    Education e;
    e.schoolName = obj["schoolName"].toString();
    e.major      = obj["major"].toString();
    e.degree     = obj["degree"].toString();
    e.startYear  = obj["startYear"].toString();
    e.endYear    = obj["endYear"].toString();
    return e;
}
