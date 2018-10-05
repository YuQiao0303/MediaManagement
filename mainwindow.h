#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //自定义函数
    void deleteItem();
    void showAll();
    void search();
    void add();
    void open();
    void wayToOpen();

private slots:
    //void on_pushButton_3_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_showAll_clicked();

    void on_pushButton_search_clicked();

    void on_pushButton_add_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_open_clicked();

    void on_pushButton_wayToOpen_clicked();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
};

#endif // MAINWINDOW_H
