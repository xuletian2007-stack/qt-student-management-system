#ifndef RESUMECONTROLLER_H
#define RESUMECONTROLLER_H

#include <QObject>
#include "../model/studentprofile.h"
#include "../model/resumegenerator.h"

// 简历控制器（Controller层）
class ResumeController : public QObject
{
    Q_OBJECT
public:
    explicit ResumeController(QObject *parent = nullptr);

    void setModel(StudentProfile *model) { m_model = model; }
    void setGenerator(ResumeGenerator *gen) { m_generator = gen; }

public slots:
    void onExportResumeRequested();

signals:
    void standardResumeGenerated(const QString &html);

private:
    void generateStandardResume();

    StudentProfile  *m_model     = nullptr;
    ResumeGenerator *m_generator = nullptr;
};

#endif // RESUMECONTROLLER_H
