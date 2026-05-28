#include "resumegenerator.h"
#include "gpacalculator.h"
#include <QFile>
#include <QTextStream>
#include <QPrinter>
#include <QTextDocument>
#include <QStandardPaths>
#include <QDateTime>

ResumeGenerator::ResumeGenerator()
{
    m_outputPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
}

QString ResumeGenerator::fillTemplate(const StudentProfile &profile)
{
    double gpa  = GpaCalculator::calculate(profile.courses());
    double avg  = GpaCalculator::weightedAverage(profile.courses());
    double cred = GpaCalculator::totalCredits(profile.courses());
    auto edus = profile.educations();

    QString html;
    // QTextBrowser 兼容的简单 HTML：只用 table/tr/td/b/p/hr，不用 CSS3 高级特性
    html += "<html><head><meta charset=\"UTF-8\"><title>个人简历</title></head>\n";
    html += "<body style=\"font-family:Arial,SimHei,sans-serif; margin:20px;\">\n";

    // 标题
    html += "<h1 align=\"center\">" + profile.name().toHtmlEscaped() + "</h1>\n";

    // 联系方式行
    html += "<p align=\"center\"><font color=\"gray\" size=\"3\">";
    QStringList contact;
    if (!profile.studentId().isEmpty()) contact << "学号：" + profile.studentId();
    if (!profile.gender().isEmpty())    contact << profile.gender();
    if (!profile.political().isEmpty()) contact << profile.political();
    if (!profile.birthday().isEmpty())  contact << profile.birthday();
    if (!profile.phone().isEmpty())     contact << "电话：" + profile.phone();
    if (!profile.email().isEmpty())     contact << "邮箱：" + profile.email();
    html += contact.join(" &nbsp;|&nbsp; ");
    html += "</font></p>\n";

    // 性格爱好
    if (!profile.personality().isEmpty() || !profile.hobby().isEmpty()) {
        html += "<p><b>个人特质</b></p>\n";
        if (!profile.personality().isEmpty())
            html += "<p>性格：" + profile.personality().toHtmlEscaped() + "</p>\n";
        if (!profile.hobby().isEmpty())
            html += "<p>爱好：" + profile.hobby().toHtmlEscaped() + "</p>\n";
    }
    html += "<hr>\n";

    // ===== 教育背景（多段）=====
    html += "<h2>🎓 教育背景</h2>\n";
    if (!edus.isEmpty()) {
        for (const auto &edu : edus) {
            html += "<p><b>";
            if (!edu.schoolName.isEmpty()) html += edu.schoolName.toHtmlEscaped();
            if (!edu.major.isEmpty())      html += " · " + edu.major.toHtmlEscaped();
            if (!edu.degree.isEmpty())     html += " · " + edu.degree;
            html += "</b></p>\n";
            if (!edu.startYear.isEmpty())
                html += "<p><font color=\"gray\">" + edu.startYear + " — " + edu.endYear + "</font></p>\n";
        }
    }

    // 学业成绩
    html += "<p><b>学业成绩</b></p>\n";
    html += "<table border=\"0\" cellpadding=\"4\" cellspacing=\"0\" width=\"100%\">\n";
    html += "<tr><td width=\"25%\"><b>GPA</b><br><font size=\"5\" color=\"#3498db\">"
         + QString::number(gpa,'f',2) + "</font></td>";
    html += "<td width=\"25%\"><b>加权平均分</b><br><font size=\"5\" color=\"#3498db\">"
         + QString::number(avg,'f',1) + "</font></td>";
    html += "<td width=\"25%\"><b>总学分</b><br><font size=\"5\" color=\"#3498db\">"
         + QString::number(cred,'f',1) + "</font></td>";
    html += "<td width=\"25%\"><b>等级</b><br><font size=\"5\" color=\"#3498db\">"
         + GpaCalculator::gpaDescription(avg) + "</font></td></tr>\n";
    html += "</table>\n";

    // 课程成绩表
    if (!profile.courses().isEmpty()) {
        html += "<p><b>课程成绩</b></p>\n";
        html += "<table border=\"1\" cellpadding=\"6\" cellspacing=\"0\" width=\"100%\" "
                "bordercolor=\"#ddd\">\n";
        html += "<tr bgcolor=\"#3498db\"><td><b><font color=\"white\">课程</font></b></td>"
                "<td><b><font color=\"white\">学分</font></b></td>"
                "<td><b><font color=\"white\">成绩</font></b></td>"
                "<td><b><font color=\"white\">绩点</font></b></td>"
                "<td><b><font color=\"white\">学期</font></b></td></tr>\n";
        for (const auto &c : profile.courses()) {
            html += "<tr><td>" + c.name().toHtmlEscaped() + "</td>"
                    "<td>" + QString::number(c.credit(),'f',1) + "</td>"
                    "<td>" + QString::number(c.score(),'f',1) + "</td>"
                    "<td>" + QString::number(c.getGradePoint(),'f',2) + "</td>"
                    "<td>" + c.semester() + "</td></tr>\n";
        }
        html += "</table>\n";
        html += "<br>\n";
    }
    html += "<hr>\n";

    // ===== 专业技能 =====
    if (!profile.skills().isEmpty()) {
        html += "<h2>💻 专业技能</h2>\n";
        html += "<p>";
        for (const auto &s : profile.skills()) {
            html += "<b>" + s.name.toHtmlEscaped() + "</b>（" + s.level + "）&nbsp;&nbsp;";
        }
        html += "</p>\n<hr>\n";
    }

    // ===== 项目与实习经历 =====
    if (!profile.experiences().isEmpty()) {
        html += "<h2>📋 项目与实习经历</h2>\n";
        for (const auto &e : profile.experiences()) {
            html += "<p><b>" + e.title.toHtmlEscaped() + "</b>";
            if (!e.role.isEmpty()) html += " — " + e.role.toHtmlEscaped();
            html += "</p>\n";
            html += "<p><font color=\"gray\" size=\"2\">" + e.startDate + " — " + e.endDate + "</font></p>\n";
            if (!e.description.isEmpty())
                html += "<p>" + e.description.toHtmlEscaped() + "</p>\n";
            html += "<br>\n";
        }
        html += "<hr>\n";
    }

    // ===== 荣誉与奖项 =====
    if (!profile.achievements().isEmpty()) {
        html += "<h2>🏆 荣誉与奖项</h2>\n";
        html += "<table border=\"1\" cellpadding=\"6\" cellspacing=\"0\" width=\"100%\" "
                "bordercolor=\"#ddd\">\n";
        html += "<tr bgcolor=\"#3498db\"><td><b><font color=\"white\">奖项</font></b></td>"
                "<td><b><font color=\"white\">级别</font></b></td>"
                "<td><b><font color=\"white\">时间</font></b></td>"
                "<td><b><font color=\"white\">描述</font></b></td></tr>\n";
        for (const auto &a : profile.achievements()) {
            html += "<tr><td>" + a.title().toHtmlEscaped() + "</td>"
                    "<td>" + a.type() + "</td>"
                    "<td>" + a.date().toString("yyyy-MM-dd") + "</td>"
                    "<td>" + a.description().toHtmlEscaped() + "</td></tr>\n";
        }
        html += "</table>\n";
    }

    html += "</body></html>";
    return html;
}

bool ResumeGenerator::exportToPdf(const QString &html)
{
    QString path = m_outputPath + "/个人简历_"
                 + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".pdf";
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageSize(QPageSize(QPageSize::A4));
    doc.print(&printer);
    return true;
}
