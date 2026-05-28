QT       += core gui widgets charts network printsupport

TARGET = PersonalDevPlan
TEMPLATE = app

macx {
    QMAKE_LIBS_OPENGL = -framework OpenGL
    ICON = icons/app.icns
}

SOURCES += \
    main.cpp \
    model/courserecord.cpp \
    model/achievement.cpp \
    model/education.cpp \
    model/skill.cpp \
    model/experience.cpp \
    model/studentprofile.cpp \
    model/gpacalculator.cpp \
    model/databasemanager.cpp \
    model/resumegenerator.cpp \
    model/accountmanager.cpp \
    service/aiassistantservice.cpp \
    controller/importcontroller.cpp \
    controller/gpacontroller.cpp \
    controller/achievementcontroller.cpp \
    controller/resumecontroller.cpp \
    controller/aiassistantcontroller.cpp \
    view/mainwindow.cpp \
    view/logindialog.cpp \
    view/importview.cpp \
    view/gpaview.cpp \
    view/profileview.cpp \
    view/resumeview.cpp \
    view/adminpanel.cpp \
    view/aiassistantview.cpp

HEADERS += \
    model/courserecord.h \
    model/achievement.h \
    model/education.h \
    model/skill.h \
    model/experience.h \
    model/studentprofile.h \
    model/gpacalculator.h \
    model/databasemanager.h \
    model/resumegenerator.h \
    model/accountmanager.h \
    service/aiassistantservice.h \
    controller/importcontroller.h \
    controller/gpacontroller.h \
    controller/achievementcontroller.h \
    controller/resumecontroller.h \
    controller/aiassistantcontroller.h \
    view/mainwindow.h \
    view/logindialog.h \
    view/importview.h \
    view/gpaview.h \
    view/profileview.h \
    view/resumeview.h \
    view/adminpanel.h \
    view/aiassistantview.h
