#include "savecn1resfile.h"
#include "ui_savecn1resfile.h"
#include <QFileDialog>
#include <QMessageBox>

SaveCN1ResFile::SaveCN1ResFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveCN1ResFile)
{
    ui->setupUi(this);
}

SaveCN1ResFile::~SaveCN1ResFile()
{
    delete ui;
}

void SaveCN1ResFile::on_close_clicked()
{
    close();
}

void SaveCN1ResFile::on_chooseResFile_clicked()
{
    fileName=QFileDialog::getOpenFileName(this,tr("Choose Codename One Resource file"),".",tr("Codename One RES file (*.res)"));

    if(!fileName.isEmpty()){
        ui->resourceFilePath->setText(fileName);
    }
}

void SaveCN1ResFile::on_save_clicked()
{
    if(fileName.isEmpty()){
        QMessageBox::warning(this,tr("File missed!"),tr("You have to choose the resource file before"));
        return;
    }
    emit save(fileName);

    close();
}

void SaveCN1ResFile::setResFilePath(QString path)
{
    fileName=path;
    ui->resourceFilePath->setText(fileName);
}
