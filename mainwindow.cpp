#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QFileDialog"
#include "QDir"
#include "QtSql"
#include "QDesktopServices"    //以默认打开方式打开
#include <Windows.h>
#include "libzplay.h"
using namespace libZPlay;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置样式
    QFont font;
    font.setFamily("Microsoft YaHei"); //
    qApp->setFont(font);

    //应用样式 apply the qss style
    QFile file(":/test.qss");
    if(!file.open(QFile::ReadOnly)) QMessageBox::about(this,"Error","读取样式失败！");
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();

    //显示数据表
    model = new QSqlTableModel(this);
    model->setTable("media");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select(); //选取整个表的所有行
    //不显示name属性列,如果这时添加记录，则该属性的值添加不上
    // model->removeColumn(1);
    ui->tableView->setModel(model);
    //使其不可编辑
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //由内容调整列
    //ui->tableView->resizeColumnsToContents();
    ui->tableView->setColumnWidth(0,30);
    ui->tableView->setColumnWidth(1,300);
    ui->tableView->setColumnWidth(2,400);

    //文件类型下拉框
    ui->comboBox_fileType->addItem("全部文件");
    ui->comboBox_fileType->addItem("文档");
    ui->comboBox_fileType->addItem("图片");
    ui->comboBox_fileType->addItem("音频");
    ui->comboBox_fileType->addItem("视频");

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
    qDebug("sousuo");
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
    ui->tableView->resizeColumnsToContents();//由内容调整列
}
void MainWindow::search()
{
    QString fileName = ui->lineEdit->text();
    int filetype = ui->comboBox_fileType->currentIndex();
    QString filter;
    /*
    switch (filetype)
    {
    case 0:QMessageBox::about(this,"1","all");filter = QString("name like '%%1%'").arg(fileName);break;
    case 1:QMessageBox::about(this,"1","document");filter = QString("name like '%%1%.txt'").arg(fileName);break;
    case 2:QMessageBox::about(this,"1","picture");filter = QString("name like '%%1%.png' or name like '%%1%.jpg'").arg(fileName);QMessageBox::about(this,"1",filter);break;
    case 3:QMessageBox::about(this,"1","adio");filter = QString("name like '%%1%.mp3'").arg(fileName);break;
    case 4:QMessageBox::about(this,"4","video");filter = QString("name like '%%1%.mp4'").arg(fileName);break;
    }
    */
    switch (filetype)
    {
    case 0:filter = QString("name like '%%1%'").arg(fileName);break;
    case 1:filter = QString("name like '%%1%.txt'").arg(fileName);break;
    case 2:filter = QString("name like '%%1%.png' or name like '%%1%.jpg'").arg(fileName);break;
    case 3:filter = QString("name like '%%1%.mp3'").arg(fileName);break;
    case 4:filter = QString("name like '%%1%.mp4'").arg(fileName);break;
    }
    model->setFilter(filter);
    model->select();
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
        //查询文件信息
        QString title,artist,album;
        ZPlay* player= CreateZPlay();
        TID3InfoEx id3_info;
        if(player == 0)
        {
          qDebug()<<"Error: Can't create ZPlay class instance !";
        }
         if(player->OpenFile(path.toLocal8Bit(),sfAutodetect))
         {
             if(player->LoadID3Ex(&id3_info,1))
             {
                 title = QString::fromLocal8Bit(id3_info.Title);
                 artist = QString::fromLocal8Bit(id3_info.Artist);
                 album = QString::fromLocal8Bit(id3_info.Album);
                 //picture = id3_info.Picture;
             }
         }
        //添加
        query.exec(QString("insert into media values(%1,'%2','%3','%4','%5','%6')").arg(id).arg(fileName).arg(path).arg(title).arg(artist).arg(album));
        //query.exec(QString("insert into media values(%1,'%2','%3')").arg(id).arg(fileName).arg(path));
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
    QString pathBack,exeBack;
    //获取要打开的文件路径path
    int curRow = ui->tableView->currentIndex().row();
    QModelIndex index=model->index(curRow,2,QModelIndex());//rowNum,columnNum为行列号
    QString path= index.data().toString();

    //处理path的类型
    path = path.replace(QRegExp("\\/"),"\\\\");//左右斜杠转换
    path = QString("\"%1\"").arg(path);  //加上引号
    std::wstring pathW = path.toStdWString();
    LPCWSTR pathL = pathW.c_str();  //转换为LPCWSTR

    //选择打开方式
    QString exe = QFileDialog::getOpenFileName(this,tr("请选择打开方式"),"C:/ProgramData/Microsoft/Windows/Start Menu/Programs/Accessories");
    exe = exe.replace(QRegExp("\\/"),"\\\\");//左右斜杠转换
    //exe =QString("\"%1\"").arg(exe);   //加上引号
    //QMessageBox::information(NULL, "Title", path, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    LPCWSTR exeL = exe.toStdWString().c_str();  //转换为LPCWSTR
    //LPCWSTR exeL = QString::toStdWString(exe);
    //exeL =TEXT("C:\\\\Program Files\\\\Windows NT\\\\Accessories\\\\wordpad.exe");

    //打开该文件
    LPCWSTR openL = TEXT("open");
    //LPCWSTR txtL = TEXT("D:\\\\软件目录.txt");
    //LPCWSTR wordpadL = TEXT("C:\\\\Program Files\\\\Windows NT\\\\Accessories\\\\wordpad.exe");
    //exeL = TEXT("");
    ShellExecuteW(NULL,openL,exeL,pathL,NULL,SW_SHOWNORMAL);

    /*
    pathBack = QString::fromStdWString(pathL);
    QMessageBox::about(NULL, "pathBack", pathBack);
    exeBack = QString::fromStdWString(exeL);
    QMessageBox::about(NULL, "exeBack", exeBack);
    */
}
void MainWindow::detail()
{
    QString filePath,title,artist,album,picture;
    //获取id
    int curRow = ui->tableView->currentIndex().row(); //获取选中的行
    QModelIndex index=model->index(curRow,0,QModelIndex());//rowNum,columnNum为行列号
    int id= index.data().toInt();
    //获取文件信息
    QSqlQuery query;
    QString sql = QString("select * from media where id = %1").arg(id);
    query.exec(sql);
    query.first() ;
    filePath = query.value(0).toString() ;
    title = query.value(3).toString();
    artist = query.value(4).toString();
    album = query.value(5).toString();

    //查找文件信息
//    ZPlay* player= CreateZPlay();
//    TID3InfoEx id3_info;
//    if(player == 0)
//    {
//      qDebug()<<"Error: Can't create ZPlay class instance !";
//    }
//     if(player->OpenFile(filePath.toLocal8Bit(),sfAutodetect))
//     {
//         if(player->LoadID3Ex(&id3_info,1))
//         {
//             title = QString::fromLocal8Bit(id3_info.Title);
//             artist = QString::fromLocal8Bit(id3_info.Artist);
//             album = QString::fromLocal8Bit(id3_info.Album);
//             //picture = id3_info.Picture;
//         }
//     }
    //显示文件信息
    QString content="标题："+title;
    content += "\r\n艺术家：" +artist;
    content += "\r\n专辑：" +album;
    QMessageBox::about(this,"详细信息",content);
};
//void MainWindow::on_menu_click(bool ){;}
void MainWindow::on_tableView_customContextMenuRequested(const QPoint &pos)  //右键条目
{
    QMenu *cmenu = new QMenu(ui->tableView);
    QAction *open = cmenu->addAction("打开");
    QAction *wayToOpen = cmenu->addAction("打开方式");
    QAction *detail = cmenu->addAction("详细信息");
    connect(open, SIGNAL(triggered(bool)), this, SLOT(on_action_open_triggered()));
    connect(wayToOpen, SIGNAL(triggered(bool)), this, SLOT(on_action_way_to_open_triggered()));
    connect(detail, SIGNAL(triggered(bool)), this, SLOT(on_action_detail_triggered()));
    cmenu->exec(QCursor::pos());
}

void MainWindow::on_action_add_triggered()
{
    add();
}

void MainWindow::on_action_delete_triggered()
{
    deleteItem();
}

void MainWindow::on_action_open_triggered()
{
    open();
}

void MainWindow::on_action_way_to_open_triggered()
{
    wayToOpen();
}

void MainWindow::on_action_show_all_triggered()
{
    showAll();
}

void MainWindow::on_action_search_triggered()
{
    search();
}

void MainWindow::on_action_detail_triggered()
{
    detail();
}
