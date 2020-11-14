#ifndef EDITPROPERTY_H
#define EDITPROPERTY_H

#include <QDialog>

namespace Ui {
class EditProperty;
}

class EditProperty : public QDialog
{
    Q_OBJECT

public:
    explicit EditProperty(QWidget *parent = nullptr);
    ~EditProperty();

    void addPropertyToEdit(const QString &content);

signals:
    void modifierPropriete(const QString &prop);



private slots:
    void on_modifyProperty_clicked();

    void on_removeAll_clicked();



private:
    Ui::EditProperty *ui;
};

#endif // EDITPROPERTY_H
