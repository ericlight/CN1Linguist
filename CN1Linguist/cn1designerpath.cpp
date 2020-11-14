#include "cn1designerpath.h"
#include "ui_cn1designerpath.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>
#include <QMessageBox>

CN1DesignerPath::CN1DesignerPath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CN1DesignerPath)
{
    ui->setupUi(this);
    setDesignerPath();
    setJavaJDKPath();
}

CN1DesignerPath::~CN1DesignerPath()
{
    delete ui;
}

void CN1DesignerPath::on_close_clicked()
{
    close();
}

void CN1DesignerPath::on_chooseFile_clicked()
{
    QString fileName=QFileDialog::getOpenFileName(this,tr("Choose Codename One Designer file"),".",tr("JAR file (*.jar)"));
    if(!fileName.isEmpty()){
        ui->cn1designerPath->setText(fileName);
    }
}

void CN1DesignerPath::on_savePath_clicked()
{
    QFileInfo info(ui->cn1designerPath->text().trimmed());
    if(info.fileName()!="designer_1.jar"){
        QMessageBox::warning(this,tr("Wrong path!"),tr("Your path is wrong. You have to choose <b>designer_1.jar</b> file path."));
        return;
    }

    QString javaFolderPath=ui->javaPath->text().trimmed()+"/bin/java.exe";
    QFile javaPath(javaFolderPath);
    if(!javaPath.exists()){
        QMessageBox::warning(this,tr("Wrong path!"), tr("Your path is wrong. You have to choose <b>Java JDK folder</b> path"));
        return;
    }

    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    settings.setValue("cn1designer_path", ui->cn1designerPath->text().trimmed());
    settings.setValue("java_path", ui->javaPath->text().trimmed());
    settings.endGroup();
    close();
}

void CN1DesignerPath::setDesignerPath()
{
    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    QString designerPath=settings.value("cn1designer_path").toString();
    settings.endGroup();

    ui->cn1designerPath->setText(QDir::toNativeSeparators(designerPath));
}

void CN1DesignerPath::setJavaJDKPath()
{
    QSettings settings("cn1ling-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    QString javaPath=settings.value("java_path").toString();
    settings.endGroup();

    ui->javaPath->setText(QDir::toNativeSeparators(javaPath));
}

void CN1DesignerPath::on_chooseJavaLocation_clicked()
{
    QString cheminDossierJDK=QFileDialog::getExistingDirectory(this,tr("Choose Java JDK folder"),".");
    if(!cheminDossierJDK.isEmpty()){
        ui->javaPath->setText(cheminDossierJDK);
    }
}
