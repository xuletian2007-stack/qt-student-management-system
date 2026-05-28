#include "importcontroller.h"
#include "../model/courserecord.h"
#include "../model/achievement.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>
#include <QFileInfo>

ImportController::ImportController(QObject *parent)
    : QObject(parent)
{
    m_db = DatabaseManager::getInstance();
}

bool ImportController::validateFile(const QString &path)
{
    QFileInfo fi(path);
    if (!fi.exists()) return false;
    QString ext = fi.suffix().toLower();
    return (ext == "json" || ext == "csv" || ext == "pdp");
}

void ImportController::onImportRequested(const QString &path)
{
    if (!validateFile(path)) {
        emit importFailed("文件不存在或格式不支持");
        return;
    }

    QString suffix = QFileInfo(path).suffix().toLower();
    StudentProfile profile = m_db->loadData("admin");

    if (suffix == "pdp") {
        // .pdp 是本系统的备份格式
        QString err;
        if (m_db->importBackup(path, err))
            emit importSuccess();
        else
            emit importFailed(err);
        return;
    }

    if (suffix == "json") {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            emit importFailed("无法打开文件");
            return;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonArray arr;
        if (doc.isArray())
            arr = doc.array();
        else if (doc.isObject() && doc.object().contains("courses"))
            arr = doc.object()["courses"].toArray();
        else {
            emit importFailed("JSON 格式错误");
            return;
        }

        for (const auto &v : arr) {
            QJsonObject obj = v.toObject();
            double score = obj["score"].toDouble();
            if (score < 0 || score > 100) {
                emit importFailed("成绩数据不合法（需在0-100之间）");
                return;
            }
            profile.addCourse(CourseRecord::fromJson(obj));
        }
    } else if (suffix == "csv") {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            emit importFailed("无法打开文件");
            return;
        }
        QTextStream in(&file);
        int lineNum = 0;
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            lineNum++;
            if (line.isEmpty() || line.startsWith('#')) continue;
            QStringList fields = line.split(',');
            if (fields.size() < 5) {
                emit importFailed(QString("第 %1 行格式错误").arg(lineNum));
                return;
            }
            double score = fields[3].trimmed().toDouble();
            if (score < 0 || score > 100) {
                emit importFailed(QString("第 %1 行成绩不合法").arg(lineNum));
                return;
            }
            profile.addCourse(CourseRecord(
                fields[1].trimmed(),           // name
                fields[2].trimmed().toDouble(), // credit
                score,                          // score
                fields[4].trimmed()             // semester
            ));
        }
        file.close();
    }

    if (!m_db->saveData("admin", profile)) {
        emit importFailed("保存数据失败");
        return;
    }

    emit importSuccess();
}
