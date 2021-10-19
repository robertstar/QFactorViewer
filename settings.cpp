#include "settings.h"
#include "ui_settings.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <QStyle>
#include <QDesktopWidget>
#include <QHeaderView>
#include <QtWidgets>
#include <QAbstractItemView>

#include "delegate.h"
#include "mainwindow.h"


Settings::Settings(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    row_cnt=0;
    ip_cnt=0;


    this->window()->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->window()->size(),
            qApp->desktop()->availableGeometry()
        )
    );




    QToolBar *ToolBar = addToolBar(tr("Панель инструментов"));
    //actAddRow = new QAction(tr("&Добавить модуль СПИ"), this);
    actAddRow = new QAction(tr("&Добавить"), this);
    actAddRow->setIcon(QIcon(":/images/add2.png"));
    connect(actAddRow, &QAction::triggered, this, &Settings::TableAddRow);

    //actDelRow = new QAction(tr("&Удалить модуль СПИ"), this);
    actDelRow = new QAction(tr("&Удалить"), this);
    actDelRow->setIcon(QIcon(":/images/delete1.png"));
    actDelRow->setEnabled(false);
    connect(actDelRow, &QAction::triggered, this, &Settings::TableDelRow);


    ToolBar->setIconSize(QSize(30, 30));
    ToolBar->addAction(actAddRow);
    ToolBar->addAction(actDelRow);
    //ToolBar->addWidget(edit);
    ToolBar->setContextMenuPolicy(Qt::PreventContextMenu);



    //auto flags = windowFlags();//save current configuration
    //your main configuration which do the trick
    setWindowFlags( (Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint) & (~Qt::WindowMaximizeButtonHint) &(~Qt::WindowMinimizeButtonHint) );
    //setWindowFlags(Qt::Window);
    //setWindowFlags(flags);//restore Qt::Window


    QStringList JEheader, TPheader;
    config = new QTableWidget();
    // Create table of parametrs
    config->setSelectionBehavior(QAbstractItemView::SelectRows);
    config->setItemDelegate(new Delegate);
    //config->setRowCount(2);
    config->setColumnCount(2);
    //TPheader<<"АЦП"<<"IP-адрес СПИ";
    TPheader<<"АЦП"<<"IP-адрес";
    config->setHorizontalHeaderLabels(TPheader);
    config->verticalHeader()->setVisible(false);
    config->horizontalHeader()->setVisible(true);
    //config->horizontalHeader()->setResizeMode( 1, QHeaderView::Stretch );
    //config->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    config->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(config, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));


    // Main layouts
    QVBoxLayout *vLay = new QVBoxLayout(ui->centralwidget);
    QHBoxLayout *hLay = new QHBoxLayout();

    btn_ok = new QPushButton( QString( "%1" ).arg( 1 ) );
    btn_ok->setFixedSize(QSize(120, 30));
    btn_ok->setText("Применить");


    //LineEdit_min_h = new QLineEdit();
    //QRegExp ipRegex("^[1-3]{1}[0-9]{1}[0-9]{1}$");
    //QRegExpValidator *Validator = new QRegExpValidator(ipRegex, LineEdit_min_h);
    //LineEdit_min_h->setValidator(Validator);
    //LineEdit_min_h->setMaximumWidth(50);
    //LineEdit_min_h->setText("100");

    hLay->addWidget(config);
    //vLay->addWidget(LineEdit_min_h);
    vLay->addLayout(hLay);
    vLay->addWidget(btn_ok);
    connect(btn_ok, &QPushButton::clicked, this, &Settings::Apply);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::Apply()
{
    emit setConfigs(1, config);
    hide();
}

void Settings::onTableClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString cellText = index.data().toString();
        qDebug() <<  "cellText: " << cellText;
        actDelRow->setEnabled(true);
    }
}

void Settings::TableDelRow()
{
    if(config->rowCount() > 0){
       config->removeRow(config->currentRow());
       qDebug() <<  "rowCount: " << QString::number(config->rowCount());
       if(config->rowCount()==0){
           actDelRow->setEnabled(false);
           row_cnt=0;
           ip_cnt=0;
       }
    }
}

void Settings::TableAddRow()
{
    qDebug() <<  "TableAddRow rowCount: " << QString::number(config->rowCount());



    config->insertRow(config->rowCount());
    //config->setItem  (config->rowCount()-1, 0, new QTableWidgetItem("Канал [ " + QString::number(row_cnt+1) + " - " + QString::number(row_cnt+32) + " ]" ));
    config->setItem  (config->rowCount()-1, 0, new QTableWidgetItem("Канал [ " + QString::number(row_cnt+1) + " - " + QString::number(row_cnt+85) + " ]" ));
    config->item     (config->rowCount()-1, 0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    config->item     (config->rowCount()-1, 0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    config->setItem  (config->rowCount()-1, 1, new QTableWidgetItem("192.168.0." + QString::number(ip_cnt+2)));
    config->item     (config->rowCount()-1, 1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    //row_cnt+=32;
    row_cnt+=85;
    ip_cnt+=1;
}


