
#include <QApplication>
#include "MainWindow.h"
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/app.ico"));

    MainWindow window;
    window.show();

    return app.exec();
}
