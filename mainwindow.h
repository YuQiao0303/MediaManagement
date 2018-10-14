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
    void detail();

private slots:
    //void on_pushButton_3_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_showAll_clicked();

    void on_pushButton_search_clicked();

    void on_pushButton_add_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_open_clicked();

    void on_pushButton_wayToOpen_clicked();

    void on_tableView_customContextMenuRequested(const QPoint &pos);
    //void on_menu_click(bool );
    void on_action_add_triggered();

    void on_action_delete_triggered();

    void on_action_open_triggered();

    void on_action_way_to_open_triggered();

    void on_action_show_all_triggered();

    void on_action_search_triggered();

    void on_action_detail_triggered();

private:
    Ui::MainWindow *ui;
    QSqlTableModel *model;
};

#endif // MAINWINDOW_H
