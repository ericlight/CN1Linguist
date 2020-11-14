#include "editproperty.h"
#include "ui_editproperty.h"

EditProperty::EditProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditProperty)
{
    ui->setupUi(this);
}

EditProperty::~EditProperty()
{
    delete ui;
}

void EditProperty::addPropertyToEdit(const QString &content)
{
    ui->propertyContent->setPlainText(content);
}

void EditProperty::on_modifyProperty_clicked()
{
    emit modifierPropriete(ui->propertyContent->toPlainText());
}

void EditProperty::on_removeAll_clicked()
{
    ui->propertyContent->clear();
}
