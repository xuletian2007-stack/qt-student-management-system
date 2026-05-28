#ifndef ACHIEVEMENTCONTROLLER_H
#define ACHIEVEMENTCONTROLLER_H

#include <QObject>
#include "../model/studentprofile.h"
#include "../model/achievement.h"
#include "../model/databasemanager.h"

// 成就控制器（Controller层）
class AchievementController : public QObject
{
    Q_OBJECT
public:
    explicit AchievementController(QObject *parent = nullptr);

    void setModel(StudentProfile *model) { m_model = model; }

public slots:
    void onAddAchievement(const Achievement &ach);
    void onUpdateAchievement(int index, const Achievement &ach);
    void onRemoveAchievement(int index);

signals:
    void achievementsChanged();

private:
    StudentProfile *m_model = nullptr;
    DatabaseManager *m_db;
};

#endif // ACHIEVEMENTCONTROLLER_H
