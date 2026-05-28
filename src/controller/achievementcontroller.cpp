#include "achievementcontroller.h"

AchievementController::AchievementController(QObject *parent)
    : QObject(parent)
{
    m_db = DatabaseManager::getInstance();
}

void AchievementController::onAddAchievement(const Achievement &ach)
{
    if (!m_model) return;
    m_model->addAchievement(ach);
    m_db->saveData("admin", *m_model);
    emit achievementsChanged();
}

void AchievementController::onUpdateAchievement(int index, const Achievement &ach)
{
    if (!m_model) return;
    m_model->updateAchievement(index, ach);
    m_db->saveData("admin", *m_model);
    emit achievementsChanged();
}

void AchievementController::onRemoveAchievement(int index)
{
    if (!m_model) return;
    m_model->removeAchievement(index);
    m_db->saveData("admin", *m_model);
    emit achievementsChanged();
}
