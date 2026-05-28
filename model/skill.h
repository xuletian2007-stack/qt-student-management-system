#ifndef SKILL_H
#define SKILL_H

#include <QString>
#include <QJsonObject>

// 专业技能（Model层 - 数据实体）
struct Skill {
    QString name;        // 技能名称
    QString level;       // 熟练程度：精通/熟练/了解

    QJsonObject toJson() const;
    static Skill fromJson(const QJsonObject &obj);
};

#endif // SKILL_H
