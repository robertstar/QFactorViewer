#ifndef DELEGATE_H
#define DELEGATE_H

#include <QItemDelegate>
#include <QLineEdit>
#include <QRegExpValidator>

class Delegate : public QItemDelegate
{
public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                      const QModelIndex & index) const
    {
        QLineEdit *lineEdit = new QLineEdit(parent);
        // Set validator


        QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
        // You may want to use QRegularExpression for new code with Qt 5 (not mandatory).
        QRegExp ipRegex ("^" + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange + "$");
        QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, lineEdit);
        //QIntValidator *validator = new QIntValidator(0, 9, lineEdit);
        lineEdit->setValidator(ipValidator);
        return lineEdit;
    }
};

#endif // DELEGATE_H
