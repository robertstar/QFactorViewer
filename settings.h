#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>


namespace Ui {
class Settings;
}

class Settings : public QMainWindow
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

signals:
    void setConfigs(int, QTableWidget *tabl);

private slots:
    void Apply();
    void onTableClicked(const QModelIndex &index);
    void TableDelRow();
    void TableAddRow();

private:
    Ui::Settings    *ui;

    QPushButton     *btn_ok;
    QPushButton     *btn_cnl;
    QLineEdit       *LineEdit_min_h;

    QAction         *actAddRow;
    QAction         *actDelRow;
    QTableWidget    *config;

    int             row_cnt;
    int             ip_cnt;
};

#endif // SETTINGS_H
