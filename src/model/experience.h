#ifndef EXPERIENCE_H
#define EXPERIENCE_H

#include <QString>
#include <QJsonObject>

// 项目与实习经历（Model层 - 数据实体）
struct Experience {
    QString title;       // 项目/实习名称
    QString role;        // 角色/职位
    QString startDate;   // 开始时间 yyyy-MM
    QString endDate;     // 结束时间 yyyy-MM
    QString description; // 具体描述

    QJsonObject toJson() const;
    static Experience fromJson(const QJsonObject &obj);
};

#endif // EXPERIENCE_H
