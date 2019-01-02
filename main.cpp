#include "mainwindow.h"
#include <QApplication>
#include "connection.h"
#include "logindialog.h"
#include <QFile>
#include <QDebug>
#include <QMessageBox>
extern QString userName;
extern QString password;
extern bool isAdmin;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if(!createConnection())   //数据库连接  / 其他数据库操作
    {
        return 1;
        qDebug() <<"cannot connect to mysql";
    }
    LoginDialog dlg;                        // 建立自己新建的LoginDlg类的实例dlg
        if(dlg.exec() == QDialog::Accepted && isAdmin==true) // 管理员登录按钮被按下
        {
            qDebug() <<"admin button pushed";
            bool loginSuccess=false;
            //查找admin表
            QSqlQuery query;
            if(!query.exec("select userName,password from admin"))
                qDebug() <<"error while selecting";
            while(query.next())
            {
                QString username_tmp = query.value(0).toString();
                qDebug() <<"username_tmp:  "<<username_tmp;
                if(userName==username_tmp ) //有这个用户名
                {
                    QString password_tmp = query.value(1).toString();
                    qDebug() <<"password_tmp: "<<password_tmp;
                    if(password_tmp==password)  //用户名和密码均匹配，登录成功
                    {
                        loginSuccess=true;
                        qDebug() << username_tmp << password_tmp ;
                    }
                }
            }
            if(loginSuccess==true)  //判断管理员用户名和密码是否正确
            {
                qDebug() <<"loginSuccess";
                MainWindow w;
                w.show();                      // 如果被按下，显示主窗口
                return a.exec();              // 程序一直执行，直到主窗口关闭
            }
            else
            {
                QMessageBox::about(NULL,"温馨提示","用户名或密码错误！");
            }
        }
        else if(dlg.exec() == QDialog::Accepted && isAdmin==false) // 游客直接访问按钮被按下
        {
            MainWindow w;
            w.show();                      // 如果被按下，显示主窗口
            return a.exec();              // 程序一直执行，直到主窗口关闭
        }
        else return 0;          //如果没有被按下，则不会进入主窗口，整个程序结束运行
}
