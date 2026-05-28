#ifndef RESUMEGENERATOR_H
#define RESUMEGENERATOR_H

#include "../model/studentprofile.h"
#include <QString>

// 标准简历生成器（Model层 - 模板填充+导出）
// 依赖于 StudentProfile
class ResumeGenerator
{
public:
    ResumeGenerator();

    void setTemplatePath(const QString &path) { m_templatePath = path; }
    void setOutputPath(const QString &path)   { m_outputPath = path; }

    // 填充模板，返回 HTML 字符串
    QString fillTemplate(const StudentProfile &profile);

    // 导出 HTML 为 PDF
    bool exportToPdf(const QString &html);

private:
    QString m_templatePath;
    QString m_outputPath;
};

#endif // RESUMEGENERATOR_H
