#ifndef ACHIEVEMENTVIEW_H
#define ACHIEVEMENTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include "../model/achievement.h"

// 成就视图（View层）
class AchievementView : public QWidget
{
    Q_OBJECT
public:
    explicit AchievementView(QWidget *parent = nullptr);

    void refreshList(const QList<Achievement> &data);
    Achievement getFormData() const;

signals:
    void addRequested(const Achievement &ach);
    void updateRequested(int index, const Achievement &ach);
    void removeRequested(int index);

private slots:
    void onAdd();
    void onEdit();
    void onDelete();
    void onFilterChanged(int index);

private:
    void setupUI();

    QTableWidget *m_table;
    QDialog      *m_form = nullptr;
    QPushButton  *m_btnAdd, *m_btnEdit, *m_btnDelete;
    QComboBox    *m_cmbFilter;
};

#endif // ACHIEVEMENTVIEW_H
