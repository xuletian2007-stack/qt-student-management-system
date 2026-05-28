#include "studentprofile.h"
#include "gpacalculator.h"
#include <QJsonArray>

StudentProfile::StudentProfile() {}
StudentProfile::StudentProfile(const QString &name, const QString &studentId)
    : m_name(name), m_studentId(studentId) {}

// ===== education list ops =====
void StudentProfile::updateEducation(int i, const Education &e) {
    if (i >= 0 && i < m_educations.size()) m_educations[i] = e;
}
void StudentProfile::removeEducation(int i) {
    if (i >= 0 && i < m_educations.size()) m_educations.removeAt(i);
}

// ===== course list ops =====
void StudentProfile::addCourse(const CourseRecord &c) { m_courses.append(c); }
void StudentProfile::updateCourse(int i, const CourseRecord &c) {
    if (i >= 0 && i < m_courses.size()) m_courses[i] = c;
}
void StudentProfile::removeCourse(int i) {
    if (i >= 0 && i < m_courses.size()) m_courses.removeAt(i);
}

// ===== skill list ops =====
void StudentProfile::addSkill(const Skill &s) { m_skills.append(s); }
void StudentProfile::updateSkill(int i, const Skill &s) {
    if (i >= 0 && i < m_skills.size()) m_skills[i] = s;
}
void StudentProfile::removeSkill(int i) {
    if (i >= 0 && i < m_skills.size()) m_skills.removeAt(i);
}

// ===== experience list ops =====
void StudentProfile::addExperience(const Experience &e) { m_experiences.append(e); }
void StudentProfile::updateExperience(int i, const Experience &e) {
    if (i >= 0 && i < m_experiences.size()) m_experiences[i] = e;
}
void StudentProfile::removeExperience(int i) {
    if (i >= 0 && i < m_experiences.size()) m_experiences.removeAt(i);
}

// ===== achievement list ops =====
void StudentProfile::addAchievement(const Achievement &a) { m_achievements.append(a); }
void StudentProfile::updateAchievement(int i, const Achievement &a) {
    if (i >= 0 && i < m_achievements.size()) m_achievements[i] = a;
}
void StudentProfile::removeAchievement(int i) {
    if (i >= 0 && i < m_achievements.size()) m_achievements.removeAt(i);
}

double StudentProfile::calculateTotalGPA() const {
    return GpaCalculator::calculate(m_courses);
}

// ===== JSON =====
static QJsonArray arrFromEdu(const QList<Education> &list) {
    QJsonArray a; for (const auto &x : list) a.append(x.toJson()); return a;
}
static QJsonArray arrFromCr(const QList<CourseRecord> &list) {
    QJsonArray a; for (const auto &x : list) a.append(x.toJson()); return a;
}
static QJsonArray arrFromSk(const QList<Skill> &list) {
    QJsonArray a; for (const auto &x : list) a.append(x.toJson()); return a;
}
static QJsonArray arrFromEx(const QList<Experience> &list) {
    QJsonArray a; for (const auto &x : list) a.append(x.toJson()); return a;
}
static QJsonArray arrFromAc(const QList<Achievement> &list) {
    QJsonArray a; for (const auto &x : list) a.append(x.toJson()); return a;
}

QJsonObject StudentProfile::toJson() const
{
    QJsonObject obj;
    obj["name"]        = m_name;
    obj["studentId"]   = m_studentId;
    obj["gender"]      = m_gender;
    obj["political"]   = m_political;
    obj["birthday"]    = m_birthday;
    obj["email"]       = m_email;
    obj["phone"]       = m_phone;
    obj["personality"] = m_personality;
    obj["hobby"]       = m_hobby;
    obj["photoPath"]   = m_photoPath;
    obj["photoBase64"] = m_photoBase64;
    obj["educations"]   = arrFromEdu(m_educations);
    obj["courses"]      = arrFromCr(m_courses);
    obj["skills"]        = arrFromSk(m_skills);
    obj["experiences"]   = arrFromEx(m_experiences);
    obj["achievements"]  = arrFromAc(m_achievements);
    return obj;
}

template<typename T>
static QList<T> parseList(const QJsonArray &arr, T (*fn)(const QJsonObject&)) {
    QList<T> list;
    for (const auto &v : arr) list.append(fn(v.toObject()));
    return list;
}

StudentProfile StudentProfile::fromJson(const QJsonObject &obj)
{
    StudentProfile sp;
    sp.m_name        = obj["name"].toString();
    sp.m_studentId   = obj["studentId"].toString();
    sp.m_gender      = obj["gender"].toString();
    sp.m_political   = obj["political"].toString();
    sp.m_birthday    = obj["birthday"].toString();
    sp.m_email       = obj["email"].toString();
    sp.m_phone       = obj["phone"].toString();
    sp.m_personality = obj["personality"].toString();
    sp.m_hobby       = obj["hobby"].toString();
    sp.m_photoPath   = obj["photoPath"].toString();
    sp.m_photoBase64 = obj["photoBase64"].toString();
    sp.m_educations   = parseList<Education>(obj["educations"].toArray(), Education::fromJson);
    sp.m_courses      = parseList<CourseRecord>(obj["courses"].toArray(), CourseRecord::fromJson);
    sp.m_skills        = parseList<Skill>(obj["skills"].toArray(), Skill::fromJson);
    sp.m_experiences   = parseList<Experience>(obj["experiences"].toArray(), Experience::fromJson);
    sp.m_achievements  = parseList<Achievement>(obj["achievements"].toArray(), Achievement::fromJson);
    return sp;
}
