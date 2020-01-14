#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include <QQuickStyle>
#include <QQmlContext>
#include "myimagesaver.h"

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    MyImageSaver myImageSaver;

    engine.rootContext()->setContextProperty("MyImageSaver", &myImageSaver);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    //engine.load(QUrl(QString("qrc:/main.qml")));

    return app.exec();
}
