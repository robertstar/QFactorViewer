/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QString::fromUtf8("Settings"));
        Settings->resize(800, 300);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icons8-services-100.png"), QSize(), QIcon::Normal, QIcon::Off);
        Settings->setWindowIcon(icon);
        centralwidget = new QWidget(Settings);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        Settings->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Settings);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        Settings->setMenuBar(menubar);
        statusbar = new QStatusBar(Settings);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        Settings->setStatusBar(statusbar);

        retranslateUi(Settings);

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QMainWindow *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
