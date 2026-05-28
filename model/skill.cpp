#include "skill.h"

QJsonObject Skill::toJson() const
{
    QJsonObject obj;
    obj["name"]  = name;
    obj["level"] = level;
    return obj;
}

Skill Skill::fromJson(const QJsonObject &obj)
{
    Skill s;
    s.name  = obj["name"].toString();
    s.level = obj["level"].toString();
    return s;
}
