#ifndef STUDENTPROFILE_H
#define STUDENTPROFILE_H

#include "courserecord.h"
#include "achievement.h"
#include "education.h"
#include "skill.h"
#include "experience.h"
#include <QList>
#include <QJsonObject>

class StudentProfile
{
public:
    StudentProfile();
    explicit StudentProfile(const QString &name, const QString &studentId);

    // ===== 基本信息 =====
    QString name()        const { return m_name; }
    QString studentId()   const { return m_studentId; }
    QString gender()      const { return m_gender; }
    QString political()   const { return m_political; }
    QString birthday()    const { return m_birthday; }
    QString email()       const { return m_email; }
    QString phone()       const { return m_phone; }
    QString personality() const { return m_personality; }
    QString hobby()       const { return m_hobby; }
    QString photoPath()   const { return m_photoPath; }
    QString photoBase64() const { return m_photoBase64; }

    void setName(const QString &v)       { m_name = v; }
    void setStudentId(const QString &v)  { m_studentId = v; }
    void setGender(const QString &v)     { m_gender = v; }
    void setPolitical(const QString &v)  { m_political = v; }
    void setBirthday(const QString &v)   { m_birthday = v; }
    void setEmail(const QString &v)      { m_email = v; }
    void setPhone(const QString &v)      { m_phone = v; }
    void setPersonality(const QString &v){ m_personality = v; }
    void setHobby(const QString &v)      { m_hobby = v; }
    void setPhotoPath(const QString &v)  { m_photoPath = v; }
    void setPhotoBase64(const QString &v){ m_photoBase64 = v; }

    // ===== 教育背景 (0..*) =====
    const QList<Education>& educations() const { return m_educations; }
    void addEducation(const Education &e)       { m_educations.append(e); }
    void updateEducation(int i, const Education &e);
    void removeEducation(int i);
    void setEducations(const QList<Education> &es) { m_educations = es; }

    // ===== 课程记录 (1..*) =====
    const QList<CourseRecord>& courses() const { return m_courses; }
    void addCourse(const CourseRecord &course);
    void updateCourse(int i, const CourseRecord &course);
    void removeCourse(int i);
    void setCourses(const QList<CourseRecord> &cs) { m_courses = cs; }

    // ===== 专业技能 (0..*) =====
    const QList<Skill>& skills() const { return m_skills; }
    void addSkill(const Skill &s);
    void updateSkill(int i, const Skill &s);
    void removeSkill(int i);
    void setSkills(const QList<Skill> &s) { m_skills = s; }

    // ===== 项目与实习经历 (0..*) =====
    const QList<Experience>& experiences() const { return m_experiences; }
    void addExperience(const Experience &e);
    void updateExperience(int i, const Experience &e);
    void removeExperience(int i);
    void setExperiences(const QList<Experience> &e) { m_experiences = e; }

    // ===== 荣誉与奖项 (0..*) =====
    const QList<Achievement>& achievements() const { return m_achievements; }
    void addAchievement(const Achievement &ach);
    void updateAchievement(int i, const Achievement &ach);
    void removeAchievement(int i);
    void setAchievements(const QList<Achievement> &as) { m_achievements = as; }

    double calculateTotalGPA() const;
    QJsonObject toJson() const;
    static StudentProfile fromJson(const QJsonObject &obj);

private:
    QString m_name, m_studentId, m_gender, m_political, m_birthday;
    QString m_email, m_phone, m_personality, m_hobby, m_photoPath, m_photoBase64;
    QList<Education>    m_educations;
    QList<CourseRecord> m_courses;
    QList<Skill>        m_skills;
    QList<Experience>   m_experiences;
    QList<Achievement>  m_achievements;
};

#endif // STUDENTPROFILE_H
