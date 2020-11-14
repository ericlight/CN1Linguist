#ifndef SAVECN1RESFILE_H
#define SAVECN1RESFILE_H

#include <QDialog>

namespace Ui {
class SaveCN1ResFile;
}

class SaveCN1ResFile : public QDialog
{
    Q_OBJECT

public:
    explicit SaveCN1ResFile(QWidget *parent = nullptr);
    ~SaveCN1ResFile();

    void setResFilePath(QString path);

private slots:
    void on_close_clicked();

    void on_chooseResFile_clicked();

    void on_save_clicked();

signals:
    void save(QString fileName);

private:
    Ui::SaveCN1ResFile *ui;
    QString fileName;
};

#endif // SAVECN1RESFILE_H
