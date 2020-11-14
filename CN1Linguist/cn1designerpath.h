#ifndef CN1DESIGNERPATH_H
#define CN1DESIGNERPATH_H

#include <QDialog>

namespace Ui {
class CN1DesignerPath;
}

class CN1DesignerPath : public QDialog
{
    Q_OBJECT

public:
    explicit CN1DesignerPath(QWidget *parent = nullptr);
    ~CN1DesignerPath();

private slots:
    void on_close_clicked();

    void on_chooseFile_clicked();

    void on_savePath_clicked();

    void on_chooseJavaLocation_clicked();

private:
    Ui::CN1DesignerPath *ui;

    void setDesignerPath();    
    void setJavaJDKPath();
};

#endif // CN1DESIGNERPATH_H
