#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include "view/logindialog.h"
#include "view/mainwindow.h"

int main(int argc, char *argv[])
{
    // 启用 HiDPI 缩放，修复高分辨率屏幕比例问题
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication a(argc, argv);
    a.setApplicationName("PersonalDevPlan");
    a.setOrganizationName("StudentDev");

    // 确保数据目录存在（initDefaultData 在 DatabaseManager 构造时自动调用）
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);

    // 登录循环：退出登录后回到登录页面，退出程序才真正关闭
    while (true) {
        LoginDialog login;
        if (login.exec() != QDialog::Accepted)
            break;  // 用户点退出

        MainWindow w(login.username(), login.role());
        w.show();
        a.exec();  // 进入主窗口事件循环，w.close() 后返回
    }

    return 0;
}
