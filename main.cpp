#include "dockerwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DockerWindow w;
    w.show();
    return a.exec();
}
