#include "mainwindow.h"

#include <QApplication>

#if defined(__MINGW32__)
extern "C" {
    int _argc = 0;
    char **_argv = nullptr;
    int *__imp___argc = &_argc;
    char ***__imp___argv = &_argv;
}
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
