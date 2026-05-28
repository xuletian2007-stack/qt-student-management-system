#ifndef GPACONTROLLER_H
#define GPACONTROLLER_H

#include <QObject>
#include "../model/studentprofile.h"

class GPAView;

// GPA 控制器（Controller层）
class GpaController : public QObject
{
    Q_OBJECT
public:
    explicit GpaController(QObject *parent = nullptr);

    void setView(GPAView *view) { m_view = view; }
    void setModel(StudentProfile *model) { m_model = model; }

public slots:
    void onViewGPARequested();

signals:
    void gpaCalculated(double gpa, const QMap<QString, double> &trend);

private:
    void calculateAndPushData();

    GPAView        *m_view   = nullptr;
    StudentProfile *m_model  = nullptr;
};

#endif // GPACONTROLLER_H
