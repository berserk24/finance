#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8")); //изменения

    QLocale::setDefault(QLocale::Russian);
    QApplication a(argc, argv);
    QTranslator *qt_translator = new QTranslator;
    if ( qt_translator->load( ":lang/qt_ru.qm" ) )
    {
        a.installTranslator( qt_translator );
    }
    Widget w;

    return a.exec();
}
