#include "confirmtool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConfirmTool w;
    w.show();
    return a.exec();
}
