#include <QtWidgets/QApplication>

#include "core/controller/main-widget.h"


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWidget main_widget;
    main_widget.show();
    return app.exec();
}
