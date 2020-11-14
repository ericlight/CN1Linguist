#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDir>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    bool checked=settings.value("ThemeFusion").toBool();
    settings.endGroup();

    if(checked==true){
        a.setStyle("fusion");
    }

    MainWindow w;

    if(argc>1){
        QString chemin=argv[1];
        if(chemin=="."){
            chemin=QDir::currentPath()+"/src";
        }
        w.ouvrirProjetViaCmd(chemin);
    }

    QScreen *screen=qApp->primaryScreen();
    QRect dimensionsEcran=screen->geometry();
    if(dimensionsEcran.width()<=1366 && dimensionsEcran.height()<=768){
        w.showMaximized();
    } else {
        w.show();
    }

    return a.exec();
}
