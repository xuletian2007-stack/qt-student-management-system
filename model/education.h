#ifndef EDUCATION_H
#define EDUCATION_H

#include <QString>
#include <QJsonObject>

// 教育背景（Model层 - 数据实体，支持多段）
struct Education {
    QString schoolName;   // 学校名称
    QString major;        // 专业
    QString degree;       // 学位
    QString startYear;    // 入学年份
    QString endYear;      // 毕业年份

    QJsonObject toJson() const;
    static Education fromJson(const QJsonObject &obj);
};

#endif // EDUCATION_H
