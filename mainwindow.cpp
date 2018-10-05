#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QFileDialog"
#include "QDir"
#include "QtSql"
#include "QDesktopServices"    //以默认打开方式打开
#include <Windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QSqlTableModel(this);
    model->setTable("media");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select(); //选取整个表的所有行

    //不显示name属性列,如果这时添加记录，则该属性的值添加不上
    // model->removeColumn(1);

    ui->tableView->setModel(model);

    //使其不可编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_delete_clicked()  //删除所选行
{
   deleteItem();
}

void MainWindow::on_pushButton_showAll_clicked()   //显示全表
{
    showAll();
}

void MainWindow::on_pushButton_search_clicked()   //搜索
{
    search();
}

void MainWindow::on_pushButton_add_clicked()    //添加新文件
{
    add();
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index) //双击打开
{
    open();
}

void MainWindow::on_pushButton_open_clicked() //打开（按默认方式）
{
    open();
}

void MainWindow::on_pushButton_wayToOpen_clicked() //打开方式
{
    wayToOpen();
}


void MainWindow::deleteItem()
{
    //获取选中的行
    int curRow = ui->tableView->currentIndex().row();
    //删除该行
    model->removeRow(curRow);

    int ok = QMessageBox::warning(this,tr("删除当前行!"),
                                  tr("你确定删除当前行吗？"),
                                  QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::No)
    {
        model->revertAll(); //如果不删除，则撤销
    }
    else model->submitAll(); //否则提交，在数据库中删除该行
}
void MainWindow::showAll()
{
    model->setTable("media");   //重新关联表
    model->select();   //这样才能再次显示整个表的内容
}
void MainWindow::search()
{
    QString fileName = ui->lineEdit->text();
    model->setFilter(QString("name like '%%1%'").arg(fileName));
    model->select();    //之后就将显示该名字的人
}
void MainWindow::add()
{
    QString path = QFileDialog::getOpenFileName(this,tr("请选择要导入的文件"),QDir::currentPath());
    if(!path.isEmpty())
    {
        //添加该文件记录
        QSqlQuery query;
        //主键
        query.exec("select max(id) from media");
        query.first() ;
        int id = query.value(0).toInt() + 1;
        //文件名和文件路径
        QFileInfo fileInfo(path);
        QString fileName = fileInfo.fileName();

        //添加
        query.exec(QString("insert into media values(%1,'%2','%3')").arg(id).arg(fileName).arg(path));
        model->select();
    }
}
void MainWindow::open()
{
    //获取选中的行
    int curRow = ui->tableView->currentIndex().row();
    //获取该行第二列信息，即path
    QModelIndex index=model->index(curRow,2,QModelIndex());//rowNum,columnNum为行列号
    QString path= index.data().toString();
    //打开该文件
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()));
}
void MainWindow::wayToOpen()
{
    //获取要打开的文件路径path
    int curRow = ui->tableView->currentIndex().row();
    QModelIndex index=model->index(curRow,2,QModelIndex());//rowNum,columnNum为行列号
    QString path= index.data().toString();

    //处理path的类型
    path = path.replace(QRegExp("\\/"),"\\\\");//左右斜杠转换
    std::wstring pathW = path.toStdWString();
    LPCWSTR pathL = pathW.c_str();  //转换为LPCWSTR
    QString pathBack = QString::fromStdWString(pathL);
    QMessageBox::about(NULL, "pathBack", pathBack);

    //选择打开方式
    QString exe = QFileDialog::getOpenFileName(this,tr("请选择打开方式"),"C:/ProgramData/Microsoft/Windows/Start Menu/Programs/Accessories");
    exe = exe.replace(QRegExp("\\/"),"\\\\");//左右斜杠转换

    //QMessageBox::information(NULL, "Title", path, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    LPCWSTR exeL = exe.toStdWString().c_str();  //转换为LPCWSTR
    //LPCWSTR exeL = QString::toStdWString(exe);
    //exeL =TEXT("C:\\\\Program Files\\\\Windows NT\\\\Accessories\\\\wordpad.exe");
    QString exeBack = QString::fromStdWString(exeL);
    //QMessageBox::about(NULL, "exeBack", exeBack);


    //打开该文件
    LPCWSTR openL = TEXT("open");
    //LPCWSTR txtL = TEXT("D:\\\\软件目录.txt");
    //LPCWSTR wordpadL = TEXT("C:\\\\Program Files\\\\Windows NT\\\\Accessories\\\\wordpad.exe");
    ShellExecuteW(NULL,openL,exeL,pathL,NULL,SW_SHOWNORMAL);
}
